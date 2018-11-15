#!/bin/bash

# export DYLD_LIBRARY_PATH=/Users/oeken/Desktop/celonis-challenge/cpp/libpqxx/build/src/dylib:/Users/oeken/torch/install/lib

pqxx_include1=../ext/libpqxx/include
pqxx_include2=../ext/libpqxx/build/include
pqxx_library=../ext/libpqxx/build/src/dylib

kvostore_library=.
kvostore_source=../src
kvostore_include=../include

# object code
clang++ -std=c++14 -c -Wall -Werror -fpic -I$pqxx_include1 -I$pqxx_include2 -I$kvostore_include $kvostore_source/KVOStore.cpp;

# compile library
clang++ -std=c++14 -Wall -Werror -shared -L$pqxx_library -lpqxx KVOStore.o -o libKVOStore.so ;

# compile test
clang++ -std=c++14 -Wall -Werror -I$kvostore_include -L$kvostore_library -lKVOStore $kvostore_source/test.cpp -o test ;
