# motor_driver_mockup
An Arduino-controlled fake motor driver board used for test bed development.


This Project was written to drive a small LED board that I built to simulate the actual board under test.
While I hope to include schematics for the that board at some point, I think the general idea is pretty obvious: Arduino drives LEDs to simulate analog inputs for a sensor board that I also made using Arduino. There are also some buttons on my board to simulate fail states and quickly allow control of pins without using the serial or analog interfaces.

The board this project targeted is the Elegoo Uno R3.
