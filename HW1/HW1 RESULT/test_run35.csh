#!/bin/tcsh -f

( ./run_test test35 >! out35) >& err35
diff -w  out35 expout35
diff -w  err35 experr35

