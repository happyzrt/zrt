pid=`/usr/bin/ps -ef|/usr/bin/grep tunnel|/usr/bin/grep -v grep|/usr/bin/grep -v stop|awk '{print $2}'`
echo $pid
/usr/bin/kill -s 9 $pid
