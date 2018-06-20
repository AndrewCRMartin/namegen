file=$1
abdatadir=../abnamedata
analdatadir=../analysis/data

if [ "X" == "X$1" ] || [ "$1" == "-h" ]; then
    echo ""
    echo "Usage: ./updatenames.sh \"INN Search.html\""
    echo ""
    echo "Extracts the antibody names from the INN Search HTML and creates a CSV file"
    echo "containing the:"
    echo "   INN number, name, PL number, RL number"
    echo "It then extracts the raw names and places them in the ../abnamedata directory"
    echo "The new files are checked into git and pushed to github"
    echo ""
else
    date=`date +%Y%m%d`
    csvfile=names_$date.csv
    namefile=abnames_$date.dat

    ./getnames.pl "$file" >$csvfile
    ./getnamelist.pl $csvfile > $abdatadir/$namefile
    cp $csvfile $analdatadir

    (cd $abdatadir; rm abnames.dat; ln -s $namefile abnames.dat)
    git add $csvfile $abdatadir/$namefile $analdatadir/$csvfile
    git commit -a -m "Added names files for $date"
    git push
fi


