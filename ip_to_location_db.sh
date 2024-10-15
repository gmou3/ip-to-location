#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

year=$(date +%Y)
month=$(date +%m)
url="https://download.db-ip.com/free/dbip-city-lite-${year}-${month}.mmdb.gz"
filename="${SCRIPT_DIR}/ip_to_location.mmdb.gz"
download=true

if [ -f "${filename%.*}" ]; then
  db_year=$(date -r "${filename%.*}" -u +%Y)
  db_month=$(date -r "${filename%.*}" -u +%m)
  if (( db_year == year )) && (( db_month == month )); then
    download=false
  fi
fi

if $download; then
  wget $url -O $filename
  gzip -df $filename
fi
