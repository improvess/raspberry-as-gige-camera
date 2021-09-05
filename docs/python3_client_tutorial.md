# Accessing `rpiasgige` camera from a Python 3 client

This tutorial shows how to remotely access a camera running `rpiasgige` using Python 3.

## Step 1 - Pre-requisites

In order to following this tutorial, you need to previously setup a `rpiasgige` server as shown in [Step-by-step tutorial](https://github.com/doleron/raspberry-as-gige-camera/blob/main/docs/tutorial.MD).

Once the server is running, you can follow this tutorial to run a Pythn 3 client on another machine. This tutorial assumes that the remote machine where the client runs is also a linux machine having OpenCV and Python 3 installed. Check [the official OpenCV documentation](https://docs.opencv.org/4.5.1/d2/de6/tutorial_py_setup_in_ubuntu.html) to learn how to install OpenCV on linux.

| Physical setup |
| -------------- |
| ![image](https://user-images.githubusercontent.com/9665358/132092835-a558e28c-7f03-47cd-951c-5a8969b183aa.png) |

## Connecting computer and Raspberry Pi

In order to follow this tutorial , you need to connect your RPI to the same network where the PC/notebook is connected. The easiest way to do that is just by using an ethernet cable. Of course, you can also connect the RPI to your local ethernet switch or router. It is up to you.

> Although you can use your wifi network to connect to the `rpiasgige` server, it is recommeded using a cable connection such as an ethernet cable.

The first thing to do is checking up if your computer actually can communicate to the Raspberry Pi. The straight-forward way to check it is pinging the raspberry pi from the computer. Assuming that your RPI ethernet IP board is set to IP `192.168.2.3`:

```
ping 192.168.2.3
```

| Pinging your PI |
| --------------- |
| ![image](https://user-images.githubusercontent.com/9665358/132093049-8fcee985-01b2-49be-936d-74dacda62837.png) |

> Note that your computer's ethernet port must be also set with an IP network (in this example, an address like `192.168.2.X` like `192.168.2.10`, `192.168.2.51`, etc...). The proper way to set your ethernet IP address depends on the type of linux distro you are using.

> check official Raspberry PI documentation to learn how to proper set Raspberry PI ethernet IP statically or even [this tutorial on sparkfun](https://learn.sparkfun.com/tutorials/headless-raspberry-pi-setup/ethernet-with-static-ip-address)

Once you have confirmed that your machine is able to ping raspberry pi, it is time to talk to the `rpiasgige` server. Let's start off by sending a `rpiasgige`-ping as shown below:

```
echo -e "PING0\x0\x0\x0\x0" | nc 192.168.2.3 4001
```

| Pinging `rpiasgige` |
| --------------- |
| ![image](https://user-images.githubusercontent.com/9665358/132098616-00075310-f52a-4c53-9b70-a89936b4354a.png) |

The `PONG0` response indicates that the `rpiasgige` server is listening and replying properly.

> If the server doesn't reply, check if the `rpiasgige` server process is actually running on raspberry pi as shown [here](https://github.com/doleron/raspberry-as-gige-camera/blob/main/docs/tutorial.MD#step-5---run-rpiasgige-server).

Sending command-line requests to `rpiasgige` can be handy in some cases. You can find more command-line examples here. However, as software developers we always prefer to use proper API's. Let's see how to use Python to do it. Execute the following commands on the client computer:

```
git clone https://github.com/doleron/raspberry-as-gige-camera.git
cd raspberry-as-gige-camera/code/client/python_api/src/
nano test.py
```

![image](https://user-images.githubusercontent.com/9665358/132099438-f6160498-92de-41da-9e60-5a9b2dd41f0d.png)

Nano is a text editor found in many linux distros. Once nano is opened, copy-paste into the following code:

```python3
from rpiasgige.client_api import Device

camera = Device("192.168.2.3", 4001)

if not camera.ping():
    print("Ops! Camera didn't reply. Exiting...", file=sys.stderr)
else:
    print("rpiasgige successfully replied!")
```
Press control+x to close nano e confirm save the file. Execute the file with:

```
python3 test.py
```

![image](https://user-images.githubusercontent.com/9665358/132099505-43fde103-7f5e-4dfb-9640-6e508715c6ca.png)

If everything is good, `rpiasgige` replied as shown above. Obviously, it was a basic connection check-up.

You can find a more interesting example in the source code: raspberry-as-gige-camera/blob/main/code/client/python_api/src/examples/basic.py

which can be run by:

```
python3 examples/basic.py
```
This example open grabs frames from the remote camera. The expected result is:



