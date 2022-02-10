#ifndef RPIASGIGE_WEBSOCKET_SERVER_HPP
#define RPIASGIGE_WEBSOCKET_SERVER_HPP

#include <string.h>

#include "rpiasgige/dumb_logger.hpp"

#include "rpiasgige/constants.hpp"

namespace rpiasgige
{

    class Websocket_Server
    {

    public:

        Websocket_Server(const std::string &_identifier, int _max_response_buffer_size) : 
            identifier(_identifier), max_response_buffer_size(_max_response_buffer_size), logger(_identifier){}

        virtual ~Websocket_Server() { }

        bool init() {
            this->online = true;
            return this->online;
        }

        void process_client(const char* request_buffer, const int request_size, char * response_buffer, int &response_size) {
            memset(response_buffer, 0, this->max_response_buffer_size);
            prepare_response(request_buffer, request_size, response_buffer, response_size);
        }

        int get_max_response_buffer_size() {
            return this->max_response_buffer_size;
        }

        bool is_online() {
            return this->online;
        }

        bool stop() {
            this->online = false;
            return true;
        }

    protected:

        const int max_response_buffer_size;

        const Logger logger;

        virtual void prepare_response(const char * request_buffer, const int request_size, char * response_buffer, int &response_size) = 0;

        const std::string &get_identifier() const {
            return this->identifier;
        }

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

        inline void set_status(char * buffer, const char *status)
        {
            memcpy(buffer + rpiasgige::STATUS_ADDRESS, status, rpiasgige::STATUS_SIZE);
        }

        inline void set_response_data_size(char * buffer, int size)
        {
            memcpy(buffer + rpiasgige::DATA_SIZE_ADDRESS, &size, sizeof size);
        }

    private:

        std::string identifier;

        bool online = false;

    };

} // namespace rpiasgige

#endif