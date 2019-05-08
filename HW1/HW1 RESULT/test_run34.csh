#!/bin/tcsh -f

( ./run_test test34 >! out34) >& err34
diff -w  out34 expout34
diff -w  err34 experr34

