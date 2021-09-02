# Step-by-step tutorial

# Status 

Sorry it isn't done yet!

## Material List

- 1 Raspberry Pi 3 or 4
- 1 SD Card class 10 with 16 GB or more
- 1 SD Card to USB adapter
- 1 USB Camera
- 1 Power Source Unit
- 1 Computer or Notebook with internet access
- 1 ethernet cable

## Getting started with Raspberry Pi

If you are tottaly new to Raspberry PI, follow the Raspberry Pi's getting started official tutorial: https://projects.raspberrypi.org/en/projects/raspberry-pi-getting-started

Else, move to the next section!

## Install dependences

Open terminal and execute the following commands:

```
sudo apt update
sudo apt upgrade
sudo apt install python3-opencv git build-essential cmake binutils-dev
```

## Clone & build `rpiasgige` server

After install dependences, execute the following commands to build `rpiasgige` server:

```
git clone https://github.com/doleron/raspberry-as-gige-camera.git
cd raspberry-as-gige-camera/code/server
mkdir build
cd build
cmake ..
make -j4
```

## Run `rpiasgige` server

Connect your USB camera to one of Raspiberry Pi' USB ports. After it, run the `rpiasgige` server by executing:

```
./rpiasgige
```
The expected output is:

```

```
Note: You can stop the `rpiasgige` server by pressing control+c

By default, `rpiasgige` uses the device /dev/video0 and TCP port 4001. You can use different settings, for example:


```
./rpiasgige
```
