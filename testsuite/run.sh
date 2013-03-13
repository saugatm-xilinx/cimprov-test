#!/bin/bash

pushd `dirname \`which $0\`` >/dev/null
RUNDIR="$(pwd -P)"
popd >/dev/null

usage()
{
cat <<EOF
USAGE: run.sh [options]
Options:
  --cfg=<CFG>           Configuration to be used
  --list-cfg            Print the list of known configurations

EOF
exit 1
}

cfg_list()
{
    tab_len="16"
    for filename in ${RUNDIR}/tester_vars_*.py; do
        bname="`basename ${filename}`"
        bname="${bname#tester_vars_}"
        bname="${bname%.py}"
        echo -n "${bname}"
        for ((i=${#bname}; i <= tab_len; i++)); do echo -n " "; done
        cat ${filename} | grep  "TESTER_DESCRIPTION" | sed s/TESTER_DESCRIPTION=\"// | sed s/\"//
    done;
    exit 1
}

while test -n "$1" ; do
    case $1 in
        --help) usage ;;
        --cfg=*) TESTER_CFG=${1#--cfg=} ;;
        --list-cfg) cfg_list ;;
        *)  RUN_OPTS="${RUN_OPTS} $1" ;;
    esac
    shift 1
done

export CIMPROV_CLASSES=${CIMPROV_CLASSES:-$RUNDIR/../classes}
export PYTHONPATH=${PYTHONPATH-$RUNDIR/pywbem}
export TESTER_CFG=${TESTER_CFG:-default}

python ${RUNDIR}/tester.py ${RUN_OPTS}
