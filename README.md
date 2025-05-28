# Colores
# Colores
System to get the position of the robot, and more.

## Description
This proyect aims to develop tools that enable EV3 based robots to handle precise position predictions and movement. Altough its main focus is the FIRST LEGO League, and similar competitions, some tools should work on other environments.

Most of the code is implemented in C, focusing on speed and memory optimization.

## List of available tools
- **Menu:** Custom menu to avoid the slow default one (on development)
- **Locator:** Allows for position estimation in real time, based on the speed of the wheels, and their diameter
- **Kalman Filters:** A series of Kalman filters, aiming to reduce the error on the gyros
- **Trazador:** A simple PID controler for controled movement
- **Image processing:** System to mix the readings of color sensors in, and allow more precise location estimations
