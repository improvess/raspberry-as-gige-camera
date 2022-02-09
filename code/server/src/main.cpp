#include <chrono>
#include <thread>

#include <opencv2/opencv.hpp>

#include "rpiasgige/machine_vision_server.hpp"
#include "rpiasgige/usb_interface.hpp"

#include "rpiasgige/constants.hpp"

int main(int argc, char **argv)
{

    const std::string keys =
        "{device           | /dev/video0    | camera path such as /dev/video0         }"
        "{address           | 0.0.0.0    | server address name or ip       }"
        "{usb_bus_id           |     | usb bus id like:  usb-0000:00:14.0-1        }"
        "{port           | 4001    | TCP port to accept connections         }"
        "{max-width-resolution           | 1920    | Max acceptable width image resolution         }"
        "{max-heigth-resolution           | 1080    | Max acceptable heigth image resolution         }"
        "{max-number-of-channels           | 3    | Max acceptable number of image channels         }"
        ;

    cv::CommandLineParser parser(argc, argv, keys);

    int port = parser.get<int>("port");
    std::string address = parser.get<std::string>("address");

    std::string device = parser.get<cv::String>("device");

    std::string usb_bus_id = parser.get<cv::String>("usb_bus_id");

    int max_width = parser.get<int>("max-width-resolution");
    int max_heigth = parser.get<int>("max-heigth-resolution");

    int max_channels = parser.get<int>("max-number-of-channels");

    rpiasgige::USB_Interface usb_camera;

    std::string identifier = device;

    if (!usb_bus_id.empty()) {
        usb_camera.set_usb_bus_id(usb_bus_id);
        identifier = usb_bus_id;
    } else {
        usb_camera.set_camera_path(device);
    }

    int max_image_size = max_channels * max_width * max_heigth;
    int max_response_buffer_size = max_image_size + rpiasgige::HEADER_SIZE + rpiasgige::IMAGE_META_DATA_SIZE;

    rpiasgige::Server server(identifier, address, port, usb_camera, max_response_buffer_size);
        
    std::thread server_thread([&server]()
                                    { server.run(); });

    if (server_thread.joinable()){
        server_thread.join();
    }

    return 0;
}