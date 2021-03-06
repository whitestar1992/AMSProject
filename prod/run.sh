#!/bin/bash
Version=18Mar12

ClassDef=${AMSCore}/prod/${Version}/lib
LD_LIBRARY_PATH=${ClassDef}:${LD_LIBRARY_PATH}
RunFile=${AMSCore}/prod/${Version}/YiProdNtuple

#DataType=ISS
#Stream=lst/flist.asgc.iss.B950.pass6
#Stream=lst/flist.ncu.iss.B950.pass6
#Stream=lst/flist.cern.iss.B950.pass6
DataType=MC
Stream=lst/flist.ncu.mc.pr.pl1.l1.054000.B1119
#Stream=lst/flist.cern.mc.pr.pl1.ecal.10004000.B1103
#Stream=lst/flist.cern.mc.pr.pl1.1800.B1042
#Stream=lst/flist.cern.mc.pr.pl1.l1.054000.B1119
#Stream=lst/flist.cern.mc.el.pl1.0_25200.B1119
#Stream=lst/flist.cern.mc.el.pl1.2004000.B1119

GroupId=2
GroupSize=10
OutputDir=.

if [[ -f ${RunFile} && -f ${Stream} ]]; then
  ${RunFile} ${DataType} ${Stream} ${GroupId} ${GroupSize} ${OutputDir}
else
  echo -e "Please Check File :"
  if [ ! -f ${RunFile} ]; then echo ${RunFile}; fi
  if [ ! -f ${Stream} ]; then echo ${Stream}; fi
fi
