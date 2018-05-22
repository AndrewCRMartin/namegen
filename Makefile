EXE    = checkname
OFILES = checkname.o
LIBDIR = $(HOME)/lib
INCDIR = $(HOME)/include
CC     = cc -L$(LIBDIR) -I$(INCDIR)

$(EXE) : $(OFILES)
	$(CC) -o $@ $(OFILES) -lbiop -lgen -lm

.c.o :
	$(CC) -c -o $@ $<

clean :
	\rm -f $(OFILES)

distclean : clean
	\rm -f $(EXE)
