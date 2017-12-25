#!/bin/bash
input=$1
output=$2
keyword=$3

if [[ ${input} == "" ]]; then exit; fi
if [[ ${output} == "" ]]; then exit; fi

dpmurl=root://${DPNS_HOST}/${output}
dpmhost=${DPM_HOME}/${output}
for file in `xrdfs ${EOS_AMS_HOST} ls ${input}`
do
    if [[ `echo ${file} | grep ${keyword}` == "" ]]; then continue; fi
    filename=${file##*/}
    filecheck=$(dpns-ls ${dpmhost}/${filename} 2>&1)
    if [[ "${filecheck}" == *"No such file or directory"* ]] || [[ "${filecheck}" == *"invalid path"* ]]; then
        srcfile=root://${EOS_AMS_HOST}/${file}
        echo -e "==== xrdcp ${srcfile} ${dpmurl}/${filename}"
        xrdcp ${srcfile} ${dpmurl}/${filename}
    else
        echo -e "==== ${filename} is exist."
    fi
done
