# Accessing `rpiasgige` camera from a C++ client

This tutorial shows how to remotely access a camera running `rpiasgige` using Python 3.

## TL;DR;

1. Connect your Raspberry PI and computer in the same ethernet/IP network
2. Starts the `rpiasgige` server process on Rasoberry Pi
3. execute the [basic C++ example](https://github.com/doleron/raspberry-as-gige-camera/blob/main/code/client/cpp_api/examples/basic.cpp)

These steps are explained in details below.

## Roadmap

- [Step 1 - Pre-requisites](https://github.com/doleron/raspberry-as-gige-camera/blob/main/docs/cpp_client_tutorial.md#step-1---pre-requisites)
- [Step 2 - Connecting computer and Raspberry Pi](https://github.com/doleron/raspberry-as-gige-camera/blob/main/docs/cpp_client_tutorial.md#step-2---connecting-computer-and-raspberry-pi)
- [Step 3 - Coding the first `rpiasgige` client with C++](https://github.com/doleron/raspberry-as-gige-camera/blob/main/docs/cpp_client_tutorial.md#step-3---coding-the-first-rpiasgige-client-with-c)
- [Step 4 - Running the C++ client example](https://github.com/doleron/raspberry-as-gige-camera/blob/main/docs/cpp_client_tutorial.md#step-4---running-the-c-client-example)
- [Step 5 - Troubleshotting](https://github.com/doleron/raspberry-as-gige-camera/blob/main/docs/cpp_client_tutorial.md#step-5---troubleshotting)

## Step 1 - Pre-requisites

In order to following this tutorial, you need to previously setup a `rpiasgige` server as shown in [Step-by-step tutorial](https://github.com/doleron/raspberry-as-gige-camera/blob/main/docs/tutorial.MD).

Once the server is running, you can follow this tutorial to run a C++ client on another machine. This tutorial assumes that the remote machine where the client runs is also a linux machine having OpenCV and a C++ 11 compliant compiler installed. Check [the official OpenCV documentation](https://docs.opencv.org/4.5.1/d2/de6/tutorial_py_setup_in_ubuntu.html) to learn how to install OpenCV on linux. This tutorial uses GCC 10 as C++ compiler. Other compilers such as clang should works as well.

| Physical setup |
| -------------- |
| ![image](https://user-images.githubusercontent.com/9665358/132226000-60041ff4-3b6d-439e-8206-e36d5de4475a.png) |



## Step 2 - Connecting computer and Raspberry Pi

In order to follow this tutorial, you need to connect your RPI to the same network where the PC/notebook is connected. The easiest way to do that is just by using an ethernet cable. Of course, you can also connect the RPI to your local ethernet switch or router. It is up to you.

> Although you can use your wifi network to connect to the `rpiasgige` server, it is recommeded using a cable connection such as an ethernet cable.

Once the two hosts are physically connected and set up with the proper network settings, the first thing to do is checking up if they can actually can communicate one to each other. The straight-forward is pinging the raspberry pi from the computer. Assuming that your RPI ethernet IP board is set to IP `192.168.2.3`:

```
ping 192.168.2.3
```

| Pinging your PI |
| --------------- |
| ![image](https://user-images.githubusercontent.com/9665358/132093049-8fcee985-01b2-49be-936d-74dacda62837.png) |

> Note that your computer's ethernet port must be also set with an IP network (in this example, an address like `192.168.2.X` like `192.168.2.10`, `192.168.2.51`, etc...). The proper way to set your ethernet IP address depends on the type of linux distro you are using.

> check official Raspberry PI documentation to learn how to proper set Raspberry PI ethernet IP statically or even [this tutorial on sparkfun](https://learn.sparkfun.com/tutorials/headless-raspberry-pi-setup/ethernet-with-static-ip-address)

Once you have confirmed that your machine is able to ping raspberry pi, it is time to talk to the `rpiasgige` server. Let's start off by sending a `rpiasgiger`-ping as shown below:

```
echo -e "PING0\x0\x0\x0\x0" | nc 192.168.2.3 4001
```

| Pinging `rpiasgige` |
| --------------- |
| ![image](https://user-images.githubusercontent.com/9665358/132098616-00075310-f52a-4c53-9b70-a89936b4354a.png) |

The `PONG0` response indicates that the `rpiasgige` server is listening and replying properly.

> If the server doesn't reply, check if the `rpiasgige` server process is actually running on raspberry pi as shown [here](https://github.com/doleron/raspberry-as-gige-camera/blob/main/docs/tutorial.MD#step-5---run-rpiasgige-server).



## Step 3 - Coding the first `rpiasgige` client with C++

Sending command-line requests to `rpiasgige` can be handy in some cases. You can find more [command-line examples here](https://github.com/doleron/raspberry-as-gige-camera/blob/main/docs/command-line-examples.MD). However, as software developers, we always prefer to use API's to make our calls. Let's see how to use C++ to do it. Execute the following commands on the client computer:

```
git clone https://github.com/doleron/raspberry-as-gige-camera.git
cd raspberry-as-gige-camera/code/client/cpp_api/
nano test.cpp
```

![image](https://user-images.githubusercontent.com/9665358/132158768-dfaf8a7e-0391-4307-982e-cc6d97bc9d5a.png)

Nano is a text editor found in many linux distros. Once nano is opened, copy-paste into the following code:

```c++
using namespace rpiasgige::client;

int main(int argc, char **argv) {

    Device camera("192.168.2.3", 4001);

    if(camera.ping()) {
        std::cout << "rpiasgige successfully replied!\";
    } else {
        std::cerr << "Ops! Camera didn't reply. Exiting...\n";
    }

    return 0;
}

```
Press control+x to close nano e confirm save the file. Compile it with the following command:

```
g++ -O3 test.cpp src/lib/client_api.cpp -I include -o test `pkg-config --cflags --libs opencv4`
```

Finally, run `./test`:

![image](https://user-images.githubusercontent.com/9665358/132158551-c41dc310-ab36-4982-8dd9-9fce28a763f0.png)

If everything is good, `rpiasgige` replied as shown above. Obviously, it was a basic connection check-up.



## Step 4 - Running the C++ client example

You can find a more motivating example in the source code: [raspberry-as-gige-camera/blob/main/code/client/cpp_api/examples/basic.cp](https://github.com/doleron/raspberry-as-gige-camera/blob/main/code/client/cpp_api/examples/basic.cpp)

which can be compiled by:

```
g++ examples/basic.cpp src/lib/client_api.cpp -I include -o basic `pkg-config --cflags --libs opencv4`
```
and run by:
```
./basic
```

This example open and grabs frames from the remote camera. The expected result is such as:

![image](https://user-images.githubusercontent.com/9665358/132161085-60a8aa49-da92-412d-b0f3-3ad06cf00ad6.png)

Note that the example tries to connect to a remote camera at `192.168.2.3` and port `4001`:

```c++
const std::string address = "192.168.2.3";
const int port = 4001;

Device camera(address, port);
```

> Change the file `examples/basic.cpp` if you are using different network settings.

The code sets the camera parameters as follows:

```c++
const double frame_width = 640;
const double frame_height = 480;
const double mjpg = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
const double fps = 30;

camera.set(cv::CAP_PROP_FRAME_WIDTH, frame_width, keep_alive);
camera.set(cv::CAP_PROP_FRAME_HEIGHT, frame_height, keep_alive);
camera.set(cv::CAP_PROP_FOURCC, mjpg, keep_alive);
camera.set(cv::CAP_PROP_FPS, fps, keep_alive);
```

I'm using a Microsoft Lifecam Studio camera which supports these settings properly. A camara without support for the settings is only one of several problems one can find when trying to connect to the remote camera. The following section talks about it.



## Step 5 - Troubleshotting

Roughtly speaking, there are threee types of problems you can face on when attempting to connect to you remote camera: 
- connection issues between the client computer and the raspberry host
- connection issues between the raspberry pi and the camera
- wrong parameters / settings

### Network connection issues

An error like this:

![image](https://user-images.githubusercontent.com/9665358/132217044-fda9c48f-da55-48d4-8152-ac289985f94d.png)

indicates that there is some issue to client talk to the `rpiasgige` server (supposedly) running on the Raspberry Pi. There are two probable causes:

- **The two hosts are not connected at all**: we cannot cover here everything you can do to debug your network. Maybe it is just an unconnected cable or maybe you need to fix a network address setup. Usually, the `ping` utility - available in every operating system - is the first step to help you on this.

<table>
    <tr>
        <td>Raspberry PI's ethernet port has two LEDs to indicating TX & RT communication. It is not good news if one or two of these LEDs are not blinking.</td>
        <td><img src="https://user-images.githubusercontent.com/9665358/132116790-4a9423c7-13e2-4923-a734-f3e0988ca927.png"></td>
    </tr>
</table>

- **The `rpiasgige` process is not running**

If the two hosts are actually connected but the camera keeps not replying, it is because the `rpiasgige` server process is not running on Raspberry Pi. Check the [Step-by-step tutorial](https://github.com/doleron/raspberry-as-gige-camera/blob/main/docs/tutorial.MD) how to build and run the `rpiasgige` server:

| Running `rpiasgige` server on non-default port |
| --------------------------------------------------------------- |
| ![image](https://user-images.githubusercontent.com/9665358/132128455-cba3319e-884b-4850-ad88-9d2697292303.png) |

If the process is running, double check if **both server and client TCP ports match**. Note that the `rpiasgige` server can be running in a different port than the default (4001) as in the example above. This port value must matching to the port defined in the client program:

```c++
const std::string address = "192.168.2.3";
const int port = 4001;

Device camera(address, port);
```
### Local camera connection issues

If the local USB or CSI camera is not properly connected/recognized by Raspberry Pi, the client will reply with `Failed to open the camera` as shown below:

![image](https://user-images.githubusercontent.com/9665358/132216270-b6af5999-f35f-4782-977a-26b2454b3161.png)

There are some reasons for this problem:

- an unconnected camera: the USB or CSI connection is not properly plugged to Raspberry Pi's interfaces
- absense of OS support (driver) for the camera: not every camera has a linux driver. If the OS doesn't recognize the camera nothing many things can happen. Check [here](https://elinux.org/RPi_USB_Webcams) to a parcially complete list of supported cameras by Raspberry Pi.
- Wrong path parameter: by default, the `rpiasgige` server connects to the camera at '/dev/video0'. If the camera is not associated to this path, the communication will not happen
- a broken camera: if it is broken, the only way is to replace it by another camera

The proper debugging procedure varies depending on which device camera model you have attached to your RPI. One very first approach is listing the current recognized device list:
```
ls /dev/video*
```
By running this command right now on my RPI, I can see that there is no a `/dev/video0` device in the list:

![image](https://user-images.githubusercontent.com/9665358/132127231-a1bc9835-37d2-429e-8bd8-dc3784b68ac6.png)

This is because the camera USB plug is actually unconnected. Let me plug it and check again:
![image](https://user-images.githubusercontent.com/9665358/132127350-0cd68cba-a488-48a0-8548-9909c0e3817a.png)

Great, now `/dev/video0` is in the list.

> The devices `/dev/video10`-`/dev/video16` are not actually physical devices. They are fake devices created by the V4L2 library.

In this case, I'm using a USB camera. Therefore, checking for malconnection issues is relatively easy. For a CSI camera, it requires a little bit of more effort though. The CSI cable can be invenrted or slight slipping. The camera module cannot be enabled or maybe your video memory setting is slow. Cover the correct connection of each camera type is besides the objective of this tutorial. 

> Check the [official tutorial](https://projects.raspberrypi.org/en/projects/getting-started-with-picamera) to learn how to connect your CSI camera to Raspberry Pi.

Note that `/dev/video0` is not the only path your camera can be represented by the OS. Raspbian can also set it as `/dev/video1`, `/dev/video2`, etc... Indeed, the way as devices are converted to paths is annoying. I wrote a simple C++ header handy to convert static USB bus address in device paths: https://github.com/doleron/v4l2-list-devices . This repo is not the only one solution. You can also check it by using utilities such as `usb-devices` or a simple `ls`:

![image](https://user-images.githubusercontent.com/9665358/132128264-512ea3f8-931b-4923-8bfe-675b3d162b4c.png)

Once you know what is the device path of you camera, you can set up it in the `rpiasgige` server initialization:

![image](https://user-images.githubusercontent.com/9665358/132128373-64b0e905-c96a-4dea-aa3e-be96f0f79a9e.png)

If your camera is properly plugged/connected but keeps not showing on `/dev/video*` one possibility is a broken and returning to the supplier can be the only way to do.

### Failed to set some camera parameter

The example source uses a set of camera configurations:
```c++
const double frame_width = 640;
const double frame_height = 480;
const double mjpg = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
const double fps = 30;
```
These configuration may not be valid for every type of camera, in particular, the type of camera you have at hand. The easiest way to know the valid camera configurations allowed by your device is using the utility command `v4l2-ctl -d 0 --list-formats-ext`:

![image](https://user-images.githubusercontent.com/9665358/132128890-4fd96ecc-e190-4b6f-a1f9-fff3a506c3b7.png)

Scrolling down, I can find that this configuration is actually supported by my camera:

![image](https://user-images.githubusercontent.com/9665358/132128917-02456f4c-42be-4280-9bd7-51f8858ec19e.png)

> `v4l2-ctl` is not part of core Raspberry Pi OS. You can install it by `sudo apt-get install v4l-utils`

Note that the example throws an error message if I try an invalid configuration:

![image](https://user-images.githubusercontent.com/9665358/132222511-06bc6d37-8da0-429d-90db-c30896f73bfc.png)

### Using test application to check up if the camera is working

`rpiasfige` has an OpenCV test application handy to check up if your camera is actually recognized by the Raspberry OS. To use it, run on your server rasp:

```
./simple_opencv_app -frame-width=800 -frame-height=448
```

This is a simple OpenCV grabbing-*n*-viewer application. So, if everything is good, your camera should show up as the example below:

![image](https://user-images.githubusercontent.com/9665358/132133895-5b9c8a3b-5229-41c3-a780-2cd5d24e01e7.png)

More details and parameters can be seen in the [source](https://github.com/doleron/raspberry-as-gige-camera/blob/main/code/server/extras/simple_opencv_cap.cpp)

### Timeouts

Before finishing this long troubleshooting section, let's talk about camera timeouts.

Some cameras respond really fast to OPEN commands. But it is not the general case.  If you get timeouts errors like these:

![image](https://user-images.githubusercontent.com/9665358/132221683-cc4aa365-b623-4c36-a2b9-46014e0e0ecf.png)

it is likely that you have a lazy-to-open camera at hand. The timeout, i.e, the max amount of time the client can wait to the response, is a decision of the client. By default, the python client API uses a timeout of 1 sec. You can change this value by calling:

```c++
# settting read timeout to 10 seconds
camera.set_read_timeout(10);
```
at any moment before try to open the camera.

### Other issues and contributions

Feel free to [file an issue](https://github.com/doleron/raspberry-as-gige-camera/issues) if you find any other problem than the ones listed here. Contributting by sending [pull requests](https://github.com/doleron/raspberry-as-gige-camera/pulls) is also super welcome!
