# sign code

import machine
from machine import Pin,SPI
from umqtt.robust import MQTTClient
import framebuf
import time

class SignMatrix(object):
    def __init__(self, width=480):
        self._a2 = Pin(4, Pin.OUT, value=0)
        self._a1 = Pin(16, Pin.OUT, value=0)
        self._a0 = Pin(5, Pin.OUT, value=0)
        self._latch = Pin(0, Pin.OUT, value=0)
        self._clock = Pin(14, Pin.OUT)
        self._data = Pin(13, Pin.OUT)
        self._irdata = Pin(12, Pin.IN)
        self.spi = SPI(1, baudrate=20000000, polarity=1, phase=1)
        self._nenable = Pin(2, Pin.OUT, value=1)
        self._width = width
        self._stride = width//8
        self._fbuffer = memoryview(bytearray(self._stride*7))
        self.fb = framebuf.FrameBuffer(self._fbuffer, width, 7, framebuf.MONO_HLSB)
        self.fb.fill(0)
        self._delay=200

    def show(self):
        for y in range(7,-1,-1):
            self._writerow(y)

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
        state = machine.disable_irq()
        self._nenable.value(0)
        time.sleep_us(self._delay)
        self._nenable.value(1)
        machine.enable_irq(state)

    def set_delay(self, delay):
        self._delay = delay

    def scroller(self, text):
        for ch in text:
            self.fb.fill_rect(484,0,4,7,0)
            self.fb.text(ch,479,0)
            for b in range(0,7):
                self.show()
                self.fb.scroll(-1,0)

#text = "THIS IS THE SCROLLTEXT THAT DOESN'T END YES IT GOES ON AND ON MY FRIEND SOME PEOPLE STARTED READING IT NOT KNOWING WHAT IT WAS AND THEY'LL JUST KEEP ON READING IT FOREVER JUST BECAUSE "
text = ""
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

    c = MQTTClient("HSBNE-main-sign", "10.0.2.120")
    c.set_callback(sub_cb)
    c.set_last_will("devices/main-sign/state","offline", retain=True)
    c.connect()
    c.subscribe("devices/main-sign/#")
    c.publish("devices/main-sign/state","online", retain=True)

    sign = SignMatrix(488)
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
        c.publish("devices/main-sign/state","offline", retain=True, qos=1)
        c.disconnect()

if __name__ == '__main__':
    main()
