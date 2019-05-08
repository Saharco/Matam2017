#!/bin/tcsh -f

( ./run_test test42 >! out42) >& err42
diff -w  out42 expout42
diff -w  err42 experr42

