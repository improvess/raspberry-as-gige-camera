# raspberry-as-gige-camera

<p align="center">
  <img style="width: 100%" src="https://user-images.githubusercontent.com/9665358/131604597-bb869280-3ca3-47e2-89c4-efb248f1ce04.png">
</p>

Transform your Raspberry PI in an ethernet multicamera device for machine vision systems using USB / CSI cameras.

## TL;DR;

Expose your USB/CSI cameras as an ethernet device using Raspberry PI's gigabyte ethernet port. In other words, you can access your remote cameras just like you do with your local devices.

`rpiasgige` has an OpenCV-style API for [C++](https://github.com/doleron/raspberry-as-gige-camera/tree/main/code/client/cpp_api/examples) and [Python 3](https://github.com/doleron/raspberry-as-gige-camera/blob/main/code/client/python_api/src/main.py) (JavaScript & Java API's on the way). Check out the examples below:

- C++
```c++
#include "rpiasgige/client_api.hpp"

using namespace rpiasgige::client;

int main(int argc, char **argv) {

    Device camera("192.168.2.3", 4001);

    camera.open();

    cv::Mat image;
    camera.retrieve(image);

    cv::imshow(image);
    camera.release();
    cv::waitKey();
    
    return 0;
}
```
- Python 3
```python
from rpiasgige.client_api import Device

camera = Device("192.168.2.3", 4001)

camera.open()

ret, frame = camera.read()

cv.imshow("frame", frame)
cv.waitKey()

camera.release():
```

## Multicamera synchronization for machine vision

Due to caches, buffers and other video streaming features, sychronizing multi IP camera systems is not easy (see [OpenCV VideoCapture lag due to the capture buffer](https://stackoverflow.com/questions/30032063/opencv-videocapture-lag-due-to-the-capture-buffer) for example). This is easier to achieve with `rpiasgige`:

<p align="center">
  <img style="width: 100%" src="https://github.com/doleron/raspberry-as-gige-camera/blob/main/images/synchronization_test.PNG?raw=true">
</p>

In the example above, 6 USB cameras are attached to 2 Raspberry Pis and frames are grabbed at 30 FPS. The clock in the image is a millisecond precision clock. As the image shows, the max time difference between the images is about ±30 ms.

<table>
  <tr>
    <td>
<img style="width: 100%" src="https://github.com/doleron/raspberry-as-gige-camera/blob/main/images/6_cameras.jpg?raw=true">
    </td>
    <td>
<img style="width: 100%" src="https://github.com/doleron/raspberry-as-gige-camera/blob/main/images/2_rpis_2.jpg?raw=true">
    </td>
  </tr>
</table>

![image](https://user-images.githubusercontent.com/9665358/132226000-60041ff4-3b6d-439e-8206-e36d5de4475a.png)

## Getting started

Check out the [Step-by-step `rpiasgige` tutorial](https://github.com/doleron/raspberry-as-gige-camera/blob/main/docs/tutorial.MD).

## Why?

A real gigabyte camera (see [gigE](https://en.wikipedia.org/wiki/GigE_Vision)) is great but not cheap and in many situations cameras like this aren't available at all on stock/suppliers. In scenarios like this, you can use your Raspberry Pi as an alternative to provide an ethernet interface for your USB cameras and grab images them from via ethernet network even from long ranges.

## Examples of setup

<p align="center">
  <img style="width: 100%" src="https://user-images.githubusercontent.com/9665358/131751327-5bd5f00a-0838-49e3-bfa5-e63888d0c6e8.png">
</p>

## Examples of usage

Grabbing images at 100-150 fps with resolution 320x240 using a [Sony Playstation 3 Eye camera](https://en.wikipedia.org/wiki/PlayStation_Eye):

![image](https://user-images.githubusercontent.com/9665358/131229615-f0a73265-755d-4572-8946-17fb75ca8675.png)

Achieving 14-19 fps at 1280x720 using a [Microsoft Lifecam Studio](https://www.microsoft.com/en-ww/accessories/products/webcams/lifecam-studio).

![image](https://user-images.githubusercontent.com/9665358/131230242-ea0ed8ed-9590-42cd-8247-5f0094396bc0.png)

See also: [Accessing Raspberry CSI Camera Module remotely](https://youtu.be/iQk7xLSjUHw)

<p align="center">
  <a href="https://youtu.be/iQk7xLSjUHw" target="_blank"><img  width="400" src="https://user-images.githubusercontent.com/9665358/131565324-cf9e0e42-c595-4ea8-8438-47afdcf99bf5.png"></a>
</p>

Instructions how to build and run `rpiasgige` are shown below.

## Building & run

`rpiasgige` has two code sets:

- **server**: the application which runs on raspberry pi to expose the USB camera as an ethernet device
- **client**: API and utilities to allow programs to access the camera remotely

In this section, it is shown how to build the **server** side application. Check the next sections to know how to use the program client API's and see other ways to acess your camera from a remote computer.

### Building the server side

This repo uses [CMake](https://cmake.org/) and [OpenCV](https://opencv.org/) to build the code on a [Raspberry PI OS](https://www.raspberrypi.org/software/) or similar operating system.

```
$ git clone https://github.com/doleron/raspberry-as-gige-camera.git
$ cd raspberry-as-gige-camera/code/server
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

### Running the `rpiasgige` server on Raspberry Pi

After building `rpiasgige`, run the server as follows:

```
pi@raspberrypi:~/raspberry-as-gige-camera/build $ ./rpiasgige -port=4001 -device=/dev/video0
DEBUG - /dev/video0 - Not initialized. Initializing now.
DEBUG - /dev/video0 - successfuly initialized.
DEBUG - /dev/video0 - Waiting for client
```
Once the `rpiasgige` server is running, it is ready to respond to incoming TCP requests.

## Building & running the clients

There are four ways to access the `rpiasgige` server:

- Sending command-line requests using native SO utilities: check [the examples](https://github.com/doleron/raspberry-as-gige-camera/blob/main/docs/command-line-examples.MD).
- Using the provided client programs: check [the example](https://github.com/doleron/raspberry-as-gige-camera/tree/main/code/client).
- Using the provided client API's: check [the API](https://github.com/doleron/raspberry-as-gige-camera/tree/main/code/client).
- Writing your own remote calls using the [rpiasgige protocol](https://github.com/doleron/raspberry-as-gige-camera/blob/main/protocol.MD)

## Building and running the tests

`rpiasgige` is shipped with a set of unit tests. You can build and run it as follows:

```
$ cd raspberry-as-gige-camera/code/server/build
$ cmake -DBUILD_TESTS=ON ..
$ make
```

Obs.: cmake will automatically donwload [googletest](https://github.com/google/googletest) for you.

Once everything is build, run the tests just by:

```
$ ./test_rpiasgige 
```

## Limitations

According to [this](https://www.raspberrypi.org/documentation/computers/processors.html), the L2 shared cache of Raspberry PI 4 processor is set to 1MB whereas the same cache is constrained to only 512 KB in RPI 3 boards. This bottleneck eventually reduces the amount of traffic data/FPS sent/received.

Note that some RPI boards - such as model A and zero - don't have a built-in ethernet interface whereas some old models do not have even a wifi interface. For narrowed boards like this you can attach a USB-to-ethernet adapter as a very last alternative. But be aware to achieve a not so high data throughtput though.

That said, `rpiasgige` is a suitable alternative to retrieve frames at at most 150 fps for low resolutions such as 640x480 or less. Of course, your camera must support the speed and resolution as well.

## What `rpiasgige` not is

- `rpiasgige` is not a surveillance system, OS, library or so. Check [motionEyeOS](https://github.com/ccrisan/motioneyeos/wiki) if your are looking for a great surveillance tool for your Raspberry PI/home.

- `rpiasgige` is not a licensed gigE vision device. See *Disclaimer* below.

- `rpiasgige` is not an implementation of RTSP or NOVIF protocols.

## Disclaimer

This code is in its early stages yet. It is not battle-tested neither ready for production yet at all.

Note also that this repository does not follow the [gigE Vision](https://www.automate.org/a3-content/vision-standards-gige-vision) standard and it is not licensed as a gigE Vision device.

## Contribute

Contributions are very welcome! Check below for next steps and file an issue if you find something weird or wrong. PR's are super welcome as well!

## TODO: Next steps

- Improving protocol description
- ~~tests~~
- More tests
- Test a bit more, man!
- C'mon tests are always welcome! More tests doleron!
- Considering removing OpenCV dependency
- Resovle the cmaera path (for ex.: /dev/video2) by the USB bus address
- ~~Coding remote C++ API~~
- ~~Coding remote client example~~
- Supporting big-endian clients
- ~~Adding Python client API~~
- Adding JavaScript client API
- Adding Java client API
- Adding callback to decorate frame before send

