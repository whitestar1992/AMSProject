#!/bin/bash
#shopt -s -o nounset

ulimit -c 0
ulimit -d 700000
ulimit -s 32000

unset DEBUGFLAG 
export GDBMAN=1
export GDBINFO=1
export VERBOSE=1
export LANGUAGE=US
export LANG=C
export LC_ALL=en_US

###### SLC Version
OSRelease=`grep SLC /etc/redhat-release | cut -d' ' -f6`
OSVersion=${OSRelease%.*}

#### CERN CVMFS %% GCC Compiler
#GCCTAG=4.9.1
GCCTAG=6.2.0
source /cvmfs/sft.cern.ch/lcg/external/gcc/${GCCTAG}/x86_64-slc6/setup.sh

#### CERN AFS %% INTEL Compiler
#ICCTAG=17
#ICCDIR=/afs/cern.ch/sw/IntelSoftware/linux
#source ${ICCDIR}/${ICCTAG}-all-setup.sh intel64 &> /dev/null
#if [ $ICCTAG -gt 15 ]; then
#    source ${ICCDIR}/x86_64/xe20${ICCTAG}/compilers_and_libraries/linux/bin/compilervars.sh intel64
#else
#    source ${ICCDIR}/x86_64/xe20${ICCTAG}/composerxe/bin/compilervars.sh intel64 
#fi

#### CERN CVMFS %% INTEL Compiler
ICCTAG=17
ICCDIR=/cvmfs/projects.cern.ch/intelsw/psxe/linux
source ${ICCDIR}/${ICCTAG}-all-setup.sh intel64 &> /dev/null
source ${ICCDIR}/x86_64/20${ICCTAG}/compilers_and_libraries/linux/bin/compilervars.sh intel64 

#export INTEL_LICENSE_FILE=${Offline}/intel/licenses

#### AMS %% ROOT Environment
AMSSW=root-v5-34-9-icc64-slc6
export Offline=/cvmfs/ams.cern.ch/Offline
export ROOTSYS=${Offline}/root/Linux/${AMSSW}
export PATH=${ROOTSYS}/bin:${PATH}
export LD_LIBRARY_PATH=${ROOTSYS}/lib:${LD_LIBRARY_PATH}

#### AMS %% Xrd Environment
AMSXRD=${Offline}/AMSsoft/xrootd/xrootd-icc64-12
export LD_LIBRARY_PATH=${AMSXRD}/lib64:${LD_LIBRARY_PATH}

#### AMS %% Software Environment
AMSVersion=vdev
ROOTARCH=linuxx8664icc5.34
export AMSSRC=${Offline}/${AMSVersion}
export AMSLIB=${AMSSRC}/lib/${ROOTARCH}

export AMSWD=${AMSSRC}
export AMSDataDir=${Offline}/AMSDataDir
export AMSDataDirRW=${AMSDataDir}

alias hadd="${ROOTSYS}/bin/hadd -k"

#### COMPILER
export COMPILER=ICC

echo -e "setting environment for GCC-${GCCTAG} ICC-${ICCTAG} AMSSW-${AMSVersion}-${ROOTARCH}"