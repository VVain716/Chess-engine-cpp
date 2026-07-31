# Goal
Make a decent chess engine solely in c++.

# Current State
Chess board and the ruleset have been built

# How to run
Make sure that you have cmake, make and g++ installed.

Run
```bash
cmake -S . -B build
```
This will put all the build files in a directory called build.

Now, move into the build directory and run
```bash
make
```
This will execute a makefile (built by cmake) that will build the executable file. 

Finally, run
```bash
./chess-engine
``` 
to run the executable