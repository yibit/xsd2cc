# Copyright (c) 2014 The xsd2cc Authors. All rights reserved.
# Use of this source code is governed by GPL license that can be
# found in the LICENSE file. See the AUTHORS file for names of contributors.
###############################################################################
#!/bin/sh

usage()
{
    echo "                                                 "
    echo " Usage: $0 <directory> [prog-name]               "
    echo "                                                 "

    return 0    
}

_make_prog()
{
    p_config=$1
    cat $p_config |awk -F'","' ' { print $2; } ' > .prog_config1
    MY_ARGS=`cat .prog_config1 |awk -F'"=' '{ print $1"="$2; }' |tr '\n' ' '`
    echo "make $MY_ARGS" |sh
    #make $MY_ARGS
}

make_run()
{
    if test -z $PROGS; then
        PROGS=`cat config.json |grep -E '[\t ]*".*":[\t ]*\{' |tr -d '"' |awk -F":" '{ print $1; }'`
    fi
    sh $JSON -b < config.json |tr -d '[\t' |tr ']' '=' > .progs_config

    for p in $PROGS
    do
        cat .progs_config |grep "\"$p\"" > .prog_config0
        _make_prog ".prog_config0"
    done
}

run()
{
    for f in $PROGS
    do
        echo "--------- run ./$f ---------"
        ./$f
    done
}

make_clean()
{
    rm -f .progs_config .prog_config0 .prog_config1
}

#=================== main ======================
WORKDIR=`pwd`
PROGS=""
JSON=../tools/json.sh

if test $# -lt 1; then
    usage
    exit 1
fi

TARGETDIR=$1

if test $# -gt 1; then
    PROGS=$2
fi

cd $TARGETDIR

make_run

run

make_clean

cd $WORKDIR

