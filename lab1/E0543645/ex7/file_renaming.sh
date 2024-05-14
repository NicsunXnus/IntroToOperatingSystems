#!/bin/bash

####################
# Lab 1 Exercise 7
# Name: Nicholas Sun Jun Yang
# Student No: A0217609B
# Lab Group: B04
####################

####################
# Strings that you may need for prompt (not in order)
####################
# Enter $N numbers:
# Enter NEW prefix (only alphabets):
# Enter prefix (only alphabets):
# INVALID
# Number of files to create:
# Please enter a valid number [0-9]:
# Please enter a valid prefix [a-z A-Z]:

####################
# Strings that you may need for prompt (not in order)
####################

#################### Steps #################### 

# Fill in the code to request user for the prefix
echo "Enter prefix (only alphabets):"
read prefix
# Check the validity of the prefix #
while [[ "$prefix" =~ [^a-zA-Z] ]]
do
    echo "INVALID"
    echo "Please enter a valid prefix [a-z A-Z]"
    read prefix
done

# Enter numbers and create files #
echo "Number of files to create:"
read N

numArr=()

echo "Enter $N numbers:"
for(( c=1; c<=$N; c++))
do
    read number
    while [[ "$number" =~ [^0-9] ]]
    do
        echo "INVALID"
        echo "Please enter a valid number [0-9]:"
        read number
    done
    numArr+=( $number )
done

echo ""
echo "Files Created"

filesArr=()

for n in "${numArr[@]}" 
do
    filename=$prefix"_"$n".txt"
    filesArr+=( $filename )
    touch $filename
done

ls *.txt
echo ""
# Fill in the code to request user for the new prefix
echo "Enter NEW prefix (only alphabets):"
read new_prefix

# Check the validity of the new prefix #
while [[ "$new_prefix" =~ [^a-zA-Z] ]]
do
    echo "INVALID"
    echo "Please enter a valid prefix [a-z A-Z]"
    read new_prefix
done

echo ""
# Renaming to new prefix #
echo "Files Renamed"
for (( i=0 ; i<$N; i++))
do
    newfilename=$new_prefix"_"${numArr[i]}".txt"
    mv ${filesArr[i]} $newfilename
done

ls *.txt