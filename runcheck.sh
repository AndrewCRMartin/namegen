#!/bin/bash
name=$1
echo ""
echo "Phonetics (85.5)"
echo "================"
./abcheckname -t 81 -p $name | sort -n -k4 | tail -5
echo ""
echo "Writing (80.7)"
echo "=============="
./abcheckname -t 76 -s $name | sort -n -k4 | tail -5
