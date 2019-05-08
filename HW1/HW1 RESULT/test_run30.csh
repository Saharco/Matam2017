#!/bin/tcsh -f

( ./run_test test30 >! out30) >& err30
diff -w  out30 expout30
diff -w  err30 experr30

