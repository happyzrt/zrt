sed -i '/net.ipv4.ip_forward/d' /etc/sysctl.conf
echo 'net.ipv4.ip_forward=1'>>/etc/sysctl.conf
sysctl -p
iptables -t nat -D POSTROUTING -s 192.168.13.0/24 -o eth0 -j MASQUERADE
iptables -t nat -A POSTROUTING -s 192.168.13.0/24 -o eth0 -j MASQUERADE
