#pragma once

#include <string>

void decompress_gz_file(std::string filename);

void maintain_mmdb_file(std::string const& mmdb_file);

std::string get_location_from_ip(std::string const& ip);
