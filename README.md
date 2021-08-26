# raspberry-as-gige-camera

Transform your USB camera in a camera gigE with Raspberry PI.

## TL;DR;

The code in this repository allows you to retrieve images from a USB camera via the Raspberry PI's gigabyte ethernet port at high speed for realtime applications.

![image](https://user-images.githubusercontent.com/9665358/130778605-99adcd9d-6081-465c-8dde-13ddadce4a13.png)

## Why?

USB cameras are great, powerful and cheap but USB cables/connectors are not so robust / reliable / long range if compared to ethernet links / hubs / infrastructure. On the other hand, GigE cameras are great but the total cost and availability of this type of devices can be challenging for some projets. On scenarios like this, Raspberry PI boards can be used to create ethernet interfaces for the USB camera. This is what this repository is intended to.

## Examples of usage

Grabbing 320x240 images from a [Microsoft Lifecam Studio](https://www.microsoft.com/en-ww/accessories/products/webcams/lifecam-studio) at 30 fps.

![image](https://user-images.githubusercontent.com/9665358/130779743-b97e4d8d-5367-46c5-9202-b6bdd8eb7154.png)

Note that 30 fps is the max camera model frame rate.

Achieving 60 fps @ 640x480 using a [Sony Playstation 3 Eye camera](https://en.wikipedia.org/wiki/PlayStation_Eye):

![image](https://user-images.githubusercontent.com/9665358/130841632-068dc38e-1f1d-4212-993f-d3e9ebe54040.png)

## Building

This repo uses [CMake](https://cmake.org/) and [OpenCV](https://opencv.org/) to build the code on a [Raspberry PI OS](https://www.raspberrypi.org/software/) or similar operating system.

```
$ git clone https://github.com/doleron/raspberry-as-gige-camera.git
$ cd raspberry-as-gige-camera
$ mkdir build
$ cd build
$ cmake ..
$ make -j4
```

Obs. 1: Check [here](https://www.pyimagesearch.com/2018/09/26/install-opencv-4-on-your-raspberry-pi/), [here](https://www.jeremymorgan.com/tutorials/raspberry-pi/how-to-install-opencv-raspberry-pi/) or [here](https://learnopencv.com/install-opencv-4-on-raspberry-pi/) to learn how to install OpenCV on Raspberry PI.

Obs. 2: If not yet, do not forget to install `git`, `cmake` and `gcc` before building:

```
sudo apt install git build-essential cmake
```

## Running `rpiasgige`

After building, run the `rpiasgige` server as follows:

```
pi@raspberrypi:~/raspberry-as-gige-camera/build $ ./rpiasgige -port=4001 -device=/dev/video0
DEBUG - /dev/video0 - Not initialized. Initializing now.
DEBUG - /dev/video0 - successfuly initialized.
DEBUG - /dev/video0 - Waiting for client
```

## Communication & getting images from the remote camera

Once `rpiasgige` is running, it is ready to respond to incoming TCP requests on the defined port and serve images. There are three ways to do it:

- Sending command line requests using native TCP programs like [Netcat](https://linuxize.com/post/netcat-nc-command-with-examples). See this examples.
- Using the provided client program
- Using the provided client API
- Writing your own remote calls using the [rpiasgige protocol](https://github.com/doleron/raspberry-as-gige-camera/blob/main/protocol.MD)

Examples of sending commands to the camera via command line:

![image](https://user-images.githubusercontent.com/9665358/130778217-62a2008a-bed5-43c5-9ec5-a72e46b1fc2f.png)

### Building and running the tests

`rpiasgige` is shipped with a set of unit tests. You can build it and run as follows:

```
$ cmake -DBUILD_TESTS=ON ..
$ ./test_rpiasgige 
```

## Limitations

Raspberry PI boards are general purpose devices, not exaclty intended to perform in intensive computing settings, having significative hardware limitations which must be taken in mind when considered using them for high CPU/IO tasks.

According to [this](https://www.raspberrypi.org/documentation/computers/processors.html), the L2 shared cache of Raspberry PI 4 processor is set to 1MB whereas the same cache is constrained to only 512 KB in RPI 3 boards. This bottleneck eventually reduces the amount of traffic data/FPS sent/received.

Note taht some RPI boards such as model A and zero don't have a built-in ethernet interface whereas some old models do not have even a wifi interface. For narrowed boards like this you can attach a USB-to-ethernet adapter as a very last solution but do not expect to achieve a high data throughtput.

## Disclaimer

This code is in its early stages. It is not tested neither ready for production yet at all.

## Next steps

- tests
- more tests
- test a bit more
- remove the OpenCV dependence
- Remote C++ API
- remote client example

