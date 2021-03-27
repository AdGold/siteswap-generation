Siteswap generation
---

A number of siteswap generation/counting programs and utilities.


## Compilation
All programs can be compiled simply using `make`.

## Programs

### Generation programs
* `siteswapping <balls> <period>` Prints out all siteswaps for the given balls/period using siteswaps by starting at bbbbb.
* `antipermutation <balls> <period>` Prints out all siteswaps for the given balls/period using the anti-permutation method from *The Mathematics of Juggling*.
* `stack <balls> <period> [-c] [-z]` Prints out all siteswaps for the given balls/period using stack notation. Optionally limits to only siteswaps with zeros in them and/or only those which may be composite (not any with only a single <balls> in their stack notation equivalent).

### Counting programs
* `count_prime <period> [-c] [-v] [-p]` Prints out a count of the prime (or composite if given -c) siteswaps fed into it, optionally print out the siteswaps themselves too using -p and a running count using -v.
* `gen_comp <balls> <period> [-f] [-z] [-v] [-p]` Prints out a count (and the actual patterns using -p) of composite patterns for a given balls/period (only those from an infinite state if not given -f), optionally only those with 0 (-z) and a running count using -v. This is much faster than generating all of them and using count_prime to limit.

## Examples

```
$ ./siteswapping 3 5 # print all 3 ball period 5 patterns
2 2 2 
1 2 3 
1 4 1 
5 0 1 
0 0 6 
3 0 3 
2 0 4 
$ ./antipermutation 3 1 # print all 3 ball period 1 patterns
6 0 
4 2 
5 1 
3 3 
$ ./stack 3 3 -z # print all 3 ball period 3 patterns with zeros
9 0 0 
8 0 1 
6 0 3 
7 2 0 
6 3 0 
4 5 0 
$ ./stack 7 6 -c | ./count_prime 6 -cv # count the number of 7 ball period 6 composite patterns, printing occaisional updates
Looked at: 0, count: 0
Looked at: 1048576, count: 14327
Looked at: 2097152, count: 28598
38228
$ ./gen_comp 10 10 -z # count the number of 10 ball period 10 patterns which are composite through an *infinite* state and contain a zero.
823887
$ ./gen_comp 10 10 -zf # count the number of composite 10 ball period 10 patterns with zeros, separating the counts of finite and infinite states.
840447
823887
16560
```

Argument parsing is done using ithe [args](https://github.com/Taywee/args) library.
