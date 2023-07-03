#!/bin/bash




if [ "$#" -ne 1 ]; then
    dirs=`ls . | grep ^tests | cut -d"." -f1`
else
    dirs=$1
fi

for dir in $dirs; do
  success=1
  for file in `ls $dir/ | grep in | cut -d"." -f1`; do
    #echo Running test for $file
  #	echo "$file"
    ./hw5 < $dir/${file}.in > $dir/${file}.lli
    h=$(head $dir/${file}.lli)
    if [[ $h = line* ]] || [[ $h = Program* ]];
    then
      cp $dir/${file}.lli $dir/${file}.res
    else
      lli < $dir/${file}.lli > $dir/${file}.res
    fi
    diff $dir/$file.res $dir/${file}.out
      retval=$?
      if [ $retval -ne 0 ];
      then
          echo "Failed test $file!"
          success=0
      fi
    #echo \*\*\* DONE \*\*\*
  done
  if [ $success -eq 1 ];
  then
      echo "Passed all ${dir}!"
  else
      echo "Failed some of the ${dir}!"
  fi
done
