#!/bin/bash
PHASE=$1
USER=$2
ARCHIVE=phase$PHASE.tar.gz

tar -czvf $ARCHIVE uarm/ umps/ src/ include/ makefiles/ tests/p1.5test_bikaya_v0.c Makefile AUTHORS README.md Doxyfile
scp $ARCHIVE $USER@aremberg.cs.unibo.it:/home/students/LABSO/2020/submit_phase$PHASE/lso20az03