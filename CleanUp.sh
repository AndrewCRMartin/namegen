date=20180620

cd analysis
\rm -f *_$date.csv
\rm -f *_$date.eps
\rm -f *_$date.amplot
\rm -f *_$date.bars
\rm -f stats.txt
cd ..

cd paper
\rm -f *.blg *.bbl *.aux *.dvi *.log
cd ..

make clean

