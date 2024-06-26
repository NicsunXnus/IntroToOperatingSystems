#!/bin/bash

####################
# Lab 1 Exercise 5
# Name: Nicholas Sun
# Student No: A0217609B
# Lab Group: B04
####################

# Fill the below up
hostname=$(hostname)
machine_hardware=$(uname -s -m)
max_user_process_count=$(cat /proc/sys/kernel/pid_max)
user_process_count=$(ps -e | wc -l)
user_with_most_processes=$(ps -eo user|sort|uniq -c|sort -n|tail -1|awk '{print $2}')
mem_free_percentage=$(free -m | head -n 2 | tail -n 1 | awk {'printf( "%.2f", ($4 / $2) * 100);'})

echo "Hostname: $hostname"
echo "Machine Hardware: $machine_hardware"
echo "Max User Processes: $max_user_process_count"
echo "User Processes: $user_process_count"
echo "User With Most Processes: $user_with_most_processes"
echo "Memory Free (%): $mem_free_percentage"