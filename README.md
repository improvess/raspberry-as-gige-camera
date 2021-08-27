# raspberry-as-gige-camera

Transform your USB camera in a gigE-like camera with Raspberry PI.

## TL;DR;

The code in this repository allows you to expose your USB camera as an ethernet device using Raspberry PI's gigabyte ethernet port.

![image](https://user-images.githubusercontent.com/9665358/130965792-e9bc97ef-f7de-4e65-ac04-72f85d3257f2.png)

## Why?

A real gigE camera is great but not cheap and in many situations not available on stock. In scenarios like this, you can use your a Raspberry Pi as an alternative to provide an ethernet interface for your USB camera.

## Examples of usage

Grabbing 320x240 images from a [Microsoft Lifecam Studio](https://www.microsoft.com/en-ww/accessories/products/webcams/lifecam-studio) at 30 fps.

![image](https://user-images.githubusercontent.com/9665358/130779743-b97e4d8d-5367-46c5-9202-b6bdd8eb7154.png)

Note that 30 fps is the max camera model frame rate.

Achieving roughly 60 fps @ 640x480 using a [Sony Playstation 3 Eye camera](https://en.wikipedia.org/wiki/PlayStation_Eye):

![image](https://user-images.githubusercontent.com/9665358/131152076-b6e7f95b-a7b9-400f-813a-d98e038efc9a.png)

## Accessing the camera remotely by code

`rpiasgige` has a C++ client API that allows to access and retrieve images from a camera remotely. Check out the example below:

```c++
#include <iostream>

#include "rpiasgige/client_api.hpp"

int main(int argc, char **argv)
{

    rpiasgige::client::Device camera("192.168.2.2", 4001, rpiasgige::client::HEADER_SIZE, 3 * 480 * 640 + rpiasgige::client::HEADER_SIZE);

    if (!camera.open())
    {
        std::cerr << "Failed to open the camera! Exiting ...\n";
        exit(0);
    }

    cv::Mat mat;
    bool success = camera.grab(mat);
    
    if (success) 
    {
        int image_size = mat.total() * mat.elemSize();
        std::cout << "The frame has " << image_size << " bytes!\n";
    } else {
        std::cerr << "Failed to grab the frame!\n";
    }
    
    camera.release();
    
    return 0;
}
```

Instructions how to build and run are shown below.

## Building

This repo uses [CMake](https://cmake.org/) and [OpenCV](https://opencv.org/) to build the code on a [Raspberry PI OS](https://www.raspberrypi.org/software/) or similar operating system.

```
$ git clone https://github.com/doleron/raspberry-as-gige-camera.git
$ cd raspberry-as-gige-camera/code/camera-server
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

## Running the `rpiasgige` on Raspberry Pi

After building `rpiasgige`, run the server as follows:

```
pi@raspberrypi:~/raspberry-as-gige-camera/build $ ./rpiasgige -port=4001 -device=/dev/video0
DEBUG - /dev/video0 - Not initialized. Initializing now.
DEBUG - /dev/video0 - successfuly initialized.
DEBUG - /dev/video0 - Waiting for client
```

## Getting images from the camera remotely

Once the `rpiasgige` server is running, it is ready to respond to incoming TCP requests. There are four ways to make reqeusts to the `rpiasgige` server:

- Sending command-line requests using native SO utilities: check [the examples](https://github.com/doleron/raspberry-as-gige-camera/blob/main/command-line-examples.MD).
- Using the provided client program: check [the example](https://github.com/doleron/raspberry-as-gige-camera/tree/main/code/client).
- Using the provided client API: check [the API](https://github.com/doleron/raspberry-as-gige-camera/blob/main/code/client/include/rpiasgige/client_api.hpp).
- Writing your own remote calls using the [rpiasgige protocol](https://github.com/doleron/raspberry-as-gige-camera/blob/main/protocol.MD)

### Building and running the tests

`rpiasgige` is shipped with a set of unit tests. You can build and run it as follows:

```
$ cd raspberry-as-gige-camera/code/camera-server/build
$ cmake -DBUILD_TESTS=ON ..
$ ./test_rpiasgige 
```

Obs.: cmake will automatically donwload googletest for you.

## Limitations

Raspberry PI boards are revolutionary - but general purpose - devices. They are not exaclty intended to perform intensive computing/IO tasks, having significative hardware limitations which must be taken in mind when considering using them to attend to extremme demands.

According to [this](https://www.raspberrypi.org/documentation/computers/processors.html), the L2 shared cache of Raspberry PI 4 processor is set to 1MB whereas the same cache is constrained to only 512 KB in RPI 3 boards. This bottleneck eventually reduces the amount of traffic data/FPS sent/received.

Note taht some RPI boards such as model A and zero don't have a built-in ethernet interface whereas some old models do not have even a wifi interface. For narrowed boards like this you can attach a USB-to-ethernet adapter as a very last alternative but be ready to achieve a not so high data throughtput.

That said, `rpiasgige` is a suitable alternative to retrieve frames at 30-60 fps at low resolutions such as 640x480 or less. If your application requires a high resolution at 80 fps or more, I would recommeded to try an actual gigE camera or even another board with a larger L2 cache.

## Disclaimer

This code is in its early stages. It is not battle-tested neither ready for production yet at all.

## TODO: Next steps

- Improving protocol description
- ~~tests~~
- More tests
- Test a bit more, man!
- C'mon tests are always welcome! More tests doleron!
- Removing OpenCV dependence
- Resovle the cmaera paht (for ex.: /dev/video2) by the USB bus address
- ~~Coding remote C++ API~~
- ~~Coding remote client example~~
- Supporting big-endian clients
- Adding Python client API
- Adding JavaScript client API
- Adding Java client API
- Adding callback to decorate frame before send

