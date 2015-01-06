# -*- coding: utf-8 -*-
"""
  UnitTests for ALSoundProcessing Module

"""

from naoqi import ALProxy

IP = "127.0.0.1"
PORT = 9559

if __name__ == "__main__":

    proxy = ALProxy("ALSoundProcessing", IP, PORT)
    proxy.subscribe("MyModule")


