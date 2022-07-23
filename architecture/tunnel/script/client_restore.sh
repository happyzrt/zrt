gate_way=`/usr/sbin/netstat -rn -f inet|/usr/bin/grep "47.91.225.66"|/usr/bin/awk '{print $2}'`
/sbin/route delete default
/sbin/route -n add  default ${gate_way}
/sbin/route delete  47.91.225.66 -gateway ${gate_way}
/usr/sbin/networksetup -setdnsservers WI-FI ${gate_way}
