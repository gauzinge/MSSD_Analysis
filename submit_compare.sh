#!/bin/sh

#  submit_compare.sh
#  
#
#  Created by Georg Auzinger on 19.12.12.
#
source /afs/cern.ch/project/eos/installation/cms/etc/setup.sh
source /afs/cern.ch/sw/lcg/contrib/gcc/4.6/x86_64-slc5-gcc46-opt/setup.sh
source /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.05/x86_64-slc5-gcc46-dbg/root/bin/thisroot.sh
export PATH=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.05/x86_64-slc5-gcc46-dbg/root/lib/:$PATH
export LD_LIBRARY_PATH=/afs/cern.ch/sw/lcg/contrib/gcc/4.6/x86_64-slc5-gcc46-opt/lib64:${LD_LIBRARY_PATH}


# export PATH=$(pwd):$PATH
DIRECTORY='/afs/cern.ch/work/g/gauzinge/public/Analysis'
cd $DIRECTORY


for i in $*;
do
echo $DIRECTORY'/compare '$i ;
$DIRECTORY/compare $i;
done
