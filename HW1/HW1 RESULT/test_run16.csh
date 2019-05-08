#!/bin/tcsh -f

( ./run_test test16 >! out16) >& err16
diff -w  out16 expout16
diff -w  err16 experr16

