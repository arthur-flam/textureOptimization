Texture Optimization
====================

Implementation of ["Texture Optimization for Example-based Synthesis"](http://www.cc.gatech.edu/cpl/projects/textureoptimization/TO-final.pdf) by Vivek Kwatra at al.

## Try it

```r
cmake . && make && ./DisplayImage input/ztext.bmp
```

## To do ##

- robust optimization 
- graph energy vs iterations
- create output on all images


## Not done ##

- the authors use a memory efficient kd tree: they only record the position of the source and recover more info as needed.
- we should compute the kdtree only once per scale level, but I ran into odd segfaults errors as I tried...
- we should get rig of edge effects using some padding
