#!/bin/tcsh -f

( ./run_test test24 >! out24) >& err24
diff -w  out24 expout24
diff -w  err24 experr24

