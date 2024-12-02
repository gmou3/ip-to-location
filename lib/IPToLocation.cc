#include "IPToLocation.hh"

#include <curl/curl.h>
#include <libdeflate.h>
#include <maxminddb.h>

#include <chrono>
#include <cstddef>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Callback function for `curl_easy_setopt` to write data to a file
size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

// Function to decompress the mmdb.gz file after download
void decompress_gz_file(std::string filename)
{
    std::ifstream gz_file(filename, std::ios::binary);
    if (!gz_file)
    {
        std::cerr << "Error opening " + filename + " to perform decompression" << std::endl;
        return;
    }

    // Read the entire .gz file into a buffer
    std::vector<char> gz_data((std::istreambuf_iterator<char>(gz_file)), std::istreambuf_iterator<char>());

    // Initialize decompressor
    libdeflate_decompressor* decompressor = libdeflate_alloc_decompressor();
    if (decompressor == nullptr)
    {
        std::cerr << "libdeflate: Failed to create decompressor" << std::endl;
        return;
    }

    // Prepare buffer to hold decompressed data (estimate maximum decompressed size)
    size_t decompressed_size = gz_data.size() * 4;
    std::vector<char> decompressed_data(decompressed_size);

    // Perform the decompression
    size_t decompressed_len;
    int result = libdeflate_gzip_decompress(
        decompressor,
        gz_data.data(),
        gz_data.size(),
        decompressed_data.data(),
        decompressed_size,
        &decompressed_len);

    if (result != LIBDEFLATE_SUCCESS)
    {
        std::cerr << "libdeflate: Decompression of " + filename + " failed" << std::endl;
    }
    else
    {
        // Write decompressed data to the output file
        filename.erase(filename.size() - 3, 3); // erase ".gz" suffix
        std::ofstream output_file(filename, std::ios::binary);
        if (!output_file)
        {
            std::cerr << "Error opening " + filename + " to save decompression result" << std::endl;
        }
        else
        {
            output_file.write(decompressed_data.data(), decompressed_len);
        }
    }
    libdeflate_free_decompressor(decompressor);
}

// Function to download and update the mmdb file
// Free database with monthly updates from <https://db-ip.com/>
void maintain_mmdb_file(std::string mmdb_file)
{
    // UTC year and month
    using namespace std::chrono;
    time_t now = time(0);
    struct tm *utc_time = gmtime(&now);
    std::string year = std::to_string(utc_time->tm_year + 1900);
    std::string month = std::to_string(utc_time->tm_mon + 1);
    if (month.size() == 1)
    {
        month = "0" + month; // pad
    }

    std::string url = "";
    if (!std::filesystem::exists(mmdb_file))
    {
        url = "https://download.db-ip.com/free/dbip-city-lite-" + year + "-" + month + ".mmdb.gz";
    }
    else
    {
        MMDB_s mmdb;
        int status = MMDB_open(mmdb_file.c_str(), MMDB_MODE_MMAP, &mmdb);
        if (MMDB_SUCCESS != status)
        {
            fprintf(
                stderr,
                "libmaxminddb: Error while opening database file (%s, %s)\n",
                mmdb_file.c_str(),
                MMDB_strerror(status));
            url = "https://download.db-ip.com/free/dbip-city-lite-" + year + "-" + month + ".mmdb.gz";
        }
        else
        {
            // Existing database file's year and month
            time_t build_epoch = mmdb.metadata.build_epoch;
            struct tm *mmdb_utc_time = gmtime(&build_epoch);
            std::string mmdb_year = std::to_string(mmdb_utc_time->tm_year + 1900);
            std::string mmdb_month = std::to_string(mmdb_utc_time->tm_mon + 1);
            if (mmdb_month.size() == 1)
            {
                mmdb_month = "0" + mmdb_month; // pad
            }
            if (month != mmdb_month || year != mmdb_year)
            {
                url = "https://download.db-ip.com/free/dbip-city-lite-" + year + "-" + month + ".mmdb.gz";
            }
        }
        MMDB_close(&mmdb);
    }

    if (url == "") // no need for a download
    {
        return;
    }

    // Download mmdb.gz file
    CURL* curl = curl_easy_init();
    if (curl)
    {
        FILE* fp;
        fp = fopen((mmdb_file + ".gz").c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl); // always cleanup
        fclose(fp);

        if (res != CURLE_OK)
        {
            fprintf(
                stderr,
                "libcurl: Error downloading %s to %s - %s\n",
                url.c_str(),
                (mmdb_file + ".gz").c_str(),
                curl_easy_strerror(res));
            return;
        }

        // Decompress mmdb.gz file
        decompress_gz_file(mmdb_file + ".gz");
    }
}

std::string get_location_from_ip(std::string ip)
{
    // Get the home directory from environment variables
    const char* home_dir = std::getenv("HOME");  // Linux/macOS
    if (!home_dir) {
        home_dir = std::getenv("USERPROFILE");  // Windows
    }
    if (!home_dir)
    {
        std::cerr << "Could not find the home directory" << std::endl;
        return "";
    }
    std::filesystem::path home_path(home_dir);

    // Database file path
    std::filesystem::path mmdb_file_path = home_path / "dbip-city-lite.mmdb";
    std::string mmdb_file = mmdb_file_path.string();

    maintain_mmdb_file(mmdb_file);

    // Compute location from IP
    MMDB_s mmdb;
    int status = MMDB_open(mmdb_file.c_str(), MMDB_MODE_MMAP, &mmdb);
    if (MMDB_SUCCESS != status)
    {
        fprintf(stderr, "libmaxminddb: Error while opening database file (%s, %s)\n", mmdb_file.c_str(), MMDB_strerror(status));
        return "";
    }
    int gai_error, mmdb_error;
    MMDB_lookup_result_s result = MMDB_lookup_string(&mmdb, ip.c_str(), &gai_error, &mmdb_error);
    if (0 != gai_error || MMDB_SUCCESS != mmdb_error || !result.found_entry)
    {
        fprintf(stderr, "libmaxminddb: Error while looking up IP %s\n", ip.c_str());
        MMDB_close(&mmdb);
        return "";
    }
    MMDB_entry_data_s entry_data;
    status = MMDB_get_value(&result.entry, &entry_data, "city", "names", "en", NULL);
    if (MMDB_SUCCESS != status || !entry_data.has_data)
    {
        fprintf(stderr, "libmaxminddb: Error while getting value of city for IP %s\n", ip.c_str());
        MMDB_close(&mmdb);
        return "";
    }
    std::string location = "";
    location.append(entry_data.utf8_string, entry_data.utf8_string + entry_data.data_size);
    location += ", ";
    status = MMDB_get_value(&result.entry, &entry_data, "country", "names", "en", NULL);
    if (MMDB_SUCCESS != status || !entry_data.has_data)
    {
        fprintf(stderr, "libmaxminddb: Error while getting value of country for IP %s\n", ip.c_str());
        MMDB_close(&mmdb);
        return "";
    }
    location.append(entry_data.utf8_string, entry_data.utf8_string + entry_data.data_size);
    MMDB_close(&mmdb);
    return location;
}
