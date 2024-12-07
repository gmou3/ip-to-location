# _ip-to-location_

A tool that transforms an `IP` into a string of the form `City, Country`.

It works by scraping a free `mmdb` database from [db-ip](https://db-ip.com/) and performing `IP` lookups using `libmaxminddb`.

The database file is automatically kept up-to-date with the upstream monthly updates.

The compressed database file is decompressed using `libdeflate`.

## Build

Clone the repo with its submodules:
```bash
git clone https://github.com/gmou3/ip-to-location --recurse-submodules
cd ip-to-location/
```

Then, to build, run:
```bash
cmake -B build
cd build/
cmake --build .
```

## Usage

Run the compiled `ip-to-location` executable with an `IP` address as an argument. For example, to get the location of your own `IP`, run
```bash
./ip-to-location $(curl -s ifconfig.me)
```

## Third party libraries used

1. [libdeflate](https://github.com/ebiggers/libdeflate.git)
2. [libmaxminddb](https://github.com/maxmind/libmaxminddb.git)
