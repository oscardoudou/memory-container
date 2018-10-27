#!/bin/bash

# Parse input
if [ $# -ne 4 ]; then
    echo "Usage: $0 <# of objects> <max size of objects> <# of tasks> <# of containers>"
    exit
fi

number_of_objects=$1 
max_size_of_objects=$2 
number_of_processes=$3
number_of_containers=$4

sudo insmod kernel_module/memory_container.ko
sudo chmod 777 /dev/mcontainer
./benchmark/benchmark $1 $2 $3 $4
cat *.log > trace
sort -n -k 4 trace > sorted_trace
./benchmark/validate $1 $2 $4 < sorted_trace

# if you want to see the log for debugging, comment out the following line.
rm -f *.log trace sorted_trace

sudo rmmod memory_container