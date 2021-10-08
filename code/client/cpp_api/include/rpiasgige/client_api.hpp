#ifndef RPIASGIGE_CLIENT_API_HPP
#define RPIASGIGE_CLIENT_API_HPP

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <chrono>

#include <opencv2/opencv.hpp>

namespace rpiasgige
{

    namespace client
    {

        struct TimeoutException : std::runtime_error
        {
            TimeoutException(const std::string &msg) : std::runtime_error(msg) {}
            std::string origin;
        };

        struct RemoteException : std::runtime_error
        {
            RemoteException(const std::string &msg) : std::runtime_error(msg) {}
        };

        static const int STATUS_SIZE = 4;
        static const int KEEP_ALIVE_SIZE = 1;
        static const int DATA_SIZE = 4;
        static const int STATUS_ADDRESS = 0;
        static const int DATA_SIZE_ADDRESS = 5;
        static const int KEEP_ALIVE_ADDRESS = 4;
        static const int HEADER_SIZE = STATUS_SIZE + KEEP_ALIVE_SIZE + DATA_SIZE;
        static const int IMAGE_META_DATA_SIZE = 3 * sizeof(int);

        class Device;

        /**
         * Packet is a mold to make dealing with request / response buffers easier.
         **/
        struct Packet
        {
            char *status;
            bool keep_alive;
            int data_size;
            char *data;

            Packet(char *status, bool keep_alive, int data_size, char *data) : status(status), keep_alive(keep_alive), data_size(data_size), data(data) {}

            /**
             * return true if the packet status is equal to the parameter query
             **/
            bool check_if_status_is(const char *query)
            {
                return strncmp(status, query, STATUS_SIZE) == 0;
            }

            /**
             * set the status code of the Packet to new_status
             **/
            void set_status(const char *new_status)
            {
                memcpy(this->status, new_status, STATUS_SIZE);
            }

            /**
             * generates a string copy of the packet's status
             **/
            std::string get_status_as_str() const
            {
                char data[STATUS_SIZE + 1];
                memcpy(data, this->status, STATUS_SIZE);
                data[STATUS_SIZE] = '\0';
                return std::string(data);
            }
        };

        /**
         * This class represents a remote camera. It provides convenient API-level methods to allow open, close, retrieve, etc, a remote camera.
         * Basically, the methods serializes, send, read, and deserialize data from the camera.
         * 
         */
        class Device
        {
        public:
            Device(const std::string &server_address, const int server_port) : Device(server_address, server_port, HEADER_SIZE + 12, HEADER_SIZE + IMAGE_META_DATA_SIZE + 1920 * 1080 * 3 ) {}

            Device(const std::string &server_address, const int server_port, const int _response_buffer_size) : Device(server_address, server_port, HEADER_SIZE + 12, _response_buffer_size) {}

            Device(const std::string &server_address, const int server_port, const int _request_buffer_size, const int _response_buffer_size) : address(server_address), port(server_port)
            {

                if (_response_buffer_size > this->response_buffer_size)
                {
                    this->response_buffer_size = _response_buffer_size;
                }

                this->response_buffer = new char[this->response_buffer_size];

                if (_request_buffer_size > this->request_buffer_size)
                {
                    this->request_buffer_size = _request_buffer_size;
                }

                this->request_buffer = new char[this->request_buffer_size];
            }

            virtual ~Device() {

                if (this->response_buffer != nullptr) {
                    delete [] this->response_buffer;
                    this->response_buffer = nullptr;
                }

                if (this->request_buffer != nullptr) {
                    delete [] this->request_buffer;
                    this->request_buffer = nullptr;
                }

            }

            bool ping(bool keep_alive = false)
            {
                bool result = false;
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
                    this->handle_timeout("ping", tex);
                }

                return result;
            }

            double get(int propId, bool keep_alive = false)
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
                    this->handle_timeout("get", tex);
                }
                return result;
            }

            bool set(int propId, double value, bool keep_alive = false)
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
                    this->handle_timeout("set", tex);
                }

                return result;
            }

            bool open(bool keep_alive = false)
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
                    this->handle_timeout("open", tex);
                }
                return result;
            }

            bool isOpened(bool keep_alive = false)
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
                    this->handle_timeout("isOpened", tex);
                }
                return result;
            }

            bool retrieve(cv::Mat &dest, bool keep_alive = false)
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
                        const char *data = response.data;
                        int size_int = sizeof(int);
                        const int *rows = (int *)data;
                        const int *cols = (int *)(data + size_int);
                        const int *type = (int *)(data + 2 * size_int);
                        cv::Mat temp = cv::Mat::zeros(*rows, *cols, *type);
                        temp.data = (unsigned char *)response.data + 3 * size_int;
                        dest = temp;
                    }
                }
                catch (TimeoutException &tex)
                {
                    this->handle_timeout("retrieve", tex);
                }
                return result;
            }
            
            bool release(bool keep_alive = false)
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
                    this->handle_timeout("release", tex);
                }
                return result;
            }

            void set_read_timeout(int timeout_in_seconds)
            {
                if (this->read_timeout_in_seconds >= 0)
                {
                    this->read_timeout_in_seconds = timeout_in_seconds;
                }
                this->update_socket_timeout();
            }

        private:
            cv::String address;
            int port;
            int timeout_count = 0;
            const int MAX_TIMEOUT_COUNT = 2;
            int read_timeout_in_seconds = 1;

            void handle_timeout(const std::string &origin, TimeoutException &tex)
            {
                //std::cout << "TimeoutException: " << tex.what() << "\n";

                if (timeout_count < MAX_TIMEOUT_COUNT)
                {
                    timeout_count++;
                }
                else
                {
                    timeout_count = 0;
                    //std::cout << "disconnect after TimeoutException: " << tex.what() << "\n";
                    this->disconnect();
                    tex.origin = origin;
                    throw tex;
                }
            }

            void send_request(const Packet &request, Packet &response)
            {
                memset(response_buffer, 0, response_buffer_size);

                bool result = false;
                if (!this->is_connected())
                {
                    this->open_tcp_conversation();
                }

                if (this->is_connected())
                {
                    if (request.keep_alive)
                    {
                        request_buffer[KEEP_ALIVE_ADDRESS] = '1';
                    }
                    else
                    {
                        request_buffer[KEEP_ALIVE_ADDRESS] = '0';
                    }

                    this->set_request_data_size(request.data_size);

                    if (this->send_request_buffer(request.data_size + HEADER_SIZE))
                    {

                        this->read_response(response);

                        if (!request.keep_alive)
                        {
                            this->disconnect();
                        }
                    }
                    else
                    {
                        std::string last_error = std::string(strerror(errno));
                        if (!request.keep_alive)
                        {
                            this->disconnect();
                        }
                        throw TimeoutException{"Failed to send data to server: " + last_error};
                    }
                }
                else
                {
                    throw RemoteException{"Not connected"};
                }
            }

            bool open_tcp_conversation()
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
                this->update_socket_timeout();
                return this->is_connected();
            }

            void update_socket_timeout()
            {
                if (this->server_socket >= 0 && this->read_timeout_in_seconds >= 0)
                {
                    struct timeval tv;
                    tv.tv_sec = this->read_timeout_in_seconds;
                    // minimum acceptable timeout is 10 ms
                    tv.tv_usec = 10000;
                    setsockopt(this->server_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);

                }
            }

            int server_socket = -1;

            int response_buffer_size = HEADER_SIZE;
            char *response_buffer = nullptr;

            int request_buffer_size = HEADER_SIZE;
            char *request_buffer = nullptr;

            inline bool is_connected() const
            {
                return server_socket >= 0;
            }

            void disconnect()
            {
                if (this->server_socket >= 0)
                {
                    close(this->server_socket);
                    this->server_socket = -1;
                }
            }

            /* *
             * Load the response into the response parameter
             * */
            bool read_response(Packet &response)
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
                                response.data_size = data_size;
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
                    response.data_size = 0;
                }
                return result;
            }

            /**
             * loads into buffer a chunk of data from the response packet
             **/
            void read_response_data(void *buffer, const int size, const Packet &response)
            {
                if (size > 0 && size <= response.data_size)
                {
                    memcpy(buffer, response.data, size);
                }
            }

            /* *
             * helper method to send request
             * */
            bool send_request_buffer(const int bytes_to_send)
            {
                bool result = false;
                if (bytes_to_send <= this->request_buffer_size)
                {
                    int sent = 0;
                    bool keep = true;
                    int fail_count = 0;
                    while (keep && sent < bytes_to_send)
                    {

                        int current_packet_size = std::min(bytes_to_send - sent, 1024);

                        int val_sent = send(this->server_socket, this->request_buffer + sent, current_packet_size, MSG_NOSIGNAL);
                        if (val_sent > 0)
                        {
                            fail_count = 0;
                            sent += val_sent;
                        }
                        else
                        {
                            fail_count++;
                            keep = fail_count < 3;
                        }
                    }
                    result = sent == bytes_to_send;
                }

                return result;
            }

            void set_request_data_size(int size)
            {
                memcpy(request_buffer + DATA_SIZE_ADDRESS, &size, sizeof(size));
            }
        };

        /**
         * A utility to measure FPS and data-transfer easier
         **/
        class Performance_Counter
        {

        public:
            Performance_Counter(const int cycle_count) : CYCLE_COUNT(cycle_count)
            {
                if (cycle_count <= 0)
                {
                    throw std::invalid_argument("cycle_count must be a positive value");
                }
                this->reset();
            }

            bool loop(double data_size)
            {

                bool result = false;

                this->count++;
                this->total_read += data_size;

                if (this->count == 1)
                {
                    begin_time_ref = std::chrono::high_resolution_clock::now();
                }

                if (this->count >= CYCLE_COUNT)
                {
                    end_time_ref = std::chrono::high_resolution_clock::now();
                    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_ref - begin_time_ref);
                    auto time_spent = ms.count();
                    this->mean_data_size = total_read / this->count;
                    if (time_spent >= 1)
                    {
                        this->fps = this->count * 1000.0 / time_spent;
                        this->mean_data_size = this->total_read / this->count;

                        result = true;
                    }
                    else
                    {
                        this->fps = -1.0;
                    }
                    this->count = 0;
                    this->total_read = 0;
                }
                return result;
            }

            void reset()
            {
                this->count = 0;
                this->total_read = 0.0;

                this->fps = -1.0;
                this->mean_data_size = -1.0;
            }

            double get_fps() const
            {
                return this->fps;
            }

            double get_mean_data_size() const
            {
                return this->mean_data_size;
            }

        private:
            const int CYCLE_COUNT;
            int count;
            double total_read;

            double fps;
            double mean_data_size;

            std::chrono::high_resolution_clock::time_point begin_time_ref;
            std::chrono::high_resolution_clock::time_point end_time_ref;
        };

    }

}

#endif