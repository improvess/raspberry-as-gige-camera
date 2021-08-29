#ifndef RPIASGIGE_SINGLE_CHANNEL_SERVER_HPP
#define RPIASGIGE_SINGLE_CHANNEL_SERVER_HPP

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>

#include <string.h>

#include "rpiasgige/dumb_logger.hpp"

namespace rpiasgige
{

    class Single_Channel_Server
    {

    public:
        static const int STATUS_SIZE = 4;
        static const int KEEP_ALIVE_SIZE = 1;
        static const int DATA_SIZE = 4;
        static const int STATUS_ADDRESS = 0;
        static const int DATA_SIZE_ADDRESS = 5;
        static const int KEEP_ALIVE_ADDRESS = 4;
        static const int HEADER_SIZE = STATUS_SIZE + KEEP_ALIVE_SIZE + DATA_SIZE;

        Single_Channel_Server(const std::string &_identifier, int _port, int _request_buffer_size, int _response_buffer_size) : identifier(_identifier), port(_port), logger(_identifier)
        {

            if (_response_buffer_size > this->response_buffer_size)
            {
                this->response_buffer_size = _response_buffer_size;
            }
            if (_request_buffer_size > this->request_buffer_size)
            {
                this->request_buffer_size = _request_buffer_size;
            }

            this->response_buffer = new char[this->response_buffer_size];
            this->request_buffer = new char[this->request_buffer_size];

            this->client_read_timeout_in_seconds = 1;
            this->client_write_timeout_in_seconds = 1;
        }

        virtual ~Single_Channel_Server()
        {
            if (this->response_buffer)
            {
                delete[] this->response_buffer;
            }
            if (this->request_buffer)
            {
                delete[] this->request_buffer;
            }
        }

        void run()
        {
            bool initialized = false;

            while (true)
            {
                if (!initialized)
                {

                    this->logger.debug_msg("Not initialized. Initializing now.");
                    initialized = init();
                }
                else
                {

                    this->logger.debug_msg("Waiting for client");
                    int client_socket = accept(this->server_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen);

                    if (client_socket >= 0)
                    {

                        this->set_client_timeout(client_socket);

                        this->logger.debug_msg("Client just arrived. Processing its request.");

                        int client_timeout_count = 0;
                        const int client_max_timeout_count = 0;

                        bool keep = true;

                        while (keep)
                        {
                            int request_size = read_client_request(client_socket);
                            if (request_size >= HEADER_SIZE)
                            {
                                int data_size = request_size - HEADER_SIZE;

                                this->keep_alive = this->request_buffer[KEEP_ALIVE_ADDRESS] == '1';
                                respond_to_client(client_socket, data_size);
                                client_timeout_count = 0;
                                if (!this->keep_alive)
                                {
                                    shutdown(client_socket, SHUT_RDWR);
                                    this->keep_alive = false;
                                    this->logger.debug_msg("Closing client socket due to keep alive was set false");
                                    keep = false;
                                }
                            }
                            else
                            {
                                client_timeout_count++;
                                if (client_timeout_count >= client_max_timeout_count)
                                {
                                    close(client_socket);
                                    this->keep_alive = false;
                                    this->logger.debug_msg("Client stop to responding. Closing and wating for a new connection.");
                                    keep = false;
                                }
                            }
                        }
                    }
                    else
                    {
                        initialized = process_accept_error(errno);
                    }
                }
            }
        }

    protected:

        const std::string &get_identifier() const {
            return this->identifier;
        }

        virtual void respond_to_client(int &client_socket, const int data_size) = 0;

        void set_status(const char *status)
        {
            memcpy(response_buffer + STATUS_ADDRESS, status, STATUS_SIZE);
        }

        void set_response_data_size(int size)
        {
            memcpy(response_buffer + DATA_SIZE_ADDRESS, &size, sizeof size);
        }

        void send_buffer_to_client(int &client_socket, const char *status, int data_size)
        {

            int response_size = HEADER_SIZE + data_size;

            int packet_size = std::min(1024, response_size);

            if (this->keep_alive)
            {
                response_buffer[KEEP_ALIVE_ADDRESS] = '1';
            }
            else
            {
                response_buffer[KEEP_ALIVE_ADDRESS] = '0';
            }

            this->set_status(status);

            this->set_response_data_size(data_size);

            int sent = 0;
            bool keep = true;
            int fail_count = 0;

            //printf("Sending %s\n", response_buffer);

            while (keep && sent < response_size)
            {

                int current_packet_size = std::min(response_size - sent, packet_size);

                int val_sent = send(client_socket, response_buffer + sent, current_packet_size, MSG_NOSIGNAL);
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

            //printf("Sent %d\n", sent);
        }

        int read_client_request(int &socket)
        {
            int total_read = 0;
            bool keep = true;
            int no_response_count = 0;
            const int max_no_response_count = 3;
            int expected_request_size = HEADER_SIZE;

            int data_size = 0;
            while (keep)
            {

                int valread = recv(socket, request_buffer + total_read, this->request_buffer_size, 0);
                //printf("valread %d of %d\n", valread, request_buffer_size);
                if (valread > 0)
                {
                    total_read += valread;
                    no_response_count = 0;

                    if (data_size == 0 && total_read >= HEADER_SIZE)
                    {
                        int temp;
                        memcpy(&temp, request_buffer + DATA_SIZE_ADDRESS, sizeof temp);

                        if (temp > 0 && (HEADER_SIZE + temp) <= request_buffer_size)
                        {
                            data_size = temp;
                            expected_request_size = HEADER_SIZE + data_size;
                        }
                    }

                    keep = total_read < expected_request_size;
                }
                else
                {
                    no_response_count++;
                    keep = no_response_count < max_no_response_count;
                }
            }

            int result = std::min(total_read, HEADER_SIZE + data_size);
            if (result < expected_request_size)
            {
                result = 0;
            }
            return result;
        }

        int server_socket;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);

        bool init()
        {
            bool result = true;

            if ((this->server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
            {
                result = false;
                const std::string msg = this->identifier + " - Failed to create socket: " + std::string(strerror(errno));
                this->logger.error_msg(msg);
            }

            if (result && setsockopt(this->server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                     &opt, sizeof(opt)))
            {
                result = false;
                const std::string msg = "Failed to set server socket options: " + std::string(strerror(errno));
                this->logger.error_msg(msg);
            }
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(this->port);

            if (result && bind(this->server_socket, (struct sockaddr *)&address,
                               sizeof(address)) < 0)
            {
                result = false;
                const std::string msg = "Failed to bind the server socket: " + std::string(strerror(errno));
                this->logger.error_msg(msg);
            }
            if (result && listen(this->server_socket, 3) < 0)
            {
                result = false;
                const std::string msg = "Failed to make the server socket listen: " + std::string(strerror(errno));
                this->logger.error_msg(msg);
            }
            if (!result)
            {
                close(this->server_socket);
            }
            else
            {
                this->logger.debug_msg("successfuly initialized.");
            }
            return result;
        }

        bool process_accept_error(int error_code)
        {
            bool result = false;
            if (error_code == ENETDOWN ||
                error_code == EPROTO ||
                error_code == ENOPROTOOPT ||
                error_code == EHOSTDOWN ||
                error_code == ENONET ||
                error_code == EHOSTUNREACH ||
                error_code == EOPNOTSUPP ||
                error_code == ENETUNREACH)
            {

                result = true;

                const std::string msg = "Failed to accept socket: " + std::string(strerror(errno));
                this->logger.error_msg(msg);
            }
            else
            {
                close(this->server_socket);
                this->logger.error_msg("Bad error on accepting client socket. Server socket reinitialization required.");
            }
            return result;
        }

        bool clean_output_buffer(const int from, const int size)
        {
            bool result = false;
            if (from >= 0 && ((from + size) <= response_buffer_size))
            {
                memset(response_buffer + from, 0, size);
                result = true;
            }
            return result;
        }

        const char *get_request_buffer() const
        {
            return this->request_buffer;
        }

        /**
         * read-only for test purposes
         */
        const char *get_response_buffer() const
        {
            return this->response_buffer;
        }

        bool set_buffer_value(int from, int size, const void *data)
        {
            bool result = false;
            if (from >= 0 && ((from + size) <= response_buffer_size))
            {
                memcpy(response_buffer + from, data, size);
                result = true;
            }
            return result;
        }

    private:

        std::string identifier;

        const int port;

        int request_buffer_size = HEADER_SIZE;
        char *request_buffer;

        int response_buffer_size = HEADER_SIZE;
        char *response_buffer;

        int client_read_timeout_in_seconds;
        int client_write_timeout_in_seconds;

        bool keep_alive = false;

        const Logger logger;

        void set_client_timeout(int client_socket)
        {
            if (this->client_read_timeout_in_seconds > 0)
            {
                struct timeval tv;
                tv.tv_sec = this->client_read_timeout_in_seconds;
                tv.tv_usec = 0;
                setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);
            }
            if (this->client_write_timeout_in_seconds > 0)
            {
                struct timeval tv;
                tv.tv_sec = this->client_write_timeout_in_seconds;
                tv.tv_usec = 0;
                setsockopt(client_socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv, sizeof tv);
            }
        }
    };

} // namespace rpiasgige

#endif