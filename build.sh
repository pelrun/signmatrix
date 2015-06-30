export PATH=/c/Program\ Files\ \(x86\)/Arduino:$PATH

arduino_debug.exe --verify --board "esp8266:esp8266:nodemcu" --pref build.path=$PWD/build $PWD/signmatrix.ino
