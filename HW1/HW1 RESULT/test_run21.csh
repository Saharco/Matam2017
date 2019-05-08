#!/bin/tcsh -f

( ./run_test test21 >! out21) >& err21
diff -w  out21 expout21
diff -w  err21 experr21

