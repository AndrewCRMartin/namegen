WHOINNFASTA=/acrm/www/html/abs/abybank/whoinnig/FASTA/whoinnig_latest.faa

grep '\>' $WHOINNFASTA | grep mab | sed 's/>//' | awk '{print $1}' | awk -F\| '{print $1}' | sort -u 

