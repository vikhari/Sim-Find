#!/bin/sh

# Use this file to compile.

echo "Compilation starting ~~~~~~~~~~~~~~"

# Used with SQLite3 (this is a remnant and can be ignored).
#g++ ./mysim/main.cpp -o ./mysim/runsf -lboost_system -lboost_filesystem

# Used with MongoDB.
c++ --std=c++11 ./simfind/sf_query.cpp -o ./simfind/run_sf_query $(pkg-config --cflags --libs libmongocxx) -lboost_system -lboost_filesystem -lcrypto

echo "Compilation ending   ~~~~~~~~~~~~~~"
