#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "rpiasgige/client_api.hpp"

namespace rpiasgige
{
    namespace client
    {
        bool Device::set(int propId, double value, bool keep_alive)
        {
            bool result = false;
            try
            {
                const int data_size = sizeof(int) + sizeof(double);
                Packet request(this->request_buffer, keep_alive, data_size, this->request_buffer + HEADER_SIZE);
                request.set_status("SET0");
                memcpy(request.data, &propId, sizeof(int));
                memcpy(request.data + sizeof(int), &value, sizeof(double));
                
                Packet response(this->response_buffer, keep_alive, 0, this->response_buffer + HEADER_SIZE);
                this->send_request(request, response);
                result = response.check_if_status_is("0200");
            }
            catch (TimeoutException &tex)
            {
                this->handle_timeout(tex);
            }

            return result;
        }

        bool Device::ping(bool keep_alive)
        {
            bool result = "";
            try
            {
                Packet request(this->request_buffer, keep_alive, 0, this->request_buffer + HEADER_SIZE);
                request.set_status("PING");
                
                Packet response(this->response_buffer, keep_alive, 0, this->response_buffer + HEADER_SIZE);
                this->send_request(request, response);
                result = response.check_if_status_is("PONG");
            }
            catch (TimeoutException &tex)
            {
                this->handle_timeout(tex);
            }

            return result;
        }

        double Device::get(int propId, bool keep_alive)
        {
            double result = -1.0;
            try
            {
                const int data_size = sizeof(int);
                Packet request(this->request_buffer, keep_alive, data_size, this->request_buffer + HEADER_SIZE);
                request.set_status("GET0");
                memcpy(request.data, &propId, sizeof(int));

                Packet response(this->response_buffer, keep_alive, 0, this->response_buffer + HEADER_SIZE);
                this->send_request(request, response);
                this->read_response_data(&result, sizeof(result), response);
            }
            catch (TimeoutException &tex)
            {
                this->handle_timeout(tex);
            }
            return result;
        }

        bool Device::open(bool keep_alive)
        {
            bool result = false;
            try
            {
                Packet request(this->request_buffer, keep_alive, 0, this->request_buffer + HEADER_SIZE);
                request.set_status("OPEN");

                Packet response(this->response_buffer, keep_alive, 0, this->response_buffer + HEADER_SIZE);
                this->send_request(request, response);
                result = response.check_if_status_is("0200");
            }
            catch (TimeoutException &tex)
            {
                this->handle_timeout(tex);
            }
            return result;
        }

        bool Device::isOpened(bool keep_alive)
        {
            bool result = false;
            try
            {
                Packet request(this->request_buffer, keep_alive, 0, this->request_buffer + HEADER_SIZE);
                request.set_status("ISOP");
                
                Packet response(this->response_buffer, keep_alive, 0, this->response_buffer + HEADER_SIZE);
                this->send_request(request, response);
                result = response.check_if_status_is("0200");
            }
            catch (TimeoutException &tex)
            {
                this->handle_timeout(tex);
            }
            return result;
        }

        bool Device::retrieve(cv::Mat &dest, bool keep_alive)
        {
            bool result = false;
            try
            {
                Packet request(this->request_buffer, keep_alive, 0, this->request_buffer + HEADER_SIZE);
                request.set_status("GRAB");
                
                Packet response(this->response_buffer, keep_alive, 0, this->response_buffer + HEADER_SIZE);
                this->send_request(request, response);
                result = response.check_if_status_is("0200");
                if (result)
                {
                    const char * data = response.data;
                    int size_int = sizeof(int);
                    const int * rows = (int *)data;
                    const int * cols = (int *)(data + size_int);
                    const int * type = (int *)(data + 2*size_int);
                    cv::Mat temp = cv::Mat::zeros(*rows, *cols, *type);
                    temp.data = (unsigned char *)response.data + 3*size_int;
                    dest = temp;
                }
            }
            catch (TimeoutException &tex)
            {
                this->handle_timeout(tex);
            }
            return result;
        }

        bool Device::release(bool keep_alive)
        {
            bool result = false;
            try
            {
                Packet request(this->request_buffer, keep_alive, 0, this->request_buffer + HEADER_SIZE);
                request.set_status("CLOS");
                
                Packet response(this->response_buffer, keep_alive, 0, this->response_buffer + HEADER_SIZE);
                this->send_request(request, response);
                result = response.check_if_status_is("0200");
            }
            catch (TimeoutException &tex)
            {
                this->handle_timeout(tex);
            }
            return result;
        }

        void Device::handle_timeout(TimeoutException &tex)
        {
            std::cout << tex.what() << "\n";

            if (timeout_count < MAX_TIMEOUT_COUNT)
            {
                timeout_count++;
            }
            else
            {
                timeout_count = 0;
                this->disconnect();
            }
        }

        bool Device::open_tcp_conversation()
        {

            if ((this->server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                this->server_socket = -1;
            }

            struct sockaddr_in serv_addr;
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(this->port);

            if (inet_pton(AF_INET, this->address.c_str(), &serv_addr.sin_addr) <= 0)
            {
                close(this->server_socket);
                this->server_socket = -1;
            }

            if (connect(this->server_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
            {
                close(this->server_socket);
                this->server_socket = -1;
            }
            return this->is_connected();
        }

        void Device::send_request(const Packet &request, Packet &response)
        {
            memset(response_buffer, 0, response_buffer_size);

            bool result = false;
            if (!this->is_connected())
            {
                this->open_tcp_conversation();
            }

            if (this->is_connected())
            {
                if (request.keep_alive) {
                    request_buffer[KEEP_ALIVE_ADDRESS] = '1';
                } else {
                    request_buffer[KEEP_ALIVE_ADDRESS] = '0';
                }

                this->set_request_data_size(request.data_size);

                if (this->send_request_buffer(request.data_size + HEADER_SIZE)) {

                    this->read_response(response);

                    if (!request.keep_alive) {
                        this->disconnect();
                    }

                } else {
                    if (!request.keep_alive) {
                        this->disconnect();
                    }
                    throw TimeoutException{"Failed to send data to server."};
                }
            }
            else
            {
                throw RemoteException{"Not connected"};
            }
        }

        bool Device::send_request_buffer(const int bytes_to_send)
        {
            bool result = false;
            if (bytes_to_send <= this->request_buffer_size) {
                int sent = 0;
                bool keep = true;
                int fail_count = 0;
                while(keep && sent < bytes_to_send) {

                    int current_packet_size = std::min(bytes_to_send - sent, 1024);

                    int val_sent = send(this->server_socket, this->request_buffer + sent , current_packet_size , MSG_NOSIGNAL);
                    if(val_sent > 0) {
                        fail_count = 0;
                        sent += val_sent;
                    } else {
                        fail_count++;
                        keep = fail_count < 3;
                    }
                }
                result = sent == bytes_to_send;
            }

            return result;
        }

        void Device::disconnect()
        {
            if (this->server_socket >= 0)
            {
                close(this->server_socket);
                this->server_socket = -1;
            }
        }

        void Device::read_response_data(void *buffer, const int size, const Packet &response)
        {
            if (size > 0 && size <= response.data_size)
            {
                memcpy(buffer, response.data, size);
            }
        }

        bool Device::read_response(Packet &response)
        {

            int total_read = 0;
            bool keep = true;
            int no_response_count = 0;
            const int max_no_response_count = 3;
            int expected_response_size = HEADER_SIZE;

            int data_size = 0;
            while (keep)
            {
                int packt_size = std::min(1024, this->response_buffer_size - total_read);
                int valread = recv(this->server_socket, response_buffer + total_read, packt_size, 0);
                if (valread > 0)
                {
                    total_read += valread;
                    no_response_count = 0;

                    if (data_size == 0 && total_read >= HEADER_SIZE)
                    {
                        int expected_data_size;
                        memcpy(&expected_data_size, response_buffer + DATA_SIZE_ADDRESS, sizeof(expected_data_size));

                        if (expected_data_size > 0 && (HEADER_SIZE + expected_data_size) <= response_buffer_size)
                        {
                            data_size = expected_data_size;
                            expected_response_size = HEADER_SIZE + data_size;
                        }
                    }

                    keep = total_read < expected_response_size;
                }
                else
                {
                    no_response_count++;
                    keep = no_response_count < max_no_response_count;
                }
            }

            int result = std::min(total_read, HEADER_SIZE + data_size);
            if (result < expected_response_size)
            {
                result = 0;
            }
            //printf("read_response result %d\n", result);
            return result;
        }

    }

}