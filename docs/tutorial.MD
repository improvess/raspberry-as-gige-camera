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

Follow the Raspberry Pi's official getting started tutorial: https://projects.raspberrypi.org/en/projects/raspberry-pi-getting-started if you are **totally new** to Raspberry PI, 

Otherwise, if you are familiar already with Raspberry Pi already, skip to the next section!

## Step 3 - Install dependences

Open terminal and execute the following commands:

```
sudo apt update
sudo apt upgrade
sudo apt install git build-essential cmake binutils-dev libopencv-dev python3-opencv 
```
Note that the OpenCV version from libopencv-dev & python3-opencv is quite old (3.2). Check [here](https://www.pyimagesearch.com/2018/09/26/install-opencv-4-on-your-raspberry-pi/), [here](https://www.jeremymorgan.com/tutorials/raspberry-pi/how-to-install-opencv-raspberry-pi/) or [here](https://learnopencv.com/install-opencv-4-on-raspberry-pi/) if you prefer to install the most recent version of OpenCV instead.

## Step 4 - Clone & build `rpiasgige` server

After install the dependences, execute the following commands to clone and build the `rpiasgige` server:

```
git clone https://github.com/doleron/raspberry-as-gige-camera.git
cd raspberry-as-gige-camera/code/server
mkdir build
cd build
cmake ..
make
```

## Step 5 - Run `rpiasgige` server

Connect your USB camera to one of Raspiberry Pi' USB ports. After it, run the `rpiasgige` server by executing:

```
./rpiasgige
```
The expected output is:

```
DEBUG - /dev/video0 - Not initialized. Initializing now.
DEBUG - /dev/video0 - successfuly initialized.
DEBUG - /dev/video0 - Waiting for client
```
Note: You can stop the `rpiasgige` server by pressing control+c

By default, `rpiasgige` uses the device `/dev/video0` and TCP port `4001`. Of course, you can use different settings, for example:

```
./rpiasgige -port=5753 -device=/dev/video2
```

It is important to take attention to which port you have choosen for your server. Client programs need this information to make TCP requests accurately.

Once the server is running, it is ready to reply incoming requests.

## Step 6 - (Optional) Set static IP for the ethernet interface

It is likely that you end up implementing a static IP for your camera. You can do it by setting the a static IP in the device itself or setting up a rule on your DHCP server. Here, we assume that you set the eth0 interface with IP 192.168.2.2 and network mask 255.255.255.0:

<p align="center">
  <img  width="300" src="https://user-images.githubusercontent.com/9665358/131861401-0bf7249c-a2ad-489b-8fff-4c128654e87b.png">
</p>

## Step 7 - Running clients

At this point, we have the server up and running. You can make TCP requests to it in different ways. More commonly, you will prefer to write your own clients to connect to the camera remotely. Check out the following client examples below:

- Running the [C++ client](https://github.com/doleron/raspberry-as-gige-camera/blob/main/docs/cpp_client_tutorial.md) example
- Running the [Python 3 client](https://github.com/doleron/raspberry-as-gige-camera/blob/main/docs/python3_client_tutorial.md) example
