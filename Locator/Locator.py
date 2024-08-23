#!/usr/bin/env pybricks-micropython
import ffi, os

# ------------------- Funciones de C ------------------- #
lib = ffi.open(os.getcwd()+"/Locator.so")
main = lib.func("i", "main", "v")   # Función main
# ------------------------------------------------------ #

print("Started c")
main()
print("Ended c")