#!/bin/tcsh -f

( ./run_test test12 >! out12) >& err12
diff -w  out12 expout12
diff -w  err12 experr12

