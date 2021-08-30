import os
import sys

import cv2 as cv

from rpiasgige.client_api import Device, Performance_Counter, printf

# This is a basic example of C++ API rpiasgige usage

def main():

    camera = Device("192.168.2.2", 4001)

    # let's ping the camera just to check we can talk to it
    if not camera.ping():
        print("Ops! Camera didn't reply. Exiting...", file=sys.stderr)
        os._exit(0)

    print("Great! Camera replied, let's move ahead!")

    # The previous call was done in non-keep-alive mode.
    # This is not good for long conversations because each call will perform a open-close TCP cycle
    # To make calls faster, it is recommended to keep-alive the conversation as done below

    keep_alive = True

    # Sending some packets to the camera

    for i in range(10):
        if camera.ping(keep_alive):
            printf("Camera successfully replied to the %d-th ping!\n", i + 1)

    # Now let's actually open the camera so we can grab some frames
    # The camera can be opened already due to a previous call
    # Thus, the first this is checking if the camera is opened already

    if camera.isOpened(keep_alive):
        print("Camera is opened already!")
    # If the camera is not open, we can open it no!
    else:
        print("Camera is not opened. Trying to open now.")
        if camera.open(keep_alive):
            print("Successfully opened the camera")
        else:
            print("Failed to open the camera", file=sys.stderr)
            os._exit(0)
    
    # Let's setup some camera properties
    # Rememeber that properties are model-specific features. Thus, adapt the folllowing settings 
    # to your actual camera brand and needs

    WIDTH = 1280
    HEIGHT = 720
    FPS = 30
    MJPG = cv.VideoWriter.fourcc('M', 'J', 'P', 'G')

    if not camera.set(cv.CAP_PROP_FRAME_WIDTH, WIDTH, keep_alive):
        print("Failed to set width resolution to " + str(WIDTH), file=sys.stderr)
        os._exit(0)
    else:
        print("Successfully set width resolution to " + str(WIDTH))

    if not camera.set(cv.CAP_PROP_FRAME_HEIGHT, HEIGHT, keep_alive):
        print("Failed to set height resolution to " + str(HEIGHT), file=sys.stderr)
        os._exit(0)
    else:
        print("Successfully set height resolution to " + str(HEIGHT))

    if not camera.set(cv.CAP_PROP_FOURCC, MJPG, keep_alive):
        print("Failed to set MJPG", file=sys.stderr)
        os._exit(0)
    else:
        print("Successfully set MJPG")

    # Note that the actual achieved FPS speed is a result of several different factors
    # like resolution, ambient light / exposure settings, network bandwidth, CPU consume, etc
    # For example, some cameras only achieve high FPS when AUTO FOCUS is disabled
    camera.set(cv.CAP_PROP_AUTOFOCUS, 0, keep_alive)

    # Note that AUTO FOCUS is not a mandatory feature for every camera. So the previous call can return false
    # Now, let's ask the camera to run at our predefined FPS rate

    if camera.set(cv.CAP_PROP_FPS, FPS, keep_alive):
        printf("Nice! Your camera seems to accept setting fps to %d !!!\n", FPS)
    else:
        printf("Sorry, you camera seems to do not support run at %d fps. No problem at all, keep going.\n", FPS)

    # Everything is set up, time to grab some frames
    # Performance_Counter is an optional component. 
    # It is a convenient way to measure the achieved FPS speed and mean data transfered. 

    performance_counter = Performance_Counter(120)

    for i in range(1000):
        ret, frame = camera.read(keep_alive)
        if not ret:
            print("failed to grab frame", file=sys.stderr)
            break
        image_size = frame.size
        if performance_counter.loop(image_size):
            printf("fps: %.1f, mean data read size: %.1f\n" , performance_counter.get_fps(), performance_counter.get_mean_data_size())

        # note that imshow & waitKey slower fps
        cv.imshow("frame", frame)
        k = cv.waitKey(1)
        if k % 256 == 27:
            break

    # The next call closes the camera, a reasonable good practice
    # Since it is the last call, let's close the network conversation as well by setting keep-alive to false
    keep_alive = False
    if camera.release(keep_alive):
        print("Successfully released the camera")
    else:
        print("Failed to release the camera", file=sys.stderr)

if __name__ == "__main__":
    main()