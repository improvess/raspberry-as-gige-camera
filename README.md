# raspberry-as-gige-camera

Transform your USB camera in a gigE-like camera with Raspberry PI.

## TL;DR;

The code in this repository allows you to expose your USB camera as an ethernet device using Raspberry PI's gigabyte ethernet port. In other words, you can access your remote USB camera just like you do with a local USB camera.

<p align="center" width="640">
  <img src="https://user-images.githubusercontent.com/9665358/130965792-e9bc97ef-f7de-4e65-ac04-72f85d3257f2.png">
</p>

```c++
#include "rpiasgige/client_api.hpp"

int main(int argc, char **argv) {

    rpiasgige::client::Device camera("192.168.2.2", 4001);

    camera.open();

    cv::Mat mat;
    camera.retrieve(mat);

    cv::imshow(mat);
    
    camera.release();
    
    return 0;
}
```

## Why?

A real [gigE](https://www.automate.org/a3-content/vision-standards-gige-vision) camera is great but not cheap and in many situations cameras like this aren't available on stock. In scenarios like this, you can use your Raspberry Pi as an alternative to provide an ethernet interface for your USB cameras.

## Examples of usage

Grabbing images at 100-150 fps with resolution 320x240 from a [Sony Playstation 3 Eye camera](https://en.wikipedia.org/wiki/PlayStation_Eye):

![image](https://user-images.githubusercontent.com/9665358/131229615-f0a73265-755d-4572-8946-17fb75ca8675.png)

Achieving 14-19 fps at 1280x720 using a [Microsoft Lifecam Studio](https://www.microsoft.com/en-ww/accessories/products/webcams/lifecam-studio).

![image](https://user-images.githubusercontent.com/9665358/131230242-ea0ed8ed-9590-42cd-8247-5f0094396bc0.png)

Instructions how to build and run are shown below.

## Building

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
$ cd raspberry-as-gige-camera/code/server/build
$ cmake -DBUILD_TESTS=ON ..
$ ./test_rpiasgige 
```

Obs.: cmake will automatically donwload googletest for you.

## Limitations

According to [this](https://www.raspberrypi.org/documentation/computers/processors.html), the L2 shared cache of Raspberry PI 4 processor is set to 1MB whereas the same cache is constrained to only 512 KB in RPI 3 boards. This bottleneck eventually reduces the amount of traffic data/FPS sent/received.

Note that some RPI boards - such as model A and zero - don't have a built-in ethernet interface whereas some old models do not have even a wifi interface. For narrowed boards like this you can attach a USB-to-ethernet adapter as a very last alternative. But be aware to achieve a not so high data throughtput though.

That said, `rpiasgige` is a suitable alternative to retrieve frames at at most 150 fps for low resolutions such as 640x480 or less. Of course, your camera must support the speed and resolution as well.

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

