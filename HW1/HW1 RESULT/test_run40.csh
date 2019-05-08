#!/bin/tcsh -f

( ./run_test test40 >! out40) >& err40
diff -w  out40 expout40
diff -w  err40 experr40

