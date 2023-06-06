old_gate_way=`/usr/sbin/netstat -rn -f inet|/usr/bin/grep "192.168.13.1"|/usr/bin/awk 'NR==1{print $2}'`
if [[ ${old_gate_way} != "192.168.13.1" ]]
then
    echo "no need restore"
    exit 0
fi
gate_way=`/usr/sbin/netstat -rn -f inet|/usr/bin/grep $1|/usr/bin/awk 'NR==1{print $2}'`
/sbin/route delete default
/sbin/route -n add  default ${gate_way}
/sbin/route delete  $1 -gateway ${gate_way}
/usr/sbin/networksetup -setdnsservers WI-FI ${gate_way}
