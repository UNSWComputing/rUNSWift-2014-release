#!/usr/bin/python

"""

Need to run this with sudo or it can't write /etc/wpa_supplicant/wpa_supplicant.conf
"""

import sys

TEMPLATE = """
ctrl_interface=/var/run/wpa_supplicant
ctrl_interface_group=0
ap_scan=1

network={
  ssid="%(ssid)s"
  scan_ssid=1
  key_mgmt=WPA-PSK
  psk="%(password)s"
}
"""

if len(sys.argv) < 2:
   sys.exit("Needs field")

field = sys.argv[1]

if field == "runswift":
   ssid = "runswift"
   password = "runswift"
else:
   ssid = "SPL_%s" % field
   password = "Nao?!Nao?!"

print "Connecting to field %s" % ssid

filledIn = TEMPLATE % {
   "ssid": ssid,
   "password": password,
}

print filledIn

open("/etc/wpa_supplicant/wpa_supplicant.conf", "w").write(filledIn)
