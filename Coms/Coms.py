#!/usr/bin/env pybricks-micropython
import uasyncio as asyncio
from machine import UART

# Configure UART for Bluetooth communication
bt_uart = UART(2, baudrate=9600, tx=17, rx=16)  # Adjust tx and rx pins as needed

async def bluetooth_echo():
    while True:
        if bt_uart.any():
            data = bt_uart.read()
            bt_uart.write(data)  # Echo received data

async def main():
    asyncio.create_task(bluetooth_echo())
    while True:
        await asyncio.sleep(1)  # Adjust the sleep duration if needed

asyncio.run(main())