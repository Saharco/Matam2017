#!/bin/tcsh -f

( ./run_test test9 >! out9) >& err9
diff -w  out9 expout9
diff -w  err9 experr9

