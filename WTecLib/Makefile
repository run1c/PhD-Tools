######
#
#	IMPORTANT
#	=========
#	The visa library is located inside the framework at
#	/Library/Frameworks/VISA.framework/Versions/Current
#	and is an binary called "VISA".
#
######

LIBNAME=libwt.a
OBJ=wt_visaResourceManager.o wt_visaDevice.o RigolDS1000Z.o RigolDG5000.o
CC=g++
CFLAGS=-Wall -g
LDFLAGS=

### Rohde & Schwarz VISA
#VISA_LIB=/Library/Frameworks/RsVisa.framework/Versions/A/RsVisa
#VISA_INC=/Library/Frameworks/RsVisa.framework/Versions/A/Headers
#LDFLAGS+=-L$(VISA_LIB) -lrsvisa
###

### National Instruments VISA
VISA_LIB=/Library/Frameworks/VISA.framework/Versions/Current
VISA_INC=/Library/Frameworks/VISA.framework/Versions/Current/Headers
LDFLAGS+=$(VISA_LIB)/VISA
###

# ROOT flags
ROOTCFLAGS:=$(shell root-config --cflags)
ROOTLIBS:=$(shell root-config --libs)
ROOTGLIBS:=$(shell root-config --glibs)

CFLAGS+=$(ROOTCFLAGS)
LDFLAGS+=-Wl,-rpath,/Applications/root_v6.16.00/lib $(ROOTGLIBS)

%.o: %.cpp
	$(CC) -c $(CFLAGS) -I$(VISA_INC) $^

$(LIBNAME): $(OBJ)
	ar ru $@ $^
	ranlib $@

clean:
	rm -rf *.o
	rm -rf $(LIBNAME)
