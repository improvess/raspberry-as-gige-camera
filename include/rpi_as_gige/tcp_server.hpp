#ifndef RPIASGIGE_TCP_SERVER_INTERFACE_HPP
#define RPIASGIGE_TCP_SERVER_INTERFACE_HPP

#include "single_channel_server.hpp"

namespace rpiasgige
{

    class TCP_Server : public Single_Channel_Server
        {

        public:
            TCP_Server(const std::string & identifier, int _port, USB_Interface &_usb_camera, const int max_image_size_in_bytes) : Single_Channel_Server(identifier, _port, STATUS_SIZE, max_image_size_in_bytes), usb_camera(_usb_camera) {}
            virtual ~TCP_Server() {}

        protected:

            void respond_to_client(int &client_socket, const int data_size)
            {

                const char *request_buffer_ro = this->get_request_buffer();

                //printf("buffer is : %s\n", request_buffer_ro);

                if (strncmp("GET0", request_buffer_ro, STATUS_SIZE) == 0)
                {

                    this->usb_camera.grab();

                    const auto & mat = this->usb_camera.get_captured_image();

                    int image_size = 0;

                    if (!mat.empty())
                    {
                        image_size = mat.total() * mat.elemSize();
                        const char *mat_data = (const char *)mat.data;
                        set_buffer_value(HEADER_SIZE, image_size, mat_data);
                    } 

                    //printf("image_size: %d\n", image_size);

                    send_buffer_to_client(client_socket, "GET0", image_size);
                }
                else if (strncmp("ping", request_buffer_ro, STATUS_SIZE) == 0)
                {
                    send_buffer_to_client(client_socket, "pong", 0);
                }
                else
                {
                    // NOP
                }
            }

        private:
            USB_Interface &usb_camera;
        };


}



#endif