#include <chrono>
#include <thread>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace beast = boost::beast;         
namespace http = beast::http;           
namespace websocket = beast::websocket; 
namespace net = boost::asio;           
using tcp = boost::asio::ip::tcp;

#include <opencv2/opencv.hpp>

#include "rpiasgige/machine_vision_server.hpp"
#include "rpiasgige/usb_interface.hpp"

#include "rpiasgige/constants.hpp"

void do_session(tcp::socket& socket, rpiasgige::Server &server)
{
    char * response_buffer = nullptr;
    
    try
    {
        websocket::stream<tcp::socket> ws{std::move(socket)};

        ws.accept();

        response_buffer = new char[server.get_max_response_buffer_size()];

        while(true) {
            beast::flat_buffer buffer;

            ws.read(buffer);

            int request_size = buffer.size();
            const char* request_buffer = boost::asio::buffer_cast<const char*>(buffer.data());

            int response_size;

            server.process_client(request_buffer, request_size, response_buffer, response_size);

            ws.text(false);
            ws.binary(true);
            ws.write(net::buffer(response_buffer, response_size));
        }

    } catch(beast::system_error const& se) {

        if(se.code() != websocket::error::closed)
            std::cerr << "Error: " << se.code().message() << "\n";

    } catch(std::exception const& e) {

        std::cerr << "Error: " << e.what() << "\n";

    }

    if (response_buffer != nullptr) {
        delete [] response_buffer;
    }

}

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

    const unsigned short server_port = static_cast<unsigned short>(parser.get<int>("port"));
    const std::string server_address = parser.get<std::string>("address");

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

    rpiasgige::Server server(identifier, usb_camera, max_response_buffer_size);

    if (!server.init()) {
        std::cerr << "Failed to initialize server.";
        return EXIT_FAILURE;
    }
        
    try
    {
        net::io_context ioc{1};
        auto const address = net::ip::make_address(server_address);
        tcp::acceptor acceptor{ioc, {address, server_port}};

        while(server.is_online())
        {
            tcp::socket socket{ioc};
            acceptor.accept(socket);

            std::thread{std::bind(
                &do_session,
                std::move(socket), std::ref(server))}.detach();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}
