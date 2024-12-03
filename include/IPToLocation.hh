#pragma once

#include <cstddef>
#include <string>

void decompress_gz_file(std::string filename);

void maintain_mmdb_file(const std::string& mmdb_file);

std::string get_location_from_ip(const std::string& ip);
