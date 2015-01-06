"""
HOW TO USE THESE.

Install the dependencies using the requirements file in this folder.
sudo pip install -r requirements.txt

Run commands in this file like so:
fab -H helo,husker,athena ifconfig

Google Python Fabric for more details / docs, but some of these below should help guide you
if what you're trying to do is simple.
"""

from fabric.api import run, sudo, env

env.use_ssh_config = True

def nao_restart():
   sudo('/etc/init.d/naoqi restart', pty=True)

def say_hello():
   run("/usr/bin/flite -t hello")

def ifconfig():
   run("ifconfig")

def check_wifi():
   run("ifconfig | grep wlan0 -A 4")
   check_wpa()

def check_wpa():
   sudo("wpa_cli status")

def change_field(fieldName):
   sudo("python /home/nao/bin/changeField.py %s" % fieldName)
   restart_wireless()

def restart_wireless():
   sudo("/etc/init.d/runswiftwireless restart")

def which_wifi():
   run("cat /etc/wpa_supplicant/wpa_supplicant.conf | grep ssid")

def setup_for_game(fieldName):
   change_field(fieldName)
   restart_wireless()
   which_wifi()
   check_wifi()
