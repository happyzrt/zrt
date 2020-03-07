iptables -A FORWARD -i tap0 -j ACCEPT
iptables -t nat -A POSTROUTING -s 192.168.7.0/24 -o eth0 -j MASQUERADE
echo 'net.ipv4.ip_forward=1'>>/etc/sysctl.conf
sysctl -p
