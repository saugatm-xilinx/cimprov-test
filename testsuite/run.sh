#!/bin/bash

pushd `dirname \`which $0\`` >/dev/null
RUNDIR="$(pwd -P)"
popd >/dev/null

export PYTHONPATH=${PYTHONPATH-$RUNDIR/pywbem}

python ${RUNDIR}/tester.py "$@"
