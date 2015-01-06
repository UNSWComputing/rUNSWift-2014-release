#!/bin/bash
# Remember to change the robot name
# Afterwards, ssh in to robot, chmod +x rm_connman.sh, and run that script

ROBOT=tank
scp $RUNSWIFT_CHECKOUT_DIR/utils/networking/net root@$ROBOT.local:/etc/conf.d/net
scp $RUNSWIFT_CHECKOUT_DIR/utils/networking/wpa_supplicant.conf root@$ROBOT.local:/etc/wpa_supplicant/wpa_supplicant.conf
scp $RUNSWIFT_CHECKOUT_DIR/utils/networking/rm_connman.sh nao@$ROBOT.local:rm_connman.sh
