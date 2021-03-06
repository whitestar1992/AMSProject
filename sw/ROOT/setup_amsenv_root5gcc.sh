#!/bin/bash
#shopt -s -o nounset

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
GCCTAG=6.2.0
source /cvmfs/sft.cern.ch/lcg/external/gcc/${GCCTAG}/x86_64-slc6/setup.sh

#### CERN CVMFS %% INTEL Compiler
ICCTAG=17
ICCDIR=/cvmfs/projects.cern.ch/intelsw/psxe/linux
ICCLUX=${ICCDIR}/x86_64/20${ICCTAG}/compilers_and_libraries/linux
source ${ICCDIR}/${ICCTAG}-all-setup.sh intel64 &> /dev/null
source ${ICCLUX}/bin/compilervars.sh intel64 

#### AMS %% ROOT Environment
AMSSW=root-v5-34-9-gcc64-slc6
export Offline=/cvmfs/ams.cern.ch/Offline
export ROOTSYS=${Offline}/root/Linux/${AMSSW}
export PATH=${ROOTSYS}/bin:${PATH}
export LIBRARY_PATH=${ROOTSYS}/lib:${LIBRARY_PATH}
export LD_LIBRARY_PATH=${ROOTSYS}/lib:${LD_LIBRARY_PATH}
export CPLUS_INCLUDE_PATH=${ROOTSYS}/include:${CPLUS_INCLUDE_PATH}

#### AMS %% Xrd Environment
AMSXRD=${Offline}/AMSsoft/xrootd/xrootd-gcc64-44
export LIBRARY_PATH=${AMSXRD}/lib64:${LIBRARY_PATH}
export LD_LIBRARY_PATH=${AMSXRD}/lib64:${LD_LIBRARY_PATH}

#### AMS %% Software Environment
AMSVersion=vdev
ROOTARCH=linuxx8664gcc5.34
export AMSSRC=${Offline}/${AMSVersion}
export AMSLIB=${AMSSRC}/lib/${ROOTARCH}
export LIBRARY_PATH=${AMSLIB}:${LIBRARY_PATH}
export LD_LIBRARY_PATH=${AMSLIB}:${LD_LIBRARY_PATH}
export CPLUS_INCLUDE_PATH=${AMSSRC}/include:${CPLUS_INCLUDE_PATH}

export AMSWD=${AMSSRC}
export AMSDataDir=${Offline}/AMSDataDir
export AMSDataDirRW=${AMSDataDir}
export AMSGeoDir=${Offline}/${AMSVersion}/display

alias hadd="${ROOTSYS}/bin/hadd -k"
function hadd2 {
    if [ $# == 1 ]; then
        if [ -f ${1}.root ]; then /bin/rm ${1}.root; fi
        ${ROOTSYS}/bin/hadd -k ${1}.root ${1}*.root
    else
        echo -e "Error: No Merged Name."
    fi
}

#### AMS %% CERN Software Environment
export CERNLIB=${Offline}/AMSsoft/linux_slc6_icc64/2005/lib
export LIBRARY_PATH=${CERNLIB}:${LIBRARY_PATH}
export LD_LIBRARY_PATH=${CERNLIB}:${LD_LIBRARY_PATH}

#### COMPILER
export COMPILER=GCC

echo -e "setting environment for GCC-${GCCTAG} ICC-${ICCTAG} AMSSW-${AMSVersion}-${ROOTARCH}"
