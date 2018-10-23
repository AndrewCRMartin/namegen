#!/bin/bash
name=$1
echo ""
echo "Phonetics"
echo "========="
./abcheckname -t 80 -p $name | sort -n -k4 | tail -5
echo ""
echo "Writing"
echo "======="
./abcheckname -t 75 -s $name | sort -n -k4 | tail -5
