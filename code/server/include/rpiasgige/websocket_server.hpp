#ifndef RPIASGIGE_WEBSOCKET_SERVER_HPP
#define RPIASGIGE_WEBSOCKET_SERVER_HPP

#include <string.h>

#include <thread>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

#include "rpiasgige/dumb_logger.hpp"

#include "rpiasgige/constants.hpp"

namespace rpiasgige
{

    class Websocket_Server
    {

    public:

        Websocket_Server(const std::string &_identifier, const std::string &_server_address, int _port, int _max_response_buffer_size) : 
            identifier(_identifier), server_address(_server_address), port(_port), max_response_buffer_size(_max_response_buffer_size), logger(_identifier){}

        virtual ~Websocket_Server()
        {
        }

        void run()
        {
            bool initialized = false;

            while (true)
            {
                if (!initialized)
                {

                    this->logger.debug_msg("Websocket_Server not initialized. Initializing now.");
                    initialized = init();
                }
                else
                {

                    auto const address = net::ip::make_address(this->server_address);
                    tcp::acceptor acceptor{ioc, {address, this->port}};

                    this->logger.debug_msg("Websocket_Server waiting for client");
                    tcp::socket socket{this->ioc};

                    acceptor.accept(socket);

                    std::thread{std::bind(
                        &Websocket_Server::process_client, this,
                        client_index++,
                        std::move(socket))}.detach();

                }
            }
        }

    protected:

        const std::string &get_identifier() const {
            return this->identifier;
        }

        bool init()
        {
            bool result = true;

            this->logger.debug_msg("successfuly initialized.");
            return result;
        }

        virtual void prepare_response(const char * request_buffer, const int request_size, char * response_buffer, int &response_size) = 0;

        int max_response_buffer_size;

        bool set_buffer_value(char * buffer, int from, int size, const void *data)
        {
            bool result = false;
            if (from >= 0 && ((from + size) <= max_response_buffer_size))
            {
                memcpy(buffer + from, data, size);
                result = true;
            }
            return result;
        }

        void set_status(char * buffer, const char *status)
        {
            memcpy(buffer + rpiasgige::STATUS_ADDRESS, status, rpiasgige::STATUS_SIZE);
        }

        void set_response_data_size(char * buffer, int size)
        {
            memcpy(buffer + rpiasgige::DATA_SIZE_ADDRESS, &size, sizeof size);
        }

    private:

        std::string identifier;

        short unsigned int port;
        std::string server_address;

        net::io_context ioc{1};

        const Logger logger;

        int client_index = 0;

        void respond_to_client(int thread_id, websocket::stream<tcp::socket> &ws, const beast::flat_buffer &request_buffer, char * response_buffer) {
            
            std::size_t request_size = request_buffer.size();
            const char* request_data = boost::asio::buffer_cast<const char*>(request_buffer.data());
   
            memset(response_buffer, 0, this->max_response_buffer_size);
            int response_size;
            prepare_response(request_data, request_size, response_buffer, response_size);

            //std::cout << response_size << ": " << response_buffer << "\n";

            ws.text(false);
            ws.binary(true);
            ws.write(net::buffer(response_buffer, response_size));

        }

        void process_client(int thread_id, tcp::socket& socket) {

            char * response_buffer = new char[this->max_response_buffer_size];

            try {
                
                websocket::stream<tcp::socket> ws{std::move(socket)};

                ws.accept();

                while(true) {

                    beast::flat_buffer request_buffer;

                    ws.read(request_buffer);

                    respond_to_client(thread_id, ws, request_buffer, response_buffer);

                }

            } catch(beast::system_error const& se) {
                if(se.code() != websocket::error::closed) {
                    std::string msg = "Thread-" + std::to_string(thread_id) + ": Error: " + se.code().message();
                    this->logger.error_msg(msg);
                }
            } catch(std::exception const& e) {
                std::string msg = "Thread-" + std::to_string(thread_id) + ": Error: " + e.what();
                this->logger.error_msg(msg);
            }
            
            delete [] response_buffer;

        }

    };

} // namespace rpiasgige

#endif