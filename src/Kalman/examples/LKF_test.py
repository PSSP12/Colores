#!/usr/bin/env pybricks-micropython
# from pybricks.hubs import EV3Brick
# from pybricks.ev3devices import (Motor, TouchSensor, ColorSensor,
#                                  InfraredSensor, UltrasonicSensor, GyroSensor)
# from pybricks.iodevices import LUMPDevice
# from pybricks.parameters import Port, Stop, Direction, Button, Color
# from pybricks.tools import wait, StopWatch, DataLog
# from pybricks.robotics import DriveBase
# from pybricks.media.ev3dev import SoundFile, ImageFile, Font
# import time as t
import ffi, os

# mem_usage = os.system('cat /proc/meminfo')
# print(mem_usage)

# ------------------- Funciones de C ------------------- #
lib = ffi.open(os.getcwd()+"/LKF_test.so")
main = lib.func("i", "main", "v")   # Función main

# lib = ffi.open(os.getcwd()+"/mem.so")
# main = lib.func("i", "main", "i")   # Función main
# ------------------------------------------------------ #

print("Started c")
main()
print("Ended c")