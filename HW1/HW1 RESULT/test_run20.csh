#!/bin/tcsh -f

( ./run_test test20 >! out20) >& err20
diff -w  out20 expout20
diff -w  err20 experr20

