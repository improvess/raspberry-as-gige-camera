# Step-by-step tutorial

Here we cover the steps to get `rpiasgige` running on Raspberry Pi

## Step 1 - Check the required material list

You need the following list of material to run tutorial:

<table border="0">
  <tr>
    <td>
      <ul>
        <li>1 Raspberry Pi 3 or 4</li>
        <li>1 SD Card class 10 with 16 GB or more</li>
        <li>1 SD Card adapter</li>
        <li>1 USB Camera or Webcam</li>
        <li>1 Power Source Unit</li>
        <li>1 Computer or Notebook with internet access</li>
        <li>1 ethernet cable</li>
      </ul>
    </td>
    <td valign="center"><img src="https://user-images.githubusercontent.com/9665358/131837147-8a8cbe5c-6073-40c5-b51d-68689479675c.png"></td>
  </tr>
</table>

## Step 2 - Getting started with Raspberry Pi

Follow the Raspberry Pi's official Getting Started tutorial: https://projects.raspberrypi.org/en/projects/raspberry-pi-getting-started only if you are **totally new** to Raspberry PI. 

Otherwise, skip to the next section!

## Step 3 - Installing dependencies

Open the Terminal and execute the following commands:

```
sudo apt update
sudo apt upgrade
sudo apt install git build-essential cmake binutils-dev libopencv-dev python3-opencv 
```

<table border="0">
  <tr>
    <td align="center">
      Installing dependencies
    </td>
  </tr>
  <tr>
    <td align="center"><img src="https://user-images.githubusercontent.com/9665358/132012434-1ace0ad2-44ce-4d74-9df0-e4fbda1de627.png"></td>
  </tr>
</table>

This last command may take longer to finish. It will download and install OpenCV 3.2 and some required tools used afterwards in the build of `rpiasgige` itself. 

> The OpenCV version installed from libopencv-dev is quite old (3.2). Check [here](https://www.pyimagesearch.com/2018/09/26/install-opencv-4-on-your-raspberry-pi/), [here](https://www.jeremymorgan.com/tutorials/raspberry-pi/how-to-install-opencv-raspberry-pi/) or [here](https://learnopencv.com/install-opencv-4-on-raspberry-pi/) if you prefer to install the most recent version of OpenCV instead.

## Step 4 - Clone & build `rpiasgige` server

Once the installation of dependencies is finished, execute the following commands to clone and build the `rpiasgige` server:

```
git clone https://github.com/doleron/raspberry-as-gige-camera.git
cd raspberry-as-gige-camera/code/server
mkdir build
cd build
cmake ..
make
```
<table border="0">
  <tr>
    <td align="center">
      Building <code>rpiasgige</code>
    </td>
  </tr>
  <tr>
    <td align="center"><img src="https://user-images.githubusercontent.com/9665358/132019557-2ce4e1eb-3d05-46a2-88a5-d0987065ab44.png"></td>
  </tr>
</table>

## Step 5 - Run `rpiasgige` server

Connect your USB camera to one of Raspiberry Pi's USB ports. After it, run the `rpiasgige` server as follows:

```
./rpiasgige
```
The expected output is:

```
DEBUG - /dev/video0 - Not initialized. Initializing now.
DEBUG - /dev/video0 - successfuly initialized.
DEBUG - /dev/video0 - Waiting for client
```
> Note: You can stop the `rpiasgige` server by pressing control+c

By default, `rpiasgige` uses the device `/dev/video0` and TCP port `4001`. Of course, you can use different settings. For example:

```
./rpiasgige -port=5753 -device=/dev/video2
```

starts the `rpiasgige` server on port `5753` using the device `/dev/video2`

> It is important to take attention tothe choice of TCP port. The correct TCP port is mandatory in order to client programs to make TCP requests.

Once the server is running, it is ready to reply incoming requests.

## Step 6 - (Optional) Set static IP for the ethernet interface

It is likely that you end up defining a static IP for your camera. You can achieve it by setting a static IP in the device itself or setting up a rule on your DHCP server. Here, we assume that you have set the eth0 interface with IP 192.168.2.2 and network mask 255.255.255.0:

<p align="center">
  <img  width="300" src="https://user-images.githubusercontent.com/9665358/131861401-0bf7249c-a2ad-489b-8fff-4c128654e87b.png">
</p>

## Step 7 - Running clients

At this point, we have the server up and running. You can make TCP requests to it in different ways. More commonly, you will prefer to write your own clients to connect to the camera remotely. Check out the following client examples below:

- Running the [C++ client](https://github.com/doleron/raspberry-as-gige-camera/blob/main/docs/cpp_client_tutorial.md) example
- Running the [Python 3 client](https://github.com/doleron/raspberry-as-gige-camera/blob/main/docs/python3_client_tutorial.md) example
