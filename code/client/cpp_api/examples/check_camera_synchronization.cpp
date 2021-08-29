#include <iostream>

#include "rpiasgige/client_api.hpp"

/**
 * This example allows to check out if the images provided by two or more cameras are actually synchronized at realtime.
 * 
 * The images from two remote cameras are grabbed continuously and show
 * on two windows. The images are updated continuously until the spacebar key is pressed. When the spacebar is pressed, the images are 
 * freezed as a snapshot. The images back to be updated continuously when the spacebar is hit again.
 * 
 * In order to verify if the cameras images are sync, one can put a hundredth-precision stopwatch in the front of the cameras and then 
 * press the spacebar to check out if the two images capture the same time within the range of 1 or 2 hundredth of a second.
 * 
 **/

using namespace rpiasgige::client;

int main(int argc, char **argv)
{

    Device camera1("192.168.2.2", 4001);
    Device camera2("192.168.2.2", 4002);

    // make conversation persistent
    bool keep_alive = true;

    // Sending some packets to the cameras. It is optional

    for (int i = 0; i < 10; i++) {
        std::cout << "Camera 1 replied " << camera1.ping(keep_alive) << "\n";
        std::cout << "Camera 2 replied " << camera2.ping(keep_alive) << "\n";
    }

    // Checking if the cameras are opened already
    if (camera1.isOpened(keep_alive))
    {
        std::cout << "Nice! The camera 1 is opened already!\n";
    } else if (!camera1.open(keep_alive))
    {
        std::cerr << "Ops! Something is wrong! Failed to open the camera 1! Exiting ...\n";
        exit(0);
    }

    if (camera2.isOpened(keep_alive))
    {
        std::cout << "Nice! The camera 2 is opened already!\n";
    } else if (!camera2.open(keep_alive))
    {
        std::cerr << "Ops! Something is wrong! Failed to open the camera 2! Exiting ...\n";
        exit(0);
    }

    // setting camera parameters

    const int frame_width = 320;
    const int frame_height = 240;
    const auto mjpg = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
    const int fps = 60; //check if your camera supports 60 fps at MJPG @ 3200x240

    if (camera1.set(cv::CAP_PROP_FRAME_WIDTH, frame_width, keep_alive) && camera2.set(cv::CAP_PROP_FRAME_WIDTH, frame_width, keep_alive))
    {
        std::cout << "Frame width set!\n";
    }
    else
    {
        std::cerr << "Failed to set frame width!\n";
        exit(0);
    }

    if (camera1.set(cv::CAP_PROP_FRAME_HEIGHT, frame_height, keep_alive) && camera2.set(cv::CAP_PROP_FRAME_HEIGHT, frame_height, keep_alive))
    {
        std::cout << "Frame height set!\n";
    }
    else
    {
        std::cerr << "Failed to set frame height!\n";
        exit(0);
    }

    if (camera1.set(cv::CAP_PROP_FOURCC, mjpg, keep_alive) && camera2.set(cv::CAP_PROP_FOURCC, mjpg, keep_alive))
    {
        std::cout << "MJPG encoding set!\n";
    }
    else
    {
        std::cerr << "Failed to set MJPG encoding!\n";
        exit(0);
    }

    if (camera1.set(cv::CAP_PROP_FPS, fps, keep_alive) && camera2.set(cv::CAP_PROP_FPS, fps, keep_alive))
    {
        std::cout << "Nice! Your cameras seem to support delivering at 60 fps!!!\n";
    }
    else
    {
        std::cerr << "Sorry, you cameras do not support run at 60 fps. No problem at all, keep going.\n";
    }

    Performance_Counter performance_counter(120);

    cv::Mat mat1, mat2;

    int key = 0;
    bool freeze = false;

    while (key != 27) {

        bool s1 = camera1.retrieve(mat1, keep_alive);
        bool s2 = s1 && camera2.retrieve(mat2, keep_alive);

        if (!s1) {
            std::cerr << "Failed to grab frame from camera 1!\n";
            break;
        }

        if (!s2) {
            std::cerr << "Failed to grab frame from camera 2!\n";
            break;
        }

        int image_size_1 = mat1.total() * mat1.elemSize();
        int image_size_2 = mat2.total() * mat2.elemSize();
        if (performance_counter.loop(image_size_1 + image_size_2)) {
            printf("fps: %.1f mean data read size: %.1f\n" , performance_counter.get_fps(), performance_counter.get_mean_data_size());
        }
        if (!freeze) {
            cv::imshow("mat1", mat1);
            cv::imshow("mat2", mat2);
        }

        key = cv::waitKey(3);

        if (key == 32) {
            freeze = !freeze;
        }
    }

    //setting keep-alive as false to finish the communication after next calls
    keep_alive = false;

    if (!camera1.release(keep_alive) || !camera2.release(keep_alive))
    {
        std::cerr << "Dammit! Failed to close the camera!\n";
        exit(0);
    }
    else
    {
        std::cout << "Cameras closed successfuly!\n";
    }

    return 0;
}