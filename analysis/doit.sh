cn=checkname
date=20170116
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
