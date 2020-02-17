#!/bin/bash
PHASE=$1
ARCHIVE=$PHASE.tar.gz

tar -czvf $ARCHIVE uarm/ umps/ src/ include/ makefiles/ Makefile AUTHORS README.md Doxyfile
scp $ARCHIVE mattia.guazzaloca@aremberg.cs.unibo.it:/home/students/LABSO/2020/submit_phase1/lso20az03