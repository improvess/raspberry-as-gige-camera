import os
import sys

import cv2 as cv

from rpiasgige.client_api import Device

camera = Device("192.168.2.2", 4001)

if camera.isOpened():
    print("Camera is opened already!")
else:
    print("Camera is not opened. Trying to open now.")
    if camera.open():
        print("Successfully opened the camera")
    else:
        print("Failed to the camera", file=sys.stderr)
        os._exit(0)

WIDTH = 1280
HEIGHT = 720

if not camera.set(cv.CAP_PROP_FRAME_WIDTH, WIDTH):
    print("Failed to set width resolution to " + str(WIDTH), file=sys.stderr)
    os._exit(0)
else:
    print("Successfully set width resolution to " + str(WIDTH))

if not camera.set(cv.CAP_PROP_FRAME_HEIGHT, HEIGHT):
    print("Failed to set height resolution to " + str(HEIGHT), file=sys.stderr)
    os._exit(0)
else:
    print("Successfully set height resolution to " + str(HEIGHT))

for i in range(1000):
    ret, frame = camera.read()
    if not ret:
        print("failed to grab frame", file=sys.stderr)
        break
    cv.imshow("frame", frame)

    k = cv.waitKey(1)
    if k % 256 == 27:
        break

if camera.release():
    print("Successfully released the camera")
else:
    print("Failed to release the camera", file=sys.stderr)
