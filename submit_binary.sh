#!/bin/bash

source /afs/cern.ch/project/eos/installation/cms/etc/setup.sh
source /afs/cern.ch/sw/lcg/contrib/gcc/4.6/x86_64-slc5-gcc46-opt/setup.sh
source /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.05/x86_64-slc5-gcc46-dbg/root/bin/thisroot.sh
export PATH=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.05/x86_64-slc5-gcc46-dbg/root/lib/:$PATH
export LD_LIBRARY_PATH=/afs/cern.ch/sw/lcg/contrib/gcc/4.6/x86_64-slc5-gcc46-opt/lib64:${LD_LIBRARY_PATH}


# export PATH=$(pwd):$PATH
DIRECTORY='/afs/cern.ch/work/g/gauzinge/public/Analysis'
cd $DIRECTORY

if [[ $# -ne 0 ]];
then
echo 'Scanning for Config Files';
counter=0;
for i in $*;
do
echo $i;
((counter++));
done
echo 'There are '$counter 'Config Files to be processed';
echo 'Submitting binary clustering!'
for i in $*;
do
echo $DIRECTORY'/binary '$i ;
$DIRECTORY/binary $i;
done
for i in $*;
do
echo $i;
done
echo 'Binary clustering done!';
else
echo 'ERROR: no Configfilenames provided!';
echo 'Syntax ' $0 ' Configfilename (including Wildchars!)';
echo 'Cleaning Matchfiles'
rm -rf $DIRECTORY/Matchfiles/*
fi
