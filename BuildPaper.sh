make
make install

cd analysis
./doit.sh
cd ..

cd paper
latex checkname
bibtex checkname
latex checkname
latex checkname
dvipdf checkname
cd ..

