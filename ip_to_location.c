#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <maxminddb.h>

int main(int argc, char *argv[]) {
	char pBuf[256];  // current exe path
	size_t len = sizeof(pBuf);
	int bytes = readlink("/proc/self/exe", pBuf, len);
	pBuf[bytes] = '\0';

	char database_script[256];  // database maintainance script
	strcpy(database_script, pBuf);
	strcat(database_script, "_db.sh");
	system(database_script);  // run

	char mmdb_file[256];  // database file
	strcpy(mmdb_file, pBuf);
	strcat(mmdb_file, ".mmdb");

	MMDB_s mmdb;
	MMDB_open(mmdb_file, MMDB_MODE_MMAP, &mmdb);
	int gai_error, mmdb_error;
	MMDB_lookup_result_s result = MMDB_lookup_string(&mmdb, argv[1], &gai_error, &mmdb_error);
	MMDB_entry_data_s entry_data;
	MMDB_get_value(&result.entry, &entry_data, "city", "names", "en", NULL);
	char location[64];
	int city_len = entry_data.data_size;
	location[city_len] = 0;
	strncpy(location, entry_data.utf8_string, entry_data.data_size);
	MMDB_get_value(&result.entry, &entry_data, "country", "names", "en", NULL);
	strcat(location, ", ");
	strncat(location, entry_data.utf8_string, entry_data.data_size);
	location[city_len + 2 + entry_data.data_size] = 0;
	printf("%s", location);
}
