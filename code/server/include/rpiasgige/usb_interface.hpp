#ifndef RPIASGIGE_CAMERA_USB_INTERFACE_HPP
#define RPIASGIGE_CAMERA_USB_INTERFACE_HPP

#include <map>

#include <linux/types.h>
#include <linux/v4l2-common.h>
#include <linux/v4l2-controls.h>
#include <linux/videodev2.h>
#include <linux/media.h>
#include <sys/ioctl.h>

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include <opencv2/opencv.hpp>

#include "dumb_logger.hpp"

namespace rpiasgige
{

    class USB_Interface
    {

    public:

        USB_Interface() : logger("USB_Interface") {}
        virtual ~USB_Interface() {}

        void set_camera_path(const std::string &camera_path) {
            this->camera_path = camera_path;
        }

        void set_usb_bus_id(const std::string &usb_bus_id) {
            this->usb_bus_id = usb_bus_id;
        }

        bool open_camera()
        {
            bool result = false;

            if (!this->capture.isOpened()) {
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
        std::string camera_path;
        std::string usb_bus_id;

        cv::VideoCapture capture;
        cv::Mat captured_image;

        bool connect_to_device()
        {

            if (!this->camera_path.empty()) {
                this->capture.open(this->camera_path);
            } else if (!this->usb_bus_id.empty()) {
                auto path = this->resolve_usb_interface(this->usb_bus_id);
                if (!path.empty()) {
                    this->capture.open(path);
                }
            } else {
                return false;
            }

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

        inline std::string resolve_usb_interface(const std::string & target_usb_bus_id)
        {

            std::string result = "";

            std::vector<std::string> files;

            const std::string dev_folder = "/dev/";

            DIR *dir;
            struct dirent *ent;
            if ((dir = opendir(dev_folder.c_str())) != NULL)
            {
                while (result.empty() && (ent = readdir(dir)) != NULL)
                {
                    std::string file = dev_folder + ent->d_name;

                    const int fd = open(file.c_str(), O_RDWR);
                    v4l2_capability capability;
                    if (fd >= 0)
                    {
                        int err = ioctl(fd, VIDIOC_QUERYCAP, &capability);
                        if (err >= 0)
                        {
                            std::string bus_info;
                            struct media_device_info mdi;

                            err = ioctl(fd, MEDIA_IOC_DEVICE_INFO, &mdi);
                            if (!err)
                            {
                                if (mdi.bus_info[0])
                                    bus_info = mdi.bus_info;
                                else
                                    bus_info = std::string("platform:") + mdi.driver;
                            }
                            else
                            {
                                bus_info = reinterpret_cast<const char *>(capability.bus_info);
                            }

                            if (!bus_info.empty())
                            {   
                                if (bus_info.compare(target_usb_bus_id) == 0) {
                                    result = file;
                                }
                            }
                        }
                    }
                    close(fd);
                }
                closedir(dir);
            }
            else
            {
                std::string msg = "Cannot list " + dev_folder + " contents!";
                throw std::runtime_error(msg);
            }
            return result;
        }
    };

} // namespace rpiasgige

#endif