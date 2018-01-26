#!/bin/sh

# Use this file to compile.

echo "Compilation starting ~~~~~~~~~~~~~~"
# Used with SQLite3.
#g++ ./mysim/main.cpp -o ./mysim/runsf -lboost_system -lboost_filesystem

# Used with MongoDB.
c++ --std=c++11 ./simfind/sf_create.cpp -o ./simfind/run_sf_create $(pkg-config --cflags --libs libmongocxx) -lboost_system -lboost_filesystem -lcrypto

echo "Compilation ending   ~~~~~~~~~~~~~~"
