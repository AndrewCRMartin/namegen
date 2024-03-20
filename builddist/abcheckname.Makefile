EXE    = abcheckname
OFILES = abcheckname.o
BIOP   = ./bioplib
LFILES = $(BIOP)/align.o $(BIOP)/array2.o $(BIOP)/GetWord.o \
         $(BIOP)/OpenFile.o
CC     = cc -I$(INCDIR) -O3

all : $(EXE)


$(EXE) : $(OFILES) $(LFILES)
	$(CC) -o $@ $(OFILES) $(LFILES) -lm

.c.o :
	$(CC) -c -o $@ $<

clean :
	\rm -f *.o $(BIOP)/*.o

distclean : clean
	\rm -f $(EXE)
