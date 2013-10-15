cd ..
LD_LIBRARY_PATH=/usr/local/lib
export LD_LIBRARY_PATH
make 2>&1 | tee scripts/make.log
