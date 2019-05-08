#!/bin/tcsh -f

( ./run_test test13 >! out13) >& err13
diff -w  out13 expout13
diff -w  err13 experr13

