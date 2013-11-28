#! /bin/bash

for i in $*
do
	awk '{print $4}' $i > $i.cfg
done
