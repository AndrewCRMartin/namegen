sed ':a;N;818ba;s/\n//g' /tmp/abnames.dat | sed 's/\(.\)/\1\n/g' | sort -u > letters_used.txt
