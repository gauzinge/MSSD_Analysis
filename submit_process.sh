#!/bin/bash
source /afs/cern.ch/project/eos/installation/cms/etc/setup.sh
source /afs/cern.ch/sw/lcg/contrib/gcc/4.6/x86_64-slc5-gcc46-opt/setup.sh
source /afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.05/x86_64-slc5-gcc46-dbg/root/bin/thisroot.sh
export PATH=/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.05/x86_64-slc5-gcc46-dbg/root/lib/:$PATH
export PATH=/afs/cern.ch/sw/lcg/external/xrootd/3.2.7/x86_64-slc5-gcc46-opt/bin:$PATH
export LD_LIBRARY_PATH=/afs/cern.ch/sw/lcg/contrib/gcc/4.6/x86_64-slc5-gcc46-opt/lib64:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=/afs/cern.ch/sw/lcg/external/xrootd/3.2.7/x86_64-slc5-gcc46-opt/lib64:${LD_LIBRARY_PATH}

# mounteos
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
echo 'Submitting analysis!'
index=0
for i in $*;
do
echo $DIRECTORY'/process '$i;
$DIRECTORY/process $i;
done
echo 'Finished processing!'
else
echo 'ERROR: no Configfilenames provided!';
echo 'Syntax ' $0 ' Configfilename (including Wildchars!)'; #Significance Cut(s)';
# echo 'Cleaning Matchfiles'
# rm -rf $DIRECTORY/Matchfiles/*
fi
 
