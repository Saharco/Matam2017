#!/bin/tcsh -f

( ./run_test test33 >! out33) >& err33
diff -w  out33 expout33
diff -w  err33 experr33

