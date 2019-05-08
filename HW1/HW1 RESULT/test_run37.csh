#!/bin/tcsh -f

( ./run_test test37 >! out37) >& err37
diff -w  out37 expout37
diff -w  err37 experr37

