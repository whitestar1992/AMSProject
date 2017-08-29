#IOPT = -axsse4.2,ssse3 # Only for Intel Core
IOPT =

DEFS = -D_PGTRACK_ -D__ROOTSHAREDLIBRARY__
PARA = -D_X8664__ -D_AMSPARALLEL__

ICCCXX  = icc $(IOPT) -std=c++11 $(PARA) -O2 -D_DEBUG -Wall
GCCCXX  = g++ $(IOPT) -std=c++11 $(PARA) -O2 -D_DEBUG -Wall

FLGS = -I$(ROOTSYS)/include -isystem$(ROOTSYS)/include -I$(AMSSRC)/include -isystem$(AMSSRC)/include -I$(AMSProjLibs) -isystem$(AMSProjLibs) -I$(AMSCoreProd) -isystem$(AMSCoreProd)

LD   = $(shell ${ROOTSYS}/bin/root-config --ld) $(IOPT) $(PARA)
LDS  = $(shell ${ROOTSYS}/bin/root-config --ld) $(IOPT) $(PARA) -static-intel

CERNDIR = /cvmfs/ams.cern.ch/Offline/CERN/2005
LIBD = $(AMSLIB)/libntuple_slc6_PG_dynamic.so
LIBA = $(AMSLIB)/libntuple_slc6_PG.a

LIBP  = $(shell $(ROOTSYS)/bin/root-config --libs)
LIBP += -L$(ROOTSYS)/lib -lRoot -lfreetype -pthread -lpcre
#LIBP += -lMinuit -lMinuit2 -lRFIO -lTMVA -lXMLIO -lMLP -lTreePlayer -lMathCore
#LIBP += -llzma -Llib -lcrypto
LIBP += -L$(CERNDIR)/lib -lmathlib
LIBP += $(LIBD)


#LIBP  = $(shell $(ROOTSYS)/bin/root-config --libs)
#LIBP += -L$(ROOTSYS)/lib -lRoot -lfreetype -pthread -lpcre
#LIBP += -lMinuit -lMinuit2 -lRFIO -lTMVA -lXMLIO -lMLP -lTreePlayer -lMathCore
#LIBP += -llzma -Llib -lcrypto
#LIBP += -L$(CERNDIR)/lib -lmathlib
#LIBP += $(LIBD)
