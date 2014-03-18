Texture Optimization
====================

Implementation of ["Texture Optimization for Example-based Synthesis"](http://www.cc.gatech.edu/cpl/projects/textureoptimization/TO-final.pdf) by Vivek Kwatra et al.

The output images are generated on 50 iterations.

## Try it

You can generate a single image:
```r
cmake . && make && ./DisplayImage input/ztext.bmp
```

Or all of them
```r
./test.sh
```

## Not done ##

- the authors use a *memory efficient kd-tree*: they only record the position of the source and recover more info as needed.
- we should *compute the kdtree only once* per scale level, but I ran into odd segfaults errors as I tried...
- we don't compute origin patches for pixels on the edges. Nothing being done, it's very messy... We should get rid of those *edge issues* with some padding.
- we should remove the OpenNL dependency: our energy minimization consists only of weighted averages...

## Comparaison with original paper ##

* This implementation searches for similar patches using color information, not just intensity information.
* Performance needs some *tuning*: long-distance repetitive structures are not as nicely captured as in the paper. Tuning the initial scale should be enough to fix this.
* Flow-controled synthesis is not covered.

## Dependencies ##
* cmake
* [opencv](http://docs.opencv.org/doc/tutorials/introduction/linux_install/linux_install.html)
