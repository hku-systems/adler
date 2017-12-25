#!/bin/bash
find . -name "*.a" -type f
find . -name "*.a" -type f -delete

find . -name "*.ez240" -type f
find . -name "*.ez240" -type f -delete

find . -name "symbols.h" -type f
find . -name "symbols.c" -type f
find . -name "symbols.h" -type f -delete
find . -name "symbols.c" -type f -delete

find . -name "*.ihex" -type f
find . -name "*.ihex" -type f -delete

find . -name "*.d" -type f
find . -name "*.d" -type f -delete

find . -name "*.o" -type f
find . -name "*.o" -type f -delete
