cn=checkname
nt=./namesovertime.pl
dy=./diversityOverTime.pl
date=20180614
ab=../abnamedata/abnames_$date.dat
$cn -a -p $ab | awk '{print $4}' >phonetics_$date.dat
$cn -a -b $ab | awk '{print $4}' >bouma_$date.dat
$cn -a -s $ab | awk '{print $4}' >letter_$date.dat
bars -m 0.000000001 -w 5 phonetics_$date.dat > phonetics_$date.bars
bars -m 0.000000001 -w 5 bouma_$date.dat > bouma_$date.bars
bars -m 0.000000001 -w 5 letter_$date.dat > letter_$date.bars

echo "Phonetics" > stats.txt
calcsd phonetics_$date.dat >> stats.txt
echo "Bouma" >> stats.txt
calcsd bouma_$date.dat >> stats.txt
echo "Letter confusion" >> stats.txt
calcsd letter_$date.dat >> stats.txt

# Create the by-year file of names
$nt -y data/innlist_$date.txt > names_byyear_$date.dat

# Analyze for SD, mean and sum
$dy -s    names_byyear_$date.dat > sd_byyear_$date.dat
$dy -m    names_byyear_$date.dat > mean_byyear_$date.dat
$dy -s -m names_byyear_$date.dat > meanplussd_byyear_$date.dat

