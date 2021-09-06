#ifndef RPIASGIGE_CLIENT_API_HPP
#define RPIASGIGE_CLIENT_API_HPP

#include <chrono>

#include <opencv2/opencv.hpp>

namespace rpiasgige
{

    namespace client
    {

        struct TimeoutException : std::runtime_error
        {
            TimeoutException(const std::string &msg) : std::runtime_error(msg) {}
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

            virtual ~Device() {}

            bool set(int propId, double value, bool keep_alive = false);
            double get(int propId, bool keep_alive = false);
            bool open(bool keep_alive = false);
            bool isOpened(bool keep_alive = false);
            bool retrieve(cv::Mat &dest, bool keep_alive = false);
            bool release(bool keep_alive = false);
            bool ping(bool keep_alive = false);

            void set_read_timeout(int timeout_in_seconds);

        private:
            cv::String address;
            int port;
            int timeout_count = 0;
            const int MAX_TIMEOUT_COUNT = 5;
            int read_timeout_in_seconds = 0;

            void handle_timeout(TimeoutException &tex);

            void send_request(const Packet &request, Packet &response);

            bool open_tcp_conversation();

            void update_socket_timeout();

            int server_socket = -1;

            int response_buffer_size = HEADER_SIZE;
            char *response_buffer;

            int request_buffer_size = HEADER_SIZE;
            char *request_buffer;

            inline bool is_connected() const
            {
                return server_socket >= 0;
            }

            void disconnect();

            /* *
             * Load the response into the response parameter
             * */
            bool read_response(Packet &response);

            /**
             * loads into buffer a chunk of data from the response packet
             **/
            void read_response_data(void *buffer, const int size, const Packet &response);

            /* *
             * helper method to send request
             * */
            bool send_request_buffer(const int bytes_to_send);

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