#!/bin/tcsh -f

( ./run_test test36 >! out36) >& err36
diff -w  out36 expout36
diff -w  err36 experr36

