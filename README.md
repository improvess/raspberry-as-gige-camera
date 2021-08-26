# raspberry-as-gige-camera

Transform your USB camera in a gigE camera with Raspberry PI.

## TL;DR;

The code in this repository allows you to retrieve images from a USB camera via ethernet or wifi on a Raspberry PI at high speed for realtime applications.

![image](https://user-images.githubusercontent.com/9665358/130778605-99adcd9d-6081-465c-8dde-13ddadce4a13.png)

## Why?

USB cameras are great, powerful and cheap but USB cables/connectors are not so robust / reliable / long range if compared to ethernet links. GigE cameras are great but the total cost and availability of this type of cameras can be challenging for some solutions.

## Examples of usage

Grabbing 320x240 images from a [Microsoft Lifecam Studio](https://www.microsoft.com/en-ww/accessories/products/webcams/lifecam-studio) at 30 fps.

![image](https://user-images.githubusercontent.com/9665358/130779743-b97e4d8d-5367-46c5-9202-b6bdd8eb7154.png)

Note that 30 fps is the max camera model frame rate.

Achieving 60 fps @ 320x240 using a [Sony Playstation 3 Eye camera](https://en.wikipedia.org/wiki/PlayStation_Eye):

![image](https://user-images.githubusercontent.com/9665358/130841632-068dc38e-1f1d-4212-993f-d3e9ebe54040.png)

## Building

This repo uses CMake & OpenCV to build the code.

```
$ git clone https://github.com/doleron/raspberry-as-gige-camera.git
$ cd raspberry-as-gige-camera
$ mkdir build
$ cd build
$ cmake ..
$ make -j4
```

Obs.: So far, I have used only Raspbian OS 64 bit and GCC 10.3 for the development. 

### Building tests

To tun the googletest unit test batch:

```
$ cmake -DBUILD_TESTS=ON ..
$ ./test_rpiasgige 
```

## Running

Run `rpiasgige` as follows:

```
pi@raspberrypi:~/raspberry-as-gige-camera/build $ ./rpiasgige -port=4001 -device=/dev/video0
DEBUG - /dev/video0 - Not initialized. Initializing now.
DEBUG - /dev/video0 - successfuly initialized.
DEBUG - /dev/video0 - Waiting for client
```

## Remote command examples

Once `rpiasgige` is running, it is ready to respond to incoming TCP requests on the defined port. Check out for some examples of request below. 

Obs. 1: the examples consider that RPI is set to use 192.168.2.2 as IP address. Replace it with the proper IP.
Obs. 2: note that the examples supposes that you are running them on a remote linux machine.
Obs. 3: the examples basically send TCP packages via `nc` linux command.

Check here (pending) for more details about the package format.

![image](https://user-images.githubusercontent.com/9665358/130778217-62a2008a-bed5-43c5-9ec5-a72e46b1fc2f.png)

### Opening the device

Usually the first thing to do is opening the device. This can be done in a similar form like opening a local USB camera before retrieving data:

```
$ echo -e "OPEN0\x0\x0\x0\x0" | nc 192.168.2.2 4001
```

### Setting resolution width to 320

```
$ echo -e "SET00\xC\x0\x0\x0\x3\x0\x0\x0\x0\x0\x0\x0\x0\x0\x74\x40" | nc 192.168.2.2 4001
```

### Getting the frame resolution width

```
$ echo -e "GET00\x4\x0\x0\x0\x3\x0\x0\x0" | nc 192.168.2.2 4001
```

### Setting the resolution height to 240

```
$ echo -e "SET00\xC\x0\x0\x0\x4\x0\x0\x0\x0\x0\x0\x0\x0\x0\x6E\x40" | nc 192.168.2.2 4001
```

### Setting the size of camera buffer to 2

```
$ echo -e "SET00\xC\x0\x0\x0\x26\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x40" | nc 192.168.2.2 4001
```

### Checking if the device is opened already

```
$ echo -e "ISOP0\x0\x0\x0\x0" | nc 192.168.2.2 4001
```

### Setting the autofocus ON

Note that the autofocus feature may be unavailable for the model of the USB camera in use.

```
$ echo -e "SET00\xC\x0\x0\x0\x27\x0\x0\x0\x0\x0\x0\x0\x0\x0\xF0\x3F" | nc 192.168.2.2 4001
```

### Setting autofocus OFF

```
$ echo -e "SET00\xC\x0\x0\x0\x27\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0" | nc 192.168.2.2 4001
```

### Setting focus to 0

```
$ echo -e "SET00\xC\x0\x0\x0\x1C\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0" | nc 192.168.2.2 4001
```

### Asking camera to run at 60 fps

```
$ echo -e "SET00\xC\x0\x0\x0\x5\x0\x0\x0\x0\x0\x0\x0\x0\x0\x4E\x40" | nc 192.168.2.2 4001
```

### Asking camera to run at 120 fps

```
$ echo -e "SET00\xC\x0\x0\x0\x5\x0\x0\x0\x0\x0\x0\x0\x0\x0\x5E\x40" | nc 192.168.2.2 4001
```

### Closing the device

```
$ echo -e "CLOS0\x0\x0\x0\x0" | nc 192.168.2.2 4001
```

### Setting fourcc to MJPG

```
$ echo -e "SET00\xC\x0\x0\x0\x6\x0\x0\x0\x0\x0\x40\x93\x12\xD4\xD1\x41" | nc 192.168.2.2 4001
```

## Protocol

The communication obay a simple request-response protocol. The program works as a single-channel TCP server, serving only one client conversation by turn.

### The conversation between server & client 

The conversation between the client and server begins when the client connects to the server. The conversation finishes when one of the following events occurs:

- The client closes the connection
- The server closes the connection
- A timeout occurs

During a conversation, the client can send an undertermined number of requests to the server. The client must set the keep-alive byte if it wants to make more than one succesive request. The position of the keep-alive byte and other package fields are discussed below.

### The packt format

The packat has the following format:

![image](https://user-images.githubusercontent.com/9665358/130887559-d8a3c0d9-7783-484e-a306-86e3189974f7.png)

The three first fields, namely **status**, **keep-alive** and **data size**, have predefined sizes in bytes (4, 1 and 4, respectivelly). The **data** field is the only one with an undetermined number of bytes. In a well-formed packat, the size of the data segment is set in the **data-size** field.

Obs.: for several practical reasons, the server assumes that **data-size** is bounded to a max positive value.

## Limitations

According to [this](https://www.raspberrypi.org/documentation/computers/processors.html), the L2 shared cache of Raspberry PI 4 processor (the one I am using right now) is set to 1MB. This bottleneck can reduce the amount of traffic data/FPS sent/received. 

## Disclaimer

This code is in its early stages. It is not tested neither ready for production yet at all.

## Next steps

- tests
- more tests
- test a bit more
- remove the OpenCV dependence
- Remote C++ API
- remote client example

