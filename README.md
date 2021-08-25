# raspberry-as-gige-camera
Transform your USB camera in a gigE camera with Raspberry PI.

## TLDR

The code in this repository allow you to retrieve images of a USB camera via ethernet or wifi on a Raspberry PI .

## Why?

USB cameras are great and cheap but USB cables/connectors are not so robust / reliable / long range if compared to ethernet links. GigE cameras are great but the total cost and availability of this type of cameras can be challenging for some solutions.

## Bulding

This repo uses CMake & OpenCV to build the code.

```
$ git clone https://github.com/doleron/raspberry-as-gige-camera.git
$ cd raspberry-as-gige-camera
$ mkdir build & cd build
$ cmake ..
$ make -j4
```

Obs.: So far, I have used only Raspbian OS 64 bit and GCC 10.3 for the development. 

## Running

```
$ ./rpiasgige -port=4001 -device=/dev/video2
```

## Remote command examples

Once `rpiasgige` is running, it is ready to respond to incoming TCP requests on the defined port. Check out for some examples of request below. 

Obs. 1: the examples consider that RPI is set to use 192.168.2.2 as IP address. Replace it with the proper IP.
Obs. 2: note that the examples supposes that you are running them on a remote linux machine.
Obs. 3: the examples basically send TCP packages via `nc` linux command.

Check here for more details about the package format.

### Open Device

The first thing to do is opening the device in a similar form like opening a local USB camera before retrieving data:

```
$ echo -e "OPEN0\x0\x0\x0\x0" | nc 192.168.2.2 4001
```

### set width to 320

```
$ echo -e "SET00\xC\x0\x0\x0\x3\x0\x0\x0\x0\x0\x0\x0\x0\x0\x74\x40" | nc 192.168.2.2 4001
```

### get width

```
$ echo -e "GET00\x4\x0\x0\x0\x3\x0\x0\x0" | nc 192.168.2.2 4001
```

### set height to 240

```
$ echo -e "SET00\xC\x0\x0\x0\x4\x0\x0\x0\x0\x0\x0\x0\x0\x0\x6E\x40" | nc 192.168.2.2 4001
```

### set buffer size to 2

```
$ echo -e "SET00\xC\x0\x0\x0\x26\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x40" | nc 192.168.2.2 4001
```

### Check if device is opened

```
$ echo -e "ISOP0\x0\x0\x0\x0" | nc 192.168.2.2 4001
```

### SET autofocus ON

Note that the autofocus feature cannot be available for the model of the USB camera in use.

```
$ echo -e "SET00\xC\x0\x0\x0\x27\x0\x0\x0\x0\x0\x0\x0\x0\x0\xF0\x3F" | nc 192.168.2.2 4001
```

### SET autofocus OFF

```
$ echo -e "SET00\xC\x0\x0\x0\x27\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0" | nc 192.168.2.2 4001
```

### SET focus to 0

```
$ echo -e "SET00\xC\x0\x0\x0\x1C\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0" | nc 192.168.2.2 4001
```

### Closing the device

```
$ echo -e "CLOS0\x0\x0\x0\x0" | nc 192.168.2.2 4001
```

### Set fourcc to MJPG

```
$ echo -e "SET00\xC\x0\x0\x0\x6\x0\x0\x0\x0\x0\x40\x93\x12\xD4\xD1\x41" | nc 192.168.2.2 4001
```

## Disclaimer

This code is in its early stages. It is not tested neither ready for production yet at all.

