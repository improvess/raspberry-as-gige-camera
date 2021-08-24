#include "rpi_as_gige/usb_interface.hpp"

namespace rpiasgige
{
    
    bool USB_Interface::open(const cv::String &camera_path)
    {
        bool result = false;

        if (!this->capture.isOpened()) {
            this->camera_path = camera_path;
            result = this->connect_to_device();
        }
        return result;
    }

    bool USB_Interface::isOpened() {
        return this->capture.isOpened();
    }

    bool USB_Interface::grab()
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

    bool USB_Interface::retrieve(cv::Mat &dest)
    {
        bool result = false;
        if (!this->captured_image.empty())
        {
            dest = this->captured_image;
            this->captured_image.release();
        }
        return result;
    }

    bool USB_Interface::release()
    {

        return this->disconnect_device();
    }

    bool USB_Interface::connect_to_device()
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

    bool USB_Interface::disconnect_device()
    {
        this->capture.release();
        this->captured_image.release();
        std::cout << "device disconnected\n";

        return true;
    }

    bool USB_Interface::set(int propId, double value)
    {
        bool result = false;
        if (this->capture.set(propId, value)) {
            std::cout << "SET " << propId << " to " << value << " worked\n";
            this->props[propId] = value;
            result = true;
        } else {
            std::cout << "SET " << propId << " to " << value << " failed\n";
        }
        return result;
    }

    double USB_Interface::get(int propId)
    {
        return this->capture.get(propId);
    }

} // namespace rpiasgige