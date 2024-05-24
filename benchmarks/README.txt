upisati u terminal u vsc za mjerenje vremena
/usr/bin/time -v ./bloom_filter

mjerenje memorije
valgrind --tool=massif ./bloom_filter
ms_print massif.out.13548

