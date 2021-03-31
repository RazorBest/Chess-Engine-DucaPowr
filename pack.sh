#!/bin/bash

rm Etapa1.zip
cd src
make clean
zip ../Etapa1.zip *


if [ $(du --block-size=1M ../Etapa1.zip | awk '{print $1}') -ge 5 ]
then
    echo "ERROR: zip file might be bigger than 5MB"
    exit 1
fi
