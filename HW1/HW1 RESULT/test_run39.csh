#!/bin/tcsh -f

( ./run_test test39 >! out39) >& err39
diff -w  out39 expout39
diff -w  err39 experr39

