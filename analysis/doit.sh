date=20180620
paperdir=../paper
paperfigdir=$paperdir/figures
cn=abcheckname
nt=./namesovertime.pl
dy=./diversityOverTime.pl
ctoa=./byyear_csv_to_amplot.pl

if [ "X$1" == "X-h" ]; then
    echo ""
    echo "Usage: doit.sh date"
    echo "       date in the format yyyymmdd [Default $date]"
    echo ""
    echo "Performs all analysis and generates graphs which are copied to the 'paper' directory"
    echo ""
    exit 0
fi

if [ "X" != "X$1" ]; then
    date=$1
fi

ab=../abnamedata/abnames_$date.dat

echo -n "Analyzing phonetics ... "
$cn -a -p $ab | awk '{print $4}' >phonetics_$date.dat
echo "done"
echo -n "Analyzing Bouma ... "
$cn -a -b $ab | awk '{print $4}' >bouma_$date.dat
echo "done"
echo -n "Analyzing Letter confusion ... "
$cn -a -s $ab | awk '{print $4}' >letter_$date.dat
echo "done"

echo ""
echo "Generating distributions:"
bars -m 0.000000001 -w 5 phonetics_$date.dat > phonetics_$date.bars
bars -m 0.000000001 -w 5 bouma_$date.dat > bouma_$date.bars
bars -m 0.000000001 -w 5 letter_$date.dat > letter_$date.bars
echo "done"
echo ""

echo -n "Plotting distributions ... "
./plotdistribution.pl $date > distribution_$date.amplot
amplot distribution_$date.amplot > distribution_$date.eps
echo "done"

echo -n "Calculating statistics ... "
echo "Phonetics" > stats.txt
calcsd phonetics_$date.dat >> stats.txt
echo "Bouma" >> stats.txt
calcsd bouma_$date.dat >> stats.txt
echo "Letter confusion" >> stats.txt
calcsd letter_$date.dat >> stats.txt
echo "done (see stats.txt)"

# Create the by-year file of names
echo -n "Creating names by year data (for each year) ... "
$nt -y data/names_$date.csv > names_byyear_$date.dat
echo "done"

#echo -n "Creating names by year data (cummulative) ... "
#$nt -y -c data/names_$date.csv > names_byyear_cummulative_$date.dat
#echo "done"

# Create the distribution of names used per year
echo -n "Generating graphs of names used per year ... "
./plotnamesperyear.pl -date=$date -xmaxc=70 -xmaxcc=600 data/names_$date.csv 
echo "done"

# Create the distribution of names approved per year
echo -n "Generating graphs of names approved per year ... "
./plotnamesperyear.pl -out=ApprovedNamesPerYear -date=$date -xmaxc=20 -xmaxcc=100 data/ApprovedNames_$date.csv 
echo "done"

# Analyze for SD, mean and sum
echo -n "Calculating diversity over time (for each year), SD ... "
$dy -s    names_byyear_$date.dat > sd_byyear_$date.csv
echo "done"
echo -n "Calculating diversity over time (for each year), Mean ... "
$dy -m    names_byyear_$date.dat > mean_byyear_$date.csv
echo "done"
echo -n "Calculating diversity over time (for each year), Threshold ... "
$dy -s -m names_byyear_$date.dat > meanplus3sd_byyear_$date.csv
echo "done"

# Generate graphs
echo -n "Generating graphs (for each year) ... "
$ctoa -xlab=Year -ylab="Standard Deviation" -xkey=0.7 -ykey=1.0 -bounds="1990 2020 5 13" sd_byyear_$date.csv > sd_byyear_$date.amplot
amplot sd_byyear_$date.amplot > sd_byyear_$date.eps

$ctoa -xlab=Year -ylab="Mean" -xkey=0.7 -ykey=1.0 -bouds="1990 2020 50 90" mean_byyear_$date.csv > mean_byyear_$date.amplot
amplot mean_byyear_$date.amplot > mean_byyear_$date.eps

$ctoa -xlab=Year -ylab="Threshold" -xkey=0.7 -ykey=1.0 -bounds="1990 2020 75 110" meanplus3sd_byyear_$date.csv > meanplus3sd_byyear_$date.amplot
amplot meanplus3sd_byyear_$date.amplot > meanplus3sd_byyear_$date.eps
echo "done"




# Analyze for SD, mean and sum
echo -n "Calculating diversity over time (cummulative), SD ... "
$dy -c -s    names_byyear_$date.dat > sd_byyear_cummulative_$date.csv
echo "done"
echo -n "Calculating diversity over time (cummulative), Mean ... "
$dy -c -m    names_byyear_$date.dat > mean_byyear_cummulative_$date.csv
echo "done"
echo -n "Calculating diversity over time (cummulative), Threshold ... "
$dy -c -s -m names_byyear_$date.dat > meanplus3sd_byyear_cummulative_$date.csv
echo "done"

# Generate graphs
echo -n "Generating graphs (cummulative) ... "
$ctoa -xlab=Year -ylab="Standard Deviation" -xkey=0.7 -ykey=1.0 -bounds="1990 2020 5 13" sd_byyear_cummulative_$date.csv > sd_byyear_cummulative_$date.amplot
amplot sd_byyear_cummulative_$date.amplot > sd_byyear_cummulative_$date.eps

$ctoa -xlab=Year -ylab="Mean" -xkey=0.7 -ykey=1.0 -bouds="1990 2020 50 90" mean_byyear_cummulative_$date.csv > mean_byyear_cummulative_$date.amplot
amplot mean_byyear_cummulative_$date.amplot > mean_byyear_cummulative_$date.eps

$ctoa -xlab=Year -ylab="Threshold" -xkey=0.7 -ykey=1.1 -bounds="1990 2020 75 110" meanplus3sd_byyear_cummulative_$date.csv > meanplus3sd_byyear_cummulative_$date.amplot
amplot meanplus3sd_byyear_cummulative_$date.amplot > meanplus3sd_byyear_cummulative_$date.eps
echo "done"













# Copy graphs to the paper directory
cp distribution_$date.eps $paperfigdir
(cd $paperfigdir; ln -sf distribution_$date.eps distribution.eps)

cp namesperyear_$date.eps $paperfigdir
(cd $paperfigdir; ln -sf namesperyear_$date.eps namesperyear.eps)

cp namesperyear_cummulative_$date.eps $paperfigdir
(cd $paperfigdir; ln -sf namesperyear_cummulative_$date.eps namesperyear_cummulative.eps)

cp ApprovedNamesPerYear_$date.eps $paperfigdir
(cd $paperfigdir; ln -sf ApprovedNamesPerYear_$date.eps ApprovedNamesPerYear.eps)

cp ApprovedNamesPerYear_cummulative_$date.eps $paperfigdir
(cd $paperfigdir; ln -sf ApprovedNamesPerYear_cummulative_$date.eps ApprovedNamesPerYear_cummulative.eps)

cp sd_byyear_$date.eps $paperfigdir
(cd $paperfigdir; ln -sf sd_byyear_$date.eps sd_byyear.eps)

cp mean_byyear_$date.eps $paperfigdir
(cd $paperfigdir; ln -sf mean_byyear_$date.eps mean_byyear.eps)

cp meanplus3sd_byyear_$date.eps $paperfigdir
(cd $paperfigdir; ln -sf meanplus3sd_byyear_$date.eps meanplus3sd_byyear.eps)

cp sd_byyear_cummulative_$date.eps $paperfigdir
(cd $paperfigdir; ln -sf sd_byyear_cummulative_$date.eps sd_byyear_cummulative.eps)

cp mean_byyear_cummulative_$date.eps $paperfigdir
(cd $paperfigdir; ln -sf mean_byyear_cummulative_$date.eps mean_byyear_cummulative.eps)

cp meanplus3sd_byyear_cummulative_$date.eps $paperfigdir
(cd $paperfigdir; ln -sf meanplus3sd_byyear_cummulative_$date.eps meanplus3sd_byyear_cummulative.eps)

# Copy data file to Supplementary Material
mkdir -p $paperdir/SUPPLEMENTARY
cp data/names_$date.csv $paperdir/SUPPLEMENTARY/names.csv
