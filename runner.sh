#!/bin/bash
mkdir -p output
./run.sh 0.103 
mv "Policy.txt" "./output/1.txt"
./run.sh 0.235 
mv "Policy.txt" "./output/2.txt"
./run.sh 0.001 
mv "Policy.txt" "./output/3.txt"
./run.sh 0.023 
mv "Policy.txt" "./output/4.txt"
./run.sh 0.345 
mv "Policy.txt" "./output/5.txt"
