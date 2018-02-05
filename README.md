Signmatrix
----------

A simple driver for the HSBNE 480x7-pixel scrolling sign.

main.py and sign.py need to be transferred to the 8266.

```
mosquitto_sub -t devices/main-sign/#

mosquitto_pub -t devices/main-sign/enable -m 1 -r
mosquitto_pub -t devices/main-sign/text -m "Hello " -r
mosquitto_pub -t devices/main-sign/delay -m 700 -r
```
