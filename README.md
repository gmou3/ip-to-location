# _ip-to-location_

A tool that transforms an `IP` to a string of the form `City, Country`.

It works by scraping a free `mmdb` database from <https://db-ip.com/> and performing `IP` lookups using `libmaxminddb`.

The database file is automatically kept up-to-date with the upstream monthly updates.

The compressed database file is decompressed using `libdeflate`.

## Usage

To build, run:
```bash
cmake -B build
cd build/
cmake --build .
```

Then, run the compiled executable with the `IP` as an argument:
```bash
./ip-to-location <IP address>
```

## Third party libraries used

`libdeflate` (<https://github.com/ebiggers/libdeflate.git>)

`libmaxminddb` (<https://github.com/maxmind/libmaxminddb.git>)
