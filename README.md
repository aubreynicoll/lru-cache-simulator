# lru-cache-simulator
Counts data cache hits, misses, and evictions based on a least-recently-used replacement policy.

This program is part of my solution to the Self-Study Cache Lab from Computer Systems: A Programmer's Perspective by Randal Bryant and David O'Hallaron.  

More information about this lab and the book can be found at the following link: http://csapp.cs.cmu.edu/3e/home.html

## Building
The included Makefile should be sufficient for building a executable binary:

```
$ git clone https://github.com/aubreynicoll/lru-cache-simulator.git
$ cd lru-cache-simulator
$ make
```

then run locally or move `cache-simulator` to your `$PATH`.

## Using the Program
The program reads data from a reference trace file as created by the Linux program `valgrind`. For example:

```
$ valgrind --log-fd=1 --tool=lackey -v --trace-mem=yes ls -l
```

runs `ls -l` and captures a trace of its memory accesses in the form of

```
I 0400d7d4,8
 M 0421c7f0,4
 L 04f6b868,8
 S 7ff0005c8,8
```

where `I` is an instruction, `M` is a modification, `L` is a load, and `S` is a store.

**`I` accesses are ignored by this program.**

### Arguments
`-t` sets the filepath.  
`-s` is passed an unsigned integer to control the number of sets (S) in the cache, where S = 2^s.  
`-b` is passed an unsigned integer to control the block size (B) of each cache line, where B = 2^b.  
`-E` is passed an unsigned integer to control how many cache lines are in each set. Cannot be zero.

The size of the cache (C) is calculated as `C = S * E * B`. For example:

```
$ cache-simulator -s 4 -E 4 -b 6 -t memtrace.txt
```

will create a cache containing 16 sets, each with 4 cache lines of size 64 bytes, for a total of 4,096B or 4kB.
