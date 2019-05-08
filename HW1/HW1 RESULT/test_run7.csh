#!/bin/tcsh -f

( ./run_test test7 >! out7) >& err7
diff -w  out7 expout7
diff -w  err7 experr7

