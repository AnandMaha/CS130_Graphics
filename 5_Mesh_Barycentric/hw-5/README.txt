CODING INSTRUCTIONS

For the coding portion of this homework we will be starting out a ray tracer.
As the quarter goes by, we will be adding files and functionality to it.


COMPILATION

To compile the code, run "scons" from the commandline. If you do not already
have this program, you will need to install it. The tool is comparable in
function to the "make" utility you may be familiar with.

The SConstruct file is similar to a makefile in that it contains instructions
for scons on how to build the code. SConstruct files written in python. You may
need to modify this file to get things to compile on your computer. (For
example, if libraries are located in different places.) You may also need to
install other libraries (such as the dev files for libpng).


TEST CASES

The directory "2" contains the test cases for homework 2. You will get a new
directory of test cases each time we add something to the code (the next batch
of test cases will be in a directory called "3"). For example, the first test
case is 2/00.txt. The solution for this test case is 2/00.png. The main.cpp file
contains detailed instructions on how to run the program on test cases.
Although the test cases are intended to be completed in order, you may work on
them out of order if you prefer.


GRADING SCRIPT

You are given the grading script that will be used to grade the coding part of
the assignment. You can run it like this: "./grading-script.py 2" This tells the
grading script to grade the test cases in the directory "2", which is the test
cases for this assignment. For the next assignment, you will run
"./grading-script.py 3" instead. It is important that you are able to run the
grading script. It is a good idea to run this frequently. Students generally run
it every time they finish a test case to see which test to work on next.


GETTING STARTED

The code contains many comments explaining what needs to be done and what
functions should do. It is important to read through the code and comments
before you begin implementing things. Being aware of the functions and variables
that are available to you with help you write your code. The code and comments
also contain hints and clues to help you complete the assignments. Note that
some of the commented out code will be used in future assignments. As you
receive the relevant code, you will need to uncomment those lines. Many of the
functions that you need to implement are marked with 'TODO' in the code. The
TODO lines print out a simple message when executed to help you identify what
functions need to be implemented and where they are located. As you complete
these parts, you can remove the TODO lines to reduce output clutter.


PIXEL TRACES

Some test cases also come with a pixel trace.  These are debugging aids.  They
provide a detailed trace through the computations and intermediates that are
encountered while computing the color of one particular pixel.  It is highly
recommended that you add print statements to your code to print out information
equivalent to what is in the debug trace and compare the results.  (The
Pixel_Print function in misc.h should help you do this.)  They should match.

These files are named as ??-t.txt.  The first line of these files will be
something like "debug pixel: -x 350 -y 240".  In this case, to compare with the
trace you should add "-x 350 -y 240" to your commandline when you run your
program.  The 350,240 is the pixel that is being traced.  The pixel is colored
green in the output png so you can see visually what pixel is being traced.

These traces serve a number of useful roles.  (1) It helps track down the source
of many bugs.  For example, it can tell you whether your intersection routine is
wrong or whether you were correctly intersecting against the wrong ray.  (2)
They let you verify that various routines are working correctly.  For example,
it can tell you that you are correctly computing your rays or that your
intersection routines are correct.  (3) If you are having difficulties passing a
test case that has a pixel trace, matching the pixel trace (though tedious) will
often be the easiest path to a fix.  In fact, it might be easier to find a later
test case that does have a pixel trace.  If you are having difficulties in a
specific area or on a specific test case that does not already have a pixel
trace, you may request one from the instructor.


SUBMISSION INSTRUCTIONS

Please package up your code before you submit it (tar.gz, tar.bz2, or zip are
fine). Your submission should contain all of the source files that you modified
(*.h, *.cpp).  You must not modify main.cpp (if you submit a copy of this file,
it will be overwritten by the original).
