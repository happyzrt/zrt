gate_way=`netstat -rn -f inet|grep default|awk '{print $2}'`
route -n add  47.91.225.66 -gateway ${gate_way}
route delete default
route -n add  default 192.168.13.1
