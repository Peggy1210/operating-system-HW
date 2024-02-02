Operating System - Homework
===

## TODOs

In this project, we are going to implement a cooperative multithreading system.

## Run

Compile your code with SDCC using the provided Makefile. You should use the following two commands. The first one deletes all the compiled files so it forces a rebuild if you have compiled before. The second one compiles it.

```
$ make clean
$ make
```

The compiler should generate several `testcoop.*` files with different extensions:

* `.hex` file can be opened directly in EdSim51
* `.map` file shows the mapping of the symbols to their addresses after linking

Finally, open the `.hex` file in [EdSim51](http://www.edsim51.com/) and run the code. The system will start to run.