#!/bin/tcsh -f

( ./run_test test26 >! out26) >& err26
diff -w  out26 expout26
diff -w  err26 experr26

