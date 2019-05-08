#!/bin/tcsh -f

( ./run_test test4 >! out4) >& err4
diff -w  out4 expout4
diff -w  err4 experr4

