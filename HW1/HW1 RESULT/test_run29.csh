#!/bin/tcsh -f

( ./run_test test29 >! out29) >& err29
diff -w  out29 expout29
diff -w  err29 experr29

