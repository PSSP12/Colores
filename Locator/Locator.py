#!/usr/bin/env pybricks-micropython
from pybricks.hubs import EV3Brick
from pybricks.ev3devices import (Motor, TouchSensor, ColorSensor,
                                 InfraredSensor, UltrasonicSensor, GyroSensor)
from pybricks.iodevices import LUMPDevice
from pybricks.parameters import Port, Stop, Direction, Button, Color
from pybricks.tools import wait, StopWatch, DataLog
from pybricks.robotics import DriveBase
from pybricks.media.ev3dev import SoundFile, ImageFile, Font
import ffi, os, time

# ------------------- C Ports ------------------- #
INPUT_1 = 49
INPUT_2 = 50
INPUT_3 = 51
INPUT_4 = 52
			
OUTPUT_A = 65
OUTPUT_B = 66
OUTPUT_C = 67
OUTPUT_D = 68
# ----------------------------------------------- #

# ------------------- C Functions ------------------- #
lib = ffi.open(os.getcwd()+"/Locator.so")
Start_locating = lib.func("i", "start_Locator", "ii")  # Locator thread
Stop_locating = lib.func("i", "stop", "v")           # Stop locator thread
# --------------------------------------------------- #

R_wheel = Motor(Port.B)
L_wheel = Motor(Port.A)

Start_locating(OUTPUT_A, OUTPUT_B)
print("Started c")
time.sleep(30)
Stop_locating()
print("Ended c")