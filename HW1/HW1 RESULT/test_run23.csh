#!/bin/tcsh -f

( ./run_test test23 >! out23) >& err23
diff -w  out23 expout23
diff -w  err23 experr23

