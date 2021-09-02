# Step-by-step tutorial

# Status 

Sorry it isn't done yet!

## Step 1 - Check the required material list

To run this tutorial, you will need of:


<table border="0">
  <tr>
    <td>
      <ul>
        <li>1 Raspberry Pi 3 or 4</li>
        <li>1 SD Card class 10 with 16 GB or more</li>
        <li>1 SD Card adapter</li>
        <li>1 USB Camera</li>
        <li>1 Power Source Unit</li>
        <li>1 Computer or Notebook with internet access</li>
        <li>1 ethernet cable</li>
      </ul>
    </td>
    <td valign="center"><img src="https://user-images.githubusercontent.com/9665358/131837147-8a8cbe5c-6073-40c5-b51d-68689479675c.png"></td>
  </tr>
 </table>

## Step 2 - Getting started with Raspberry Pi

Follow the Raspberry Pi's getting started official tutorial: https://projects.raspberrypi.org/en/projects/raspberry-pi-getting-started if you are **tottaly new** to Raspberry PI, 

Otherwise, move to the next section!

## Step 3 - Install dependences

Open terminal and execute the following commands:

```
sudo apt update
sudo apt upgrade
sudo apt install python3-opencv git build-essential cmake binutils-dev
```

## Step 4 - Clone & build `rpiasgige` server

After install dependences, execute the following commands to build `rpiasgige` server:

```
pi@raspberrypi:~ $ git clone https://github.com/doleron/raspberry-as-gige-camera.git
cd raspberry-as-gige-camera/code/server
mkdir build
cd build
cmake ..
make -j4
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

Note that by default, `rpiasgige` uses the device /dev/video0 and TCP port 4001. You can use different settings, for example:

```
./rpiasgige -port=5753 -device=/dev/video2
```

It is important to take attention to which port you have choosen for your server. Client programs need this information to make TCP requests accurately.

Once the server is running, it is ready to get incoming requests and reply according.

## Step 6 - Running clients

At this point, we have the server up and running. You can make TCP requests to it in different ways. More commonly, you will prefer to write your own clients to connect to the camera remotely. Check out the following client examples below:

- Running the C++ client example
- Running the Python client example
