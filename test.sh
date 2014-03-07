#!/bin/bash          

# compile
cmake . && make

# test
for input in $(ls input/);
do
	echo $input    
	./textureOptimization input/$input
done