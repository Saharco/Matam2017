#!/bin/tcsh -f

( ./run_test test17 >! out17) >& err17
diff -w  out17 expout17
diff -w  err17 experr17

