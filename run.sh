#!/bin/sh

run_task=$1
result_dir=$2

if [ ! -f ${run_task} ]; then
    echo "Calculation program '${run_task}' does not exists!"
    exit 1;
fi

if [ ! -d ${result_dir} ]; then
    echo "Result directory '${result_dir}' does not exists!"
    exit 1;
fi

prefix='diamond'
curr_time=`date +%s`
rand=$RANDOM

tmp_dirpath="/tmp/${prefix}-${curr_time}-${rand}"

mkdir ${tmp_dirpath}
cd ${tmp_dirpath}
${run_task}
mv * ${result_dir}
rm -rf ${tmp_dirpath}
