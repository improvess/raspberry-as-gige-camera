#ifndef RPIASGIGE_CLIENT_API_HPP
#define RPIASGIGE_CLIENT_API_HPP

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace beast = boost::beast;        
namespace http = beast::http;
namespace websocket = beast::websocket; 
namespace net = boost::asio;          
using tcp = boost::asio::ip::tcp; 

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
            websocket::stream<tcp::socket> *ws = nullptr;
            net::io_context ioc;

            int timeout_count = 0;
            const int MAX_TIMEOUT_COUNT = 2;
            int read_timeout_in_seconds = 1;

            void handle_timeout(const std::string &origin, TimeoutException &tex)
            {

                if (timeout_count < MAX_TIMEOUT_COUNT)
                {
                    timeout_count++;
                }
                else
                {
                    timeout_count = 0;
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

                bool test = true;
                if (this->is_connected()) {
                    test = this->disconnect();
                }

                if (!test) {
                    return false;
                }

                tcp::resolver resolver{ioc};
                if (this->ws == nullptr) {
                    this->ws = new websocket::stream<tcp::socket>{ioc};
                }

                auto host = this->address.c_str();
                auto s_port = std::to_string(this->port).c_str();

                auto const results = resolver.resolve(host, s_port);

                net::connect(ws->next_layer(), results.begin(), results.end());

                ws->set_option(websocket::stream_base::decorator(
                    [](websocket::request_type& req)
                    {
                        req.set(http::field::user_agent,
                            std::string(BOOST_BEAST_VERSION_STRING) +
                                " websocket-client-coro");
                    }));

                ws->handshake(host, "/");

                return this->is_connected();
            }

            void update_socket_timeout()
            {
                
            }

            int response_buffer_size = HEADER_SIZE;
            char *response_buffer = nullptr;

            int request_buffer_size = HEADER_SIZE;
            char *request_buffer = nullptr;

            inline bool is_connected() const
            {
                return this->ws != nullptr && this->ws->is_open();
            }

            bool disconnect()
            {
                bool result = true;
                if (this->ws != nullptr)
                {
                    try {
                        this->ws->close(websocket::close_code::normal);
                    } catch(std::exception const& e) {
                        result = false;
                        std::cerr << "Failed to close websocket: " << e.what() << "\n";
                    }
                    
                }
                return result;
            }

            /* *
             * Load the response into the response parameter
             * */
            bool read_response(Packet &response)
            {

                beast::flat_buffer buffer;

                int bytes_read = this->ws->read(buffer);

                const char* response_data = boost::asio::buffer_cast<const char*>(buffer.data());

                memcpy(this->response_buffer, response_data, std::min(bytes_read, this->response_buffer_size));

                int expected_data_size;
                int expected_data_size_sz = sizeof(expected_data_size);
                bool result = false;
                response.data_size = 0;
                if (bytes_read >= (DATA_SIZE_ADDRESS + expected_data_size_sz))
                {
                    response.data_size = 0;
                    memcpy(&expected_data_size, this->response_buffer + DATA_SIZE_ADDRESS, expected_data_size_sz);
                    if (bytes_read - HEADER_SIZE >= expected_data_size)
                    {
                        response.data_size = expected_data_size;
                    }
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

                this->ws->binary(true);
                this->ws->text(false);

                this->ws->write(net::buffer(this->request_buffer, bytes_to_send));

                return true;
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