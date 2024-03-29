# Sending command-line requests to the camera

Sending command-line requests is a convenient way to check if your camera is running & communicating as expected.

| Example of sending an OPEN command via command-line  | 
| ------------- | 
| ![image](https://user-images.githubusercontent.com/9665358/132090846-678ec0f4-511b-4a27-b67c-5669cb623568.png)  |




Here you can find a handly set of command examples. It is noteworthly that:

- the examples consider that RPI is set to use 192.168.2.3 as IP address. Replace it with the proper Raspberry PI ethernet IP.
- note that the examples assumes that you are running them on a remote linux machine.
- the examples use Netcat  linux utility called by the `nc` linux command. You can adapt them to use other clients of your preference.

## Opening the device

Usually the first thing to do is opening the device. This can be done in a similar form like opening a local USB camera before retrieving data:

```
$ echo -e "OPEN0\x0\x0\x0\x0" | nc 192.168.2.3 4001
```

## Setting resolution width to 320

```
$ echo -e "SET00\xC\x0\x0\x0\x3\x0\x0\x0\x0\x0\x0\x0\x0\x0\x74\x40" | nc 192.168.2.3 4001
```

## Getting the frame resolution width

```
$ echo -e "GET00\x4\x0\x0\x0\x3\x0\x0\x0" | nc 192.168.2.3 4001
```

## Setting the resolution height to 240

```
$ echo -e "SET00\xC\x0\x0\x0\x4\x0\x0\x0\x0\x0\x0\x0\x0\x0\x6E\x40" | nc 192.168.2.3 4001
```

## Setting the size of camera buffer to 2

```
$ echo -e "SET00\xC\x0\x0\x0\x26\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x40" | nc 192.168.2.3 4001
```

## Checking if the device is opened already

```
$ echo -e "ISOP0\x0\x0\x0\x0" | nc 192.168.2.3 4001
```

## Setting the autofocus ON

Note that the autofocus feature may be unavailable for the model of the USB camera in use.

```
$ echo -e "SET00\xC\x0\x0\x0\x27\x0\x0\x0\x0\x0\x0\x0\x0\x0\xF0\x3F" | nc 192.168.2.3 4001
```

## Setting autofocus OFF

```
$ echo -e "SET00\xC\x0\x0\x0\x27\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0" | nc 192.168.2.3 4001
```

## Setting focus to 0

```
$ echo -e "SET00\xC\x0\x0\x0\x1C\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0" | nc 192.168.2.3 4001
```

## Asking camera to run at 60 fps

```
$ echo -e "SET00\xC\x0\x0\x0\x5\x0\x0\x0\x0\x0\x0\x0\x0\x0\x4E\x40" | nc 192.168.2.3 4001
```

## Asking camera to run at 120 fps

```
$ echo -e "SET00\xC\x0\x0\x0\x5\x0\x0\x0\x0\x0\x0\x0\x0\x0\x5E\x40" | nc 192.168.2.3 4001
```

## Closing the device

```
$ echo -e "CLOS0\x0\x0\x0\x0" | nc 192.168.2.3 4001
```

## Setting fourcc to MJPG

```
$ echo -e "SET00\xC\x0\x0\x0\x6\x0\x0\x0\x0\x0\x40\x93\x12\xD4\xD1\x41" | nc 192.168.2.3 4001
```
