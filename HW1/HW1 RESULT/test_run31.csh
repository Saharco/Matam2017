#!/bin/tcsh -f

( ./run_test test31 >! out31) >& err31
diff -w  out31 expout31
diff -w  err31 experr31

