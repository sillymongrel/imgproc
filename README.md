imgproc
=======

Image processing code

## Build
```
g++ histeq.cc -o histeq
```

## Usage
```
$ histeq <inimg> <outimg>
```
Histogram equalises <inimg> and stores the result in <outimg>

## Test
An exmaple image is included in the data folder. To test run the following:
```
$ histeq data/Flowerbed.pgm result.pgm
```

View the input and output images side-by-side to confirm the equalisation took place.

