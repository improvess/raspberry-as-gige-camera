#include <iostream>

#include "rpiasgige/client_api.hpp"

using namespace rpiasgige::client;

/**
 * This is a basic example of rpiasgige usage 
 * 
 **/
int main(int argc, char **argv)
{


    const int frame_width = 1280;
    const int frame_height = 720;
    const auto mjpg = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
    const int fps = 30;

    const int MAX_IMAGE_SIZE = frame_width * frame_height * 3; // for a 3-channel image with max resolution of 1280x720 

    const std::string address = "192.168.2.2";
    const int port = 4001;

    Device camera(address, port, MAX_IMAGE_SIZE + HEADER_SIZE + IMAGE_META_DATA_SIZE);

    // make the conversation persistent
    bool keep_alive = true;

    // optional. Sending some packets to the camera

    for (int i = 0; i < 10; i++) {
        std::cout << "Camera replied " << camera.ping(keep_alive) << "\n";
    }

    // Checking if the camera is opened already
    if (camera.isOpened(keep_alive))
    {
        std::cout << "Nice! The camera is opened already!\n";

    //opening the camera once it is not yet
    } else if (!camera.open(keep_alive))
    {
        std::cerr << "Ops! Something is wrong! Failed to open the camera! Exiting ...\n";
        exit(0);
    }

    // setting camera properties

    if (camera.set(cv::CAP_PROP_FRAME_WIDTH, frame_width, keep_alive))
    {
        std::cout << "Frame width set!\n";
    }
    else
    {
        std::cerr << "Failed to set frame width!\n";
        exit(0);
    }

    if (camera.set(cv::CAP_PROP_FRAME_HEIGHT, frame_height, keep_alive))
    {
        std::cout << "Frame height set!\n";
    }
    else
    {
        std::cerr << "Failed to set frame height!\n";
        exit(0);
    }

    if (camera.set(cv::CAP_PROP_FOURCC, mjpg, keep_alive))
    {
        std::cout << "MJPG encoding set!\n";
    }
    else
    {
        std::cerr << "Failed to set MJPG encoding!\n";
        exit(0);
    }

    if (camera.set(cv::CAP_PROP_FPS, fps, keep_alive))
    {
        std::cout << "Nice! Your camera seems to support delivering at 60 fps!!!\n";
    }
    else
    {
        std::cerr << "Sorry, you camera seems to do not support run at 60 fps. No problem at all, keep going.\n";
    }

    rpiasgige::client::FPS_Counter fps_counter(120);

    cv::Mat mat;

    int key = 0;
    for(int i = 0; key != 27; i++) {

        if(!camera.retrieve(mat, keep_alive)) {
            std::cerr << "This is not good. Failed to grab the " << i << "-th frame!\n";
            break;
        } else {
            int image_size = mat.total() * mat.elemSize();
            if (fps_counter.loop(image_size)) {
                printf("fps: %.1f mean data read size: %.1f\n" , fps_counter.get_fps(), fps_counter.get_mean_data_size());
            }
            // note that imshow & waitKey slower fps
            cv::imshow("mat", mat);
            key = cv::waitKey(1);
        }
    }

    keep_alive = false;

    if (!camera.release(keep_alive))
    {
        std::cerr << "Dammit! Failed to close the camera!\n";
        exit(0);
    }
    else
    {
        std::cout << "Camera closed successfuly!\n";
    }

    return 0;
}