#include <iostream>

#include "rpiasgige/client_api.hpp"

int main(int argc, char **argv)
{

    rpiasgige::client::Device camera("192.168.2.2", 4001, rpiasgige::client::HEADER_SIZE + 32, 3 * 480 * 640*2 + rpiasgige::client::HEADER_SIZE);

    bool keep_alive = true;
    if (camera.isOpened(keep_alive))
    {
        std::cout << "Nice! The camera is opened already!\n";
    } else if (!camera.open(keep_alive))
    {
        std::cerr << "Ops! Something is wrong! Failed to open the camera! Exiting ...\n";
        exit(0);
    }

    if (camera.isOpened(keep_alive))
    {
        std::cout << "Ok! The camera is now opened and read to grab some images!\n";
    }
    else
    {
        std::cerr << "OMG! Something is wrong! The camera should be open but it is not!\n";
        exit(0);
    }

    if (camera.set(cv::CAP_PROP_FRAME_WIDTH, 640, keep_alive))
    {
        std::cout << "Frame width set!\n";
    }
    else
    {
        std::cerr << "Failed to set frame width!\n";
        exit(0);
    }

    if (camera.set(cv::CAP_PROP_FRAME_HEIGHT, 480, keep_alive))
    {
        std::cout << "Frame height set!\n";
    }
    else
    {
        std::cerr << "Failed to set frame height!\n";
        exit(0);
    }

    if (camera.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), keep_alive))
    {
        std::cout << "MJPG encoding set!\n";
    }
    else
    {
        std::cerr << "Failed to set MJPG encoding!\n";
        exit(0);
    }

    if (camera.set(cv::CAP_PROP_FPS, 60, keep_alive))
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
    for(int i = 0; key != 27 && i < 1000; i++) {

        if(!camera.grab(mat, keep_alive)) {
            std::cerr << "This is not good. Failed to grab the " << i << "-th frame!\n";
            break;
        } else {
            int image_size = mat.total() * mat.elemSize();
            if (fps_counter.loop(image_size)) {
                printf("fps: %.1f mean data read size: %.1f\n" , fps_counter.get_fps(), fps_counter.get_mean_data_size());
            }
            cv::imshow("mat", mat);
            // note that waitKey slower fps
            if (i % 4 == 0) {
                key = cv::waitKey(1);
            }
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