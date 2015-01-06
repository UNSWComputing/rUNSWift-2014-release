#!/bin/bash
sudo ln -sfF /etc/init.d/net.lo /etc/init.d/net.eth0
sudo ln -sfF /etc/init.d/net.lo /etc/init.d/net.wlan0
sudo rc-update add net.eth0 boot
sudo rc-update add net.wlan0 boot
sudo rc-update del connman boot
sudo bash -c 'echo rc_depend_strict=\"NO\" >> /etc/rc.conf'


