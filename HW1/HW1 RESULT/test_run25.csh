#!/bin/tcsh -f

( ./run_test test25 >! out25) >& err25
diff -w  out25 expout25
diff -w  err25 experr25

