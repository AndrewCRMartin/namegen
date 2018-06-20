cn=checkname
nt=./namesovertime.pl
dy=./diversityOverTime.pl
date=20180620
ab=../abnamedata/abnames_$date.dat
ctoa=./byyear_csv_to_amplot.pl
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
$nt -y data/names_$date.csv > names_byyear_$date.dat

# Analyze for SD, mean and sum
$dy -s    names_byyear_$date.dat > sd_byyear_$date.csv
$dy -m    names_byyear_$date.dat > mean_byyear_$date.csv
$dy -s -m names_byyear_$date.dat > meanplus3sd_byyear_$date.csv

# Generate graphs
$ctoa -xlab=Year -ylab="Standard Deviation" sd_byyear_$date.csv > sd_byyear_$date.amplot
amplot sd_byyear_$date.amplot > sd_byyear_$date.eps

$ctoa -xlab=Year -ylab="Mean" mean_byyear_$date.csv > mean_byyear_$date.amplot
amplot mean_byyear_$date.amplot > mean_byyear_$date.eps

$ctoa -xlab=Year -ylab="Threshold" meanplus3sd_byyear_$date.csv > meanplus3sd_byyear_$date.amplot
amplot meanplus3sd_byyear_$date.amplot > meanplus3sd_byyear_$date.eps
