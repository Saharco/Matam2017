#!/bin/tcsh -f

( ./run_test test32 >! out32) >& err32
diff -w  out32 expout32
diff -w  err32 experr32

