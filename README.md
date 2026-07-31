# Goal
Make a decent chess engine solely in c++.

# Current State
Chess board and the ruleset have been built

# Installing dependencies
MacOS:
run
```bash
brew install cmake make sdl3 sdl3_image
```

# How to run
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