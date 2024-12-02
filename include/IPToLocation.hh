#pragma once

#include <cstddef>
#include <string>

size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream);

void maintain_mmdb_file(std::string mmdb_file);

std::string get_location_from_ip(std::string ip);
