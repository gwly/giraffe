rm -f error.out.*
cd ..
rm -f 9969.log
rm -f 22224.log
rm -f 39967.log
ulimit -c unlimited
ASAN_OPTIONS=verbosity=1:log_path=scripts/error.out ./monitor 2>&1 | tee scripts/print.txt 
