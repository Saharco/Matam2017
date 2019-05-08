#!/bin/tcsh -f

( ./run_test test18 >! out18) >& err18
diff -w  out18 expout18
diff -w  err18 experr18

