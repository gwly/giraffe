rm -f error.out.*
cd ..
rm -f 9969.log
rm -f 22224.log
rm -f 39967.log
ulimit -c unlimited
./stop.sh
nohup ./monitor &
