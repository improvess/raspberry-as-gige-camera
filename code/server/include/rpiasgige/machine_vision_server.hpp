#ifndef RPIASGIGE_TCP_SERVER_INTERFACE_HPP
#define RPIASGIGE_TCP_SERVER_INTERFACE_HPP

#include <mutex> 
#include <chrono>

#include "usb_interface.hpp"
#include "websocket_server.hpp"

namespace rpiasgige
{

    class Server : public Websocket_Server
        {

        public:
            Server(const std::string & identifier, const std::string &server_address, int _port, USB_Interface &_usb_camera, const int max_image_size_in_bytes) : 
            Websocket_Server(identifier, server_address , _port, max_image_size_in_bytes), usb_camera(_usb_camera) {}
            virtual ~Server() {}

            static const int IMAGE_META_DATA_SIZE = 3 * sizeof(int);

            bool set_camera_timeout_in_milliseconds(const int val) {
                bool result = false;
                if (val > 0) {
                    this->usb_camera_mutex_timeout = std::chrono::milliseconds(val);
                    result = true;
                }
                return result;
            }

            bool set_camera_open_timeout_in_milliseconds(const int val) {
                bool result = false;
                if (val > 0) {
                    this->usb_camera_open_mutex_timeout = std::chrono::milliseconds(val);
                    result = true;
                }
                return result;
            }

        protected:

            virtual void prepare_response(const char * request_buffer, const int request_size, char * response_buffer, int &response_size)
            {

                response_size = HEADER_SIZE;

                response_buffer[KEEP_ALIVE_ADDRESS] = request_buffer[KEEP_ALIVE_ADDRESS];

                bool camera_timeout = false;
                if (strncmp("GRAB", request_buffer, STATUS_SIZE) == 0) {

                    if(usb_camera_mutex.try_lock_for(this->usb_camera_mutex_timeout)) {
                        this->usb_camera.grab();
                        const cv::Mat &mat = this->usb_camera.get_captured_image();
                        int image_size = 0;

                        if (!mat.empty()) {
                            image_size = mat.total() * mat.elemSize();
                            const char *mat_data = (const char *)mat.data;
                            int size_int = sizeof(int);
                            const int metada_data_size = 3*size_int;
                            this->set_buffer_value(response_buffer, HEADER_SIZE, size_int, &mat.rows);
                            this->set_buffer_value(response_buffer, HEADER_SIZE + size_int, size_int, &mat.cols);
                            int type = mat.type();
                            this->set_buffer_value(response_buffer, HEADER_SIZE + 2*size_int, size_int, &type);
                            this->set_buffer_value(response_buffer, HEADER_SIZE + metada_data_size, image_size, mat_data);
                            this->set_status(response_buffer, "0200");

                            response_size = HEADER_SIZE + image_size + metada_data_size;

                            set_response_data_size(response_buffer, image_size + metada_data_size);
                        } else {
                            this->set_status(response_buffer, "NOPE");
                        }
                    } else {
                        camera_timeout = true;
                    }
        
                }
                else if (strncmp("SET0", request_buffer, STATUS_SIZE) == 0) {
                    int data_size = request_size - HEADER_SIZE;
                    if (data_size >= 12) {

                        int propId;
                        double value;

                        memcpy(&propId, request_buffer + HEADER_SIZE, sizeof(int));
                        memcpy(&value, request_buffer + HEADER_SIZE + sizeof(int), sizeof(double));

                        bool result = false;
                        if(usb_camera_mutex.try_lock_for(this->usb_camera_mutex_timeout)) {
                            result = this->usb_camera.set(propId, value);
                        } else {
                            camera_timeout = true;
                        }
                        if (!camera_timeout) {
                            if (result) {
                                this->set_status(response_buffer, "0200");
                            } else {
                                this->set_status(response_buffer, "NOPE");
                            }
                        }

                    } else {
                        this->set_status(response_buffer, "0400");
                    }

                } else if (strncmp("OPEN", request_buffer, STATUS_SIZE) == 0) {
                    bool result = false;
                    if(usb_camera_mutex.try_lock_for(this->usb_camera_open_mutex_timeout)) {
                        result = this->usb_camera.open_camera();
                    } else {
                        camera_timeout = true;
                    }
                    if (!camera_timeout) {

                        if (result) {
                            this->set_status(response_buffer, "0200");
                        } else {
                            this->set_status(response_buffer, "NOPE");
                        }
                    }

                } else if (strncmp("CLOS", request_buffer, STATUS_SIZE) == 0) {
                    bool result = false;
                    if(usb_camera_mutex.try_lock_for(this->usb_camera_mutex_timeout)) {
                        result = this->usb_camera.release();
                    } else {
                        camera_timeout = true;
                    }

                    if (!camera_timeout) {

                        if (result) {
                            this->set_status(response_buffer, "0200");
                        } else {
                            this->set_status(response_buffer, "NOPE");
                        }
                    }

                } else if (strncmp("GET0", request_buffer, STATUS_SIZE) == 0) {

                    int propId;

                    memcpy(&propId, request_buffer + HEADER_SIZE, sizeof(int));
                    double value = -1;
                    if(usb_camera_mutex.try_lock_for(this->usb_camera_mutex_timeout)) {
                        value = this->usb_camera.get(propId);
                    } else {
                        camera_timeout = true;
                    }
                    if (!camera_timeout) {

                        this->set_buffer_value(response_buffer, HEADER_SIZE, SIZE_OF_DOUBLE, &value);

                        this->set_response_data_size(response_buffer, SIZE_OF_DOUBLE);

                        response_size = HEADER_SIZE + sizeof(double);
                        this->set_status(response_buffer, "0200");
                    }

                } else if (strncmp("ISOP", request_buffer, STATUS_SIZE) == 0) {
                    bool result = false;
                    if(usb_camera_mutex.try_lock_for(this->usb_camera_mutex_timeout)) {
                        result = this->usb_camera.isOpened();
                    } else {
                        camera_timeout = true;
                    }
                    if (!camera_timeout) {

                        if (result) {
                            this->set_status(response_buffer, "0200");
                        } else {
                            this->set_status(response_buffer, "NOPE");
                        }
                    }

                } else if (strncmp("PING", request_buffer, STATUS_SIZE) == 0) {
                    this->set_status(response_buffer, "PONG");
                } else {
                    this->set_status(response_buffer, "0404");
                }
                if (camera_timeout) {
                    this->set_status(response_buffer, "TIME");
                }
            }

        private:
            USB_Interface &usb_camera;
            std::timed_mutex usb_camera_mutex;
            std::chrono::milliseconds usb_camera_mutex_timeout = std::chrono::milliseconds(200);
            std::chrono::milliseconds usb_camera_open_mutex_timeout = std::chrono::milliseconds(1000);
            static const int SIZE_OF_DOUBLE = sizeof(double);
            static const int SET_DATA_SIZE = sizeof(int) + SIZE_OF_DOUBLE;
        };


}



#endif