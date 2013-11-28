#!/bin/bash
for i in $*;
do
       	cut -d ' ' -f 1 $i  > $i'.tmp';
done
