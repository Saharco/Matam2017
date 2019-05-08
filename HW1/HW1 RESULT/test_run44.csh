#!/bin/tcsh -f

( ./run_test test44 >! out44) >& err44
diff -w  out44 expout44
diff -w  err44 experr44

