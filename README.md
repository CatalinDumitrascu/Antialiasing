# Antialiasing

Implementation of a threaded C program capable of resizing a given image, shrinking the loss of information through super-sampling antialiasing. It is built using the API offered by the pthread library, and it scales with the number of threads available.

The I/O of this program are images in the .pnm (colour) or .pgm (grayscale) formats. We also need to input a resizeFactor for our image. The program splits the pixel matrix into squares of size resizeFactor and attributes them to our threads.
