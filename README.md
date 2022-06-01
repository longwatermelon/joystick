# joystick
[Freenove project 12](https://github.com/Freenove/Freenove_Ultimate_Starter_Kit_for_Raspberry_Pi) tutorial extension

# Usage
Build client on your machine:
```
$ git clone https://github.com/longwatermelon/joystick
$ cd joystick
$ make
```

Build server on pi:
```
$ git clone https://github.com/longwatermelon/joystick
$ cd joystick
$ g++ pi.cpp -lwiringPi -lADCDevice
```

Run server on pi: `./a.out`

Run client on your machine: `./a.out [pi ip]`

# Diagram
![image](https://user-images.githubusercontent.com/73869536/171482050-39896ffc-9fd9-4182-b0d3-6c453c25df49.png)
