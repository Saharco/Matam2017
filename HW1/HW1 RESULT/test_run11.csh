#!/bin/tcsh -f

( ./run_test test11 >! out11) >& err11
diff -w  out11 expout11
diff -w  err11 experr11

