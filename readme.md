Compile:
mpicxx convexhull.cpp

Run
mpirun --hostfile host -np 16 --oversubscribe a.out data/1000000/2.txt