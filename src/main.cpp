#include <chrono>
#include <thread>

#include "rpi_as_gige/usb_interface.hpp"
#include "rpi_as_gige/tcp_server.hpp"

int main(int argc, char **argv)
{

    const std::string keys =

        "{camera-path           | /dev/video0    | camera path such as /dev/video0         }"

        ;

    cv::CommandLineParser parser(argc, argv, keys);

    cv::String camera_path = parser.get<cv::String>("camera-path");

    rpiasgige::USB_Interface usb_camera;

    if (usb_camera.open(camera_path))
    {

        usb_camera.set(cv::CAP_PROP_BUFFERSIZE, 2);
        usb_camera.set(cv::CAP_PROP_FRAME_WIDTH, 320);
        usb_camera.set(cv::CAP_PROP_FRAME_HEIGHT, 240);
        usb_camera.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));

        usb_camera.open(camera_path);

        rpiasgige::TCP_Server tcp_server("tcp server", 4001, usb_camera, 3*640*480);
        std::thread tcp_server_thread([&tcp_server]()
                                      { tcp_server.run(); });

        if (tcp_server_thread.joinable()){
            tcp_server_thread.join();
        }

    } else {
        std::cerr << "Failed to open device at " << camera_path << "\n";
    }

    return 0;
}