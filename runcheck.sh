#!/bin/bash
name=$1

abnames=/tmp/abnames.dat

if [ ! -f /tmp/abnames.dat ]; then
    echo "You must install the abnames.dat file in /tmp ($abnames)"
else
    echo ""
    echo "Phonetics (85.5)"
    echo "================"
    ./abcheckname -n $abnames -t 81 -p $name | sort -n -k4 | tail -5
    echo ""
    echo "Writing (80.7)"
    echo "=============="
    ./abcheckname -n $abnames -t 76 -s $name | sort -n -k4 | tail -5
fi
