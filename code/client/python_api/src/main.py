from rpiasgige.client_api import Device

camera = Device("192.168.2.2", 4001, 9)

for n in range(10):
    print("The camera reply with " + str(camera.ping()))

if camera.isOpened():
    print("Camera is opened already!")
else:
    print("Camera is not opened. Trying to open now.")
    if camera.open():
        print("Successfully opened the camera")
    else:
        print("Failed to the camera")

if camera.isOpened():
    if camera.release():
        print("Successfully released the camera")
    else:
        print("Failed to release the camera")

if not camera.isOpened():
    print("The camera is no longer opened")
else:
    print("OMG the camera is still opened!")