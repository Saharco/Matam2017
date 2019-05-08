#!/bin/tcsh -f

( ./run_test test27 >! out27) >& err27
diff -w  out27 expout27
diff -w  err27 experr27

