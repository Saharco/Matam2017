#!/bin/tcsh -f

( ./run_test test43 >! out43) >& err43
diff -w  out43 expout43
diff -w  err43 experr43

