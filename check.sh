#!/bin/bash

# pip install cpplint
cpplint --filter=\
-build/include,\
+build/include_subdir,\
+build/include_order,\
+build/include_what_you_use,\
-runtime/references --recursive * > checkstyle.txt 2>&1
