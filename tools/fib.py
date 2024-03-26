#! /usr/bin/python

import sys


if len(sys.argv) < 3:
    print "Not enough parameters. Please specify the count of Fibonacci numbers and the order!"
    sys.exit(1)


nr = eval(sys.argv[1])
order = eval(sys.argv[2])


if order < 2:
    print "The minimum order allowed is 2!"
    sys.exit(1)

    
f = [1] * order


for n in range(nr-1):
    sys.stdout.write(`f[-2]` + ", ")
    pom = f[0]

    nf = 0
    for fib in f:
        nf = nf + fib

    f = f[1:] + [nf]

sys.stdout.write(`f[-2]` + "\n")

