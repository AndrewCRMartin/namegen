EXE    = abcheckname
OFILES = abcheckname.o
LIBDIR = $(HOME)/lib
INCDIR = $(HOME)/include
CC     = cc -L$(LIBDIR) -I$(INCDIR) -O3

all : $(EXE)
	(cd utils/MakeKondrakMatrix; make)

$(EXE) : $(OFILES)
	$(CC) -o $@ $(OFILES) -lbiop -lgen -lm

.c.o :
	$(CC) -c -o $@ $<

clean :
	\rm -f $(OFILES)
	(cd utils/MakeKondrakMatrix; make clean)

distclean : clean
	\rm -f $(EXE)
	(cd utils/MakeKondrakMatrix; make distclean)

install : all
	./install.sh
