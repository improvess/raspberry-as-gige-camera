#include <chrono>
#include <thread>

#include "rpiasgige/tcp_server.hpp"

int main(int argc, char **argv)
{

    const std::string keys =
        "{camera-path           | /dev/video0    | camera path such as /dev/video0         }"
        ;

    cv::CommandLineParser parser(argc, argv, keys);

    std::string identifier = parser.get<cv::String>("camera-path");

    rpiasgige::USB_Interface usb_camera;

    rpiasgige::TCP_Server tcp_server(identifier, 4001, usb_camera, 3*640*480 * 2);
    std::thread tcp_server_thread([&tcp_server]()
                                    { tcp_server.run(); });

    if (tcp_server_thread.joinable()){
        tcp_server_thread.join();
    }

        /*
        usb_camera.set(cv::CAP_PROP_BUFFERSIZE, 2);
        usb_camera.set(cv::CAP_PROP_FRAME_WIDTH, 320);
        usb_camera.set(cv::CAP_PROP_FRAME_HEIGHT, 240);
        usb_camera.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
        */

    return 0;
}