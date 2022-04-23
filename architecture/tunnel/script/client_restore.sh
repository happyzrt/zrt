gate_way=`netstat -rn -f inet|grep default|awk '{print $2}'`
route delete default
route -n add  default ${gate_way}
route delete  47.91.225.66 -gateway ${gate_way}
