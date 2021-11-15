rm results.txt;
for i in slurm*;
  do echo "--------------------" >> results.txt;
  echo $i >> results.txt;
  cat $i >> results.txt;
  done;
nano results.txt;