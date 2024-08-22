#!/usr/bin/env pybricks-micropython
from pybricks.hubs import EV3Brick
from pybricks.ev3devices import (Motor, TouchSensor, ColorSensor,
                                 InfraredSensor, UltrasonicSensor, GyroSensor)
from pybricks.iodevices import LUMPDevice
from pybricks.parameters import Port, Stop, Direction, Button, Color
from pybricks.tools import wait, StopWatch, DataLog
from pybricks.robotics import DriveBase
from pybricks.media.ev3dev import SoundFile, ImageFile, Font
import time as t
import ffi, os

# ------------------- Funciones de C ------------------- #
lib = ffi.open(os.getcwd()+"/Trazador.so")
Trazador = lib.func("i", "Trazador", "fff")   # Función main
# ------------------------------------------------------ #
GyroD = GyroSensor(Port.S2)

Trazador(1000, 200, 0)