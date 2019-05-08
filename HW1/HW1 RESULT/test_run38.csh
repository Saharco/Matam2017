#!/bin/tcsh -f

( ./run_test test38 >! out38) >& err38
diff -w  out38 expout38
diff -w  err38 experr38

