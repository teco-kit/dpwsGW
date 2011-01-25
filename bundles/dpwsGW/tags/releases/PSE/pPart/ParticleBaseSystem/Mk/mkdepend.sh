#!/bin/sh
which gpvc >/dev/null 2>&1 

if [  $? = 0 ];then
echo ${1%.cod}.hex: \\
gpvc -d $1|awk ' 
/^ *$/{next}
start{gsub("\\\\","/");gsub("^(.?):","/cygdrive/"substr($0,1,1));printf("%s ",$0)}
/^--/{start=1}
END{print ""}
'
else
echo ${1%.cod}.hex: FORCE
fi
