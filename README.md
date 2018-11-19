# Big Red Button

Sometimes you just need a big red button to do stuff.

This repo contains an Arduino sketch for emitting button (switch) toggle events over a serial device
and go code for listening to these events on the other end of the serial connection.
It is intended that the Arduino (or alternative, eg. Wemos D1 mini) is connected to a Bluetooth
HC-05 board linked to a computer running the go code.