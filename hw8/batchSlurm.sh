for i in *.slurm;
    sbatch $i;
done;
squeue;
