#!/bin/bash

find . -type f -name '*_conf.c' -exec rm {} +
find . -type f -name '*_types.c' -exec rm {} +
find . -type f -name '*.gch' -exec rm {} +
find . -type f -name '*.o' -exec rm {} +
find . -type f -name '*.su' -exec rm {} +
find . -type f -name '*.cyclo' -exec rm {} +
find . -type f -name '*.a' -exec rm {} +
find . -type f -name '*.tar.gz' -exec rm {} +
