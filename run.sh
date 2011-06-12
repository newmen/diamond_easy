#!/bin/sh

run_task=$1
result_dir=$2
prefix=$3

if [ "${run_task}" = "" ] || [ "${result_dir}" = "" ]; then
    echo "Usage: $0 full_run_taks_path full_result_dir_path prefix"
    exit 1
fi

if [ ! -f ${run_task} ]; then
    echo "Calculation program '${run_task}' does not exist!"
    exit 1
fi

if [ "${prefix}" = "" ]; then
    echo "Prefix cannot be blank!"
    exit 1
fi

if [ ! -d ${result_dir} ]; then
    mkdir -p ${result_dir}
fi

curr_time=`date +%s`
rand=$RANDOM

tmp_dirpath="/tmp/${prefix}-${curr_time}-${rand}"

mkdir ${tmp_dirpath}
cd ${tmp_dirpath}
${run_task}
mv * ${result_dir}
rm -rf ${tmp_dirpath}
