#!/bin/bash
sigrok-cli -d dreamsourcelab-dslogic -C 0-7 -c samplerate=20m:voltage_threshold=1.4-3.6 --time 1s $*
