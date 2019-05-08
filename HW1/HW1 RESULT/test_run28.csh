#!/bin/tcsh -f

( ./run_test test28 >! out28) >& err28
diff -w  out28 expout28
diff -w  err28 experr28

