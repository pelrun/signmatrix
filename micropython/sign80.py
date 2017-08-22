# sign code

import machine
from machine import Pin,SPI
from umqtt.robust import MQTTClient
import framebuf
import time
import network

class SignMatrix(object):
    def __init__(self, width=160):
        self._a2 = Pin(4, Pin.OUT, value=0)
        self._a1 = Pin(16, Pin.OUT, value=0)
        self._a0 = Pin(5, Pin.OUT, value=0)
        self._latch = Pin(0, Pin.OUT, value=0)
        self._clock = Pin(14, Pin.OUT)
        self._data = Pin(13, Pin.OUT)
        self._irdata = Pin(12, Pin.IN)
        self.spi = SPI(1, baudrate=2000000, polarity=0, phase=0)
        self._nenable = Pin(2, Pin.OUT, value=1)
        self._width = width
        self._stride = width//8
        self._fbuffer = memoryview(bytearray(self._stride*7))
        self.fb = framebuf.FrameBuffer(self._fbuffer, width, 7, framebuf.MONO_HLSB)
        self.fb.fill(1)
        self._delay=200

    def show(self):
        state = machine.disable_irq()
        for y in range(7,-1,-1):
            self._writerow(y)
        machine.enable_irq(state)

    def _writerow(self, y):
        # shift out row data
        self.spi.write(self._fbuffer[self._stride*y:self._stride*y+self._stride-1])
        # latch row data
        self._latch.on()
        self._latch.off()
        # set row
        self._a0.value(y&1)
        self._a1.value(y&2)
        self._a2.value(y&4)
        # turn on output
        self._nenable.value(0)
        time.sleep_us(self._delay)
        self._nenable.value(1)

    def set_delay(self, delay):
        self._delay = delay

    def scroller(self, text):
        for ch in text:
            self.fb.fill_rect(0,0,8,7,1)
            self.fb.text(ch,0,0,0)
            w = 7
            if ch == 'W' or ch == 'M' or ch == "m" or ch == "w":
                w = 8
            for b in range(0,w):
                self.show()
                self.fb.scroll(1,0)

#text = "THIS IS THE SCROLLTEXT THAT DOESN'T END YES IT GOES ON AND ON MY FRIEND SOME PEOPLE STARTED READING IT NOT KNOWING WHAT IT WAS AND THEY'LL JUST KEEP ON READING IT FOREVER JUST BECAUSE "
text = "No message set "
delay = 700
enable = 0

def sub_cb(topic, msg):
    global text, delay, enable
    print((topic,msg))
    if topic == b'devices/main-sign/text':
        text = str(msg,'ascii')
    if topic == b'devices/main-sign/delay':
        delay = int(msg)
    if topic == b'devices/main-sign/enable':
        enable = int(msg)

def main():
    machine.freq(160000000)

    c = MQTTClient("HSBNE-small-sign", "10.0.1.253", user="", password="")
    c.set_callback(sub_cb)
    c.set_last_will("devices/small-sign/state","offline", retain=True)
    c.connect()
    c.subscribe("devices/small-sign/#")
    c.publish("devices/small-sign/state","online", retain=True)
    c.publish("devices/main-sign/ip",network.WLAN(network.STA_IF).ifconfig()[0],
              retain = True)

    sign = SignMatrix()
    try:
        while True:
            while c.check_msg():
                pass
            if enable != 0:
                sign.set_delay(delay)
                sign.scroller(text)
            else:
                c.wait_msg()
    except KeyboardInterrupt:
        c.publish("devices/small-sign/state","offline", retain=True, qos=1)
        c.disconnect()

def test():
    machine.freq(160000000)
    sign = SignMatrix(160)
    sign.fb.text("A",0,0,0)
    while True:
        sign.show()

#main()
#test()

if __name__ == '__main__':
    timeout = 0
    while not network.WLAN().isconnected():
        time.sleep(1)
        if timeout>10:
            SignMatrix().scroller("Connection Fail!              ")
        else:
            timeout = timeout + 1

    main()

