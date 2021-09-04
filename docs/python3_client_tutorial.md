# Accessing `rpiasgige` camera from a Python 3 client

This tutorial shows how to remotely access a camera running `rpiasgige` using Python 3.

## Step 1 - Pre-requisites

In order to following this tutorial, you need previously setup a `rpiasgige` server as shown in the [Step-by-step tutorial](https://github.com/doleron/raspberry-as-gige-camera/blob/main/docs/tutorial.MD).

Once the server is running, you can follow this tutorial to run a Pythn 3 client on another machine. This tutorial assumes that the remote machine where the client runs is also linux and have OpenCV and Python 3 installed. Check [the official OpenCV documentation](https://docs.opencv.org/4.5.1/d2/de6/tutorial_py_setup_in_ubuntu.html) to learn how to install OpenCV on linux.

| Physical setup |
| -------------- |
| ![image](https://user-images.githubusercontent.com/9665358/132092835-a558e28c-7f03-47cd-951c-5a8969b183aa.png) |

## Connecting to the camera

Connect your RPI to the same network where the PC/notebook is connected. The easier way to do that is just using an ethernet cable to connect them directly. You can also connect the RPI to your local ethernet switch or router. It is up to you.

> Althought you can connect to `rpiasgige` server via wifi network, it is recommeded to use a cable connection such as ethernet.

The first thing to do is checking up if your computer actually can communicate to the Raspberry Pi.

Let's assume that your RPI ethernet IP board is set to use IP 192.168.2.3. Thus, the straight-forward way to check if the computer can access the PI is ping the raspberry pi from the computer:

```
ping 192.168.2.3
```

| Pinging your PI |
| -------------- |
| ![image](https://user-images.githubusercontent.com/9665358/132093049-8fcee985-01b2-49be-936d-74dacda62837.png) |
