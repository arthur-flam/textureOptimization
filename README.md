Texture Optimization
====================

Implementation of ["Texture Optimization for Example-based Synthesis"](http://www.cc.gatech.edu/cpl/projects/textureoptimization/TO-final.pdf) by Vivek Kwatra at al.

## Try it

You can generate a single image:
```r
cmake . && make && ./DisplayImage input/ztext.bmp
```

Or all of them
```r
./test.sh
```
=======


## Not done ##

- the authors use a memory efficient kd tree: they only record the position of the source and recover more info as needed.
- we should compute the kdtree only once per scale level, but I ran into odd segfaults errors as I tried...