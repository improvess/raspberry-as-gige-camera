#ifndef RPIASGIGE_CAMERA_USB_INTERFACE_HPP
#define RPIASGIGE_CAMERA_USB_INTERFACE_HPP

#include <map>

#include <opencv2/opencv.hpp>

#include "dumb_logger.hpp"

namespace rpiasgige
{

    class USB_Interface
    {

    public:

        USB_Interface() : logger("USB_Interface") {}
        virtual ~USB_Interface() {}

        bool open(const cv::String &camera_path)
        {
            bool result = false;

            if (!this->capture.isOpened()) {
                this->camera_path = camera_path;
                result = this->connect_to_device();
            }
            return result;
        }

        bool isOpened() {
            return this->capture.isOpened();
        }

        bool grab()
        {

            bool success = false;
            if (this->capture.isOpened())
            {
                success = this->capture.grab() && this->capture.retrieve(this->captured_image);
            }

            if (!success)
            {

                if (consecutive_misses >= MAX_CONSECUTIVE_MISSES)
                {
                    this->disconnect_device();
                }
                else
                {
                    consecutive_misses++;
                }
            }
            else
            {
                consecutive_misses = 0;
            }

            return success;
        }

        bool retrieve(cv::Mat &dest)
        {
            bool result = false;
            if (!this->captured_image.empty())
            {
                dest = this->captured_image;
                this->captured_image.release();
                result = true;
            }
            return result;
        }

        bool release()
        {

            return this->disconnect_device();
        }

        const cv::Mat &get_captured_image() const {
            return this->captured_image;
        }

        double get(int propId)
        {
            double result;

            if (this->props.find(propId) == this->props.end()) {
                result = this->capture.get(propId);
            } else {
                result = this->props[propId];
            }

            return result;
        }

        bool set(int propId, double value)
        {
            bool result = false;
            if (this->capture.set(propId, value)) {

                this->props[propId] = value;
                result = true;

                std::string msg = "SET " + std::to_string(propId) + " to " + std::to_string(value) + " worked";
                this->logger.debug_msg(msg.c_str());

            } else {
                std::string msg = "SET " + std::to_string(propId) + " to " + std::to_string(value) + " failed";
                this->logger.warn_msg(msg.c_str());
            }
            return result;
        }

    private:
        cv::String camera_path;
        cv::VideoCapture capture;
        cv::Mat captured_image;

        bool connect_to_device()
        {

            this->capture.open(this->camera_path);

            bool result = this->capture.isOpened();

            if (result)
            {
                std::map<int, double>::iterator it;
                for (it = this->props.begin(); it != this->props.end(); it++)
                {
                    this->capture.set(it->first, it->second);
                }
            }

            return result;
        }
        
        bool disconnect_device()
        {
            this->capture.release();
            this->captured_image.release();
            this->logger.debug_msg("device disconnected.");

            return true;
        }

        static const int MAX_CONSECUTIVE_MISSES = 3;
        int consecutive_misses = 0;

        std::map<int, double> props;

        const Logger logger;
    };

} // namespace rpiasgige

#endif