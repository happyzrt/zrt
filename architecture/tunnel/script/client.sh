result=`/sbin/ping 192.168.13.1 -c 1`
if [[ $result =~ "0 packets received" ]]
then
    echo "ping failed"
    exit
fi
gate_way=`/usr/sbin/netstat -rn -f inet|/usr/bin/grep default|/usr/bin/awk 'NR==1{print $2}'`
/sbin/route -n add  $1 -gateway ${gate_way}
/sbin/route delete default
/sbin/route -n add  default 192.168.13.1
/usr/sbin/networksetup -setdnsservers WI-FI 100.100.2.138 100.100.2.136
