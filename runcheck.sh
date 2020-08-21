#!/bin/bash
name=$1

abnames=abnamedata/abnames_covid.dat

if [ ! -f $abnames ]; then
    echo "You must install the antibody names file file ($abnames)"
else
    echo ""
    echo "Phonetics (85.5)"
    echo "================"
    ./abcheckname -n $abnames -t 81 -p $name | sort -n -k4 | tail -5
    echo ""
    echo "Writing (80.7)"
    echo "=============="
    ./abcheckname -n $abnames -t 76 -s $name | sort -n -k4 | tail -5
    echo ""
    echo "Longest end match"
    echo "================="
    ./abcheckname -n $abnames -e $name
fi
