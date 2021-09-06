#include <iostream>

#include "rpiasgige/client_api.hpp"

using namespace rpiasgige::client;

/**
 * This is a basic example of C++ API rpiasgige usage
 * 
 **/
int main(int argc, char **argv)
{

    const std::string address = "192.168.2.3";
    const int port = 4001;

    Device camera(address, port);

    // let's ping the camera just to check we can talk to it

    if (!camera.ping()) {
        std::cerr << "Ops! Camera didn't reply. Exiting...\n";
        exit(0);
    }

    std::cout << "Great! Camera replied, let's move ahead!\n";

    // The previous call was done in non-keep-alive mode.
    // This is not good for long conversations because each call will perform a open-close TCP cycle
    // To make calls faster, it is recommended to keep-alive the conversation as done below

    bool keep_alive = true;

    // Sending some packets to the camera

    for (int i = 0; i < 10; ) {
        if (camera.ping(keep_alive)) {
            std::cout << "Camera successfully replied to the " << ++i << "-th ping!\n";
        }
    }

    // Now let's actually open the camera so we can grab some frames
    // The camera can be opened already due to a previous call
    // Thus, the first this is checking if the camera is opened already
    if (camera.isOpened(keep_alive))
    {
        std::cout << "Nice! The camera is opened already!\n";
    // If the camera is not open, we can open it no!
    } else if (!camera.open(keep_alive))
    {
        std::cerr << "Ops! Something is wrong! Failed to open the camera! Exiting ...\n";
        exit(0);
    }

    // Let's setup some camera properties
    // Rememeber that properties are model-specific features. Thus, adapt the folllowing settings 
    // to your actual camera brand and needs

    const double frame_width = 640;
    const double frame_height = 480;
    const double mjpg = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
    const double fps = 30;

    camera.set(cv::CAP_PROP_FRAME_WIDTH, frame_width, keep_alive);
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, frame_height, keep_alive);
    camera.set(cv::CAP_PROP_FOURCC, mjpg, keep_alive);
    camera.set(cv::CAP_PROP_FPS, fps, keep_alive);

    if (camera.get(cv::CAP_PROP_FRAME_WIDTH, keep_alive) == frame_width)
    {
        std::cout << "Frame width set to " << frame_width << "!\n";
    }
    else
    {
        std::cerr << "Failed to set frame width!\n";
        exit(0);
    }

    if (camera.get(cv::CAP_PROP_FRAME_HEIGHT, keep_alive) == frame_height)
    {
        std::cout << "Frame height set to " << frame_height << "!\n";
    }
    else
    {
        std::cerr << "Failed to set frame height!\n";
        exit(0);
    }

    if (camera.get(cv::CAP_PROP_FOURCC, keep_alive) == mjpg)
    {
        std::cout << "MJPG encoding set!\n";
    }
    else
    {
        std::cerr << "Failed to set MJPG encoding!\n";
        exit(0);
    }

    // Note that the actual achieved FPS speed is a result of several different factors
    // like resolution, ambient light / exposure settings, network bandwidth, CPU consume, etc
    // For example, some cameras only achieve high FPS when AUTO FOCUS is disabled

    camera.set(cv::CAP_PROP_AUTOFOCUS, 0, keep_alive);

    // Note that AUTO FOCUS is not a mandatory feature for every camera. So the previous call can return false
    // Now, let's ask the camera to run at our predefined FPS rate

    if (camera.get(cv::CAP_PROP_FPS, keep_alive) == fps)
    {
        double actual_fps_settings = camera.get(cv::CAP_PROP_FPS);
        if (std::abs(actual_fps_settings - fps) < 0.1) {
            std::cout << "Nice! Your camera seems to accept setting fps to " << fps << " !!!\n";
        }
    }
    else
    {
        std::cerr << "Sorry, you camera seems to do not support run at 60 fps. No problem at all, keep going.\n";
    }

    // Everything is set up, time to grab some frames
    // Performance_Counter is an optional component. 
    // It is a convenient way to measure the achieved FPS speed and mean data transfered. 

    Performance_Counter performance_counter(120);

    cv::Mat mat;

    int key = 0;
    for(int i = 0; key != 27 && i < 1000; i++) {

        if(!camera.retrieve(mat, keep_alive)) {
            std::cerr << "This is not good. Failed to grab the " << i << "-th frame!\n";
            break;
        } else {
            int image_size = mat.total() * mat.elemSize();
            if (performance_counter.loop(image_size)) {
                printf("fps: %.1f, mean data read size: %.1f\n" , performance_counter.get_fps(), performance_counter.get_mean_data_size());
            }
            // note that imshow & waitKey slower fps
            cv::imshow("mat", mat);
            key = cv::waitKey(1);
        }
    }

    // The next call closes the camera, a reasonable good practice
    // Since it is the last call, let's close the network conversation as well by setting keep-alive to false
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