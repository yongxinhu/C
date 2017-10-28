#!/bin/bash

# Just run this file and you can test your circ files!
# Make sure your files are in the directory above this one though!
# Credits to William Huang

cp alu.circ tests-3-1
cp regfile.circ tests-3-1
cd tests-3-1
python2.7 sanity_test.py
cd ..
