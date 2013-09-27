
pid=$(ps aux | grep ./monitor | grep -v grep | awk '{print $2}')
kill -9 $pid
