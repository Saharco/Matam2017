#!/bin/tcsh -f

( ./run_test test14 >! out14) >& err14
diff -w  out14 expout14
diff -w  err14 experr14

