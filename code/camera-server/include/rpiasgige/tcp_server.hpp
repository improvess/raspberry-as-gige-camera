#ifndef RPIASGIGE_TCP_SERVER_INTERFACE_HPP
#define RPIASGIGE_TCP_SERVER_INTERFACE_HPP

#include "usb_interface.hpp"
#include "single_channel_server.hpp"
#include "tcp_server.hpp"

namespace rpiasgige
{

    class TCP_Server : public Single_Channel_Server
        {

        public:
            TCP_Server(const std::string & identifier, int _port, USB_Interface &_usb_camera, const int max_image_size_in_bytes) : Single_Channel_Server(identifier, _port, HEADER_SIZE + 32, max_image_size_in_bytes), usb_camera(_usb_camera) {}
            virtual ~TCP_Server() {}

        protected:

            void respond_to_client(int &client_socket, const int data_size)
            {

                const char *request_buffer_ro = this->get_request_buffer();

                if (strncmp("GRAB", request_buffer_ro, STATUS_SIZE) == 0)
                {

                    this->usb_camera.grab();

                    const auto & mat = this->usb_camera.get_captured_image();

                    int image_size = 0;

                    if (!mat.empty())
                    {
                        image_size = mat.total() * mat.elemSize();
                        const char *mat_data = (const char *)mat.data;
                        int size_int = sizeof(int);
                        const int metada_data_size = 3*size_int;
                        set_buffer_value(HEADER_SIZE, size_int, &mat.rows);
                        set_buffer_value(HEADER_SIZE + size_int, size_int, &mat.cols);
                        int type = mat.type();
                        set_buffer_value(HEADER_SIZE + 2*size_int, size_int, &type);
                        set_buffer_value(HEADER_SIZE + metada_data_size, image_size, mat_data);
                        send_buffer_to_client(client_socket, "0200", image_size);
                    } else {
                        send_buffer_to_client(client_socket, "NOPE", 0);
                    }
        
                }
                else if (strncmp("SET0", request_buffer_ro, STATUS_SIZE) == 0)
                {
                    if (data_size >= 12) {

                        int propId;
                        double value;

                        memcpy(&propId, request_buffer_ro + HEADER_SIZE, sizeof(int));
                        memcpy(&value, request_buffer_ro + HEADER_SIZE + sizeof(int), sizeof(double));

                        bool result = this->usb_camera.set(propId, value);

                        if (result)
                        {
                            send_buffer_to_client(client_socket, "0200", 0);
                        } else {
                            send_buffer_to_client(client_socket, "NOPE", 0);
                        }

                    } else {
                        send_buffer_to_client(client_socket, "0400", 0);
                    }

                } else if (strncmp("OPEN", request_buffer_ro, STATUS_SIZE) == 0)
                {
                    bool result = this->usb_camera.open(this->get_identifier());

                    if (result)
                    {
                        send_buffer_to_client(client_socket, "0200", 0);
                    } else {
                        send_buffer_to_client(client_socket, "NOPE", 0);
                    }

                } else if (strncmp("CLOS", request_buffer_ro, STATUS_SIZE) == 0)
                {
                    bool result = this->usb_camera.release();

                    if (result)
                    {
                        send_buffer_to_client(client_socket, "0200", 0);
                    } else {
                        send_buffer_to_client(client_socket, "NOPE", 0);
                    }

                } else if (strncmp("GET0", request_buffer_ro, STATUS_SIZE) == 0)
                {

                    int propId;

                    memcpy(&propId, request_buffer_ro + HEADER_SIZE, sizeof(int));

                    double value = this->usb_camera.get(propId);

                    set_buffer_value(HEADER_SIZE, SIZE_OF_DOUBLE, &value);

                    send_buffer_to_client(client_socket, "0200", SIZE_OF_DOUBLE);

                } else if (strncmp("ISOP", request_buffer_ro, STATUS_SIZE) == 0)
                {
                    bool result = this->usb_camera.isOpened();

                    if (result)
                    {
                        send_buffer_to_client(client_socket, "0200", 0);
                    } else {
                        send_buffer_to_client(client_socket, "NOPE", 0);
                    }

                } else if (strncmp("PING", request_buffer_ro, STATUS_SIZE) == 0)
                {
                    send_buffer_to_client(client_socket, "PONG", 0);
                }
                else
                {
                    send_buffer_to_client(client_socket, "0404", 0);
                }
            }

        private:
            USB_Interface &usb_camera;
            static const int SIZE_OF_DOUBLE = sizeof(double);
            static const int SET_DATA_SIZE = sizeof(int) + SIZE_OF_DOUBLE;
        };


}



#endif