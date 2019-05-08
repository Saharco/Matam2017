#!/bin/tcsh -f

( ./run_test test41 >! out41) >& err41
diff -w  out41 expout41
diff -w  err41 experr41

