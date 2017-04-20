#!/bin/bash                                                                                                                            
# runParallelMPI.sh                                                                                                                   
# Muriel Brunet                                                                                                         
# CSC 352, Spring 17                                                                                                                   
# Runs for 3000 generations
# with a 10,000 array dish
# over 2, 4, 6, 8, 16 processes                                                                                                            
# chmod a+x runParallelMPI.sh                                                                                                         
# ./runParallelMPI.sh                                                                                                                 
# ./runParallelMPI.sh 2>&1 | grep "with\|real" > times.data                                                                     

javac *.java # compile all the files                                                                                                   
for i in 2 4 8 12 16 20 32 64 ; do # number of threads                                                                                 
    printf "\n"
    printf "\n"
    echo "..................................... with" $i "threads"
    for j in 1 2 3 4 5 6 7 8 9 10 ; do # number of times to run the program                                                            
        echo "RUN" $j
        time java GameOfLifeApplication "dish.txt" $i 10 false
    done
done


