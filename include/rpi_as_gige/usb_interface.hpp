#ifndef RPIASGIGE_CAMERA_USB_INTERFACE_HPP
#define RPIASGIGE_CAMERA_USB_INTERFACE_HPP

#include <map>

#include <opencv2/opencv.hpp>

namespace rpiasgige
{

    class USB_Interface
    {

    public:

        USB_Interface() {}

        bool set(int propId, double value);
        double get(int propId);
        bool open(const cv::String &camera_path);
        bool isOpened();
        bool grab();
        bool retrieve(cv::Mat &dest);
        bool release();

        const cv::Mat &get_captured_image() const {
            return this->captured_image;
        }

    private:
        cv::String camera_path;
        cv::VideoCapture capture;
        cv::Mat captured_image;

        bool connect_to_device();
        bool disconnect_device();

        static const int MAX_CONSECUTIVE_MISSES = 3;
        int consecutive_misses = 0;

        std::map<int, double> props;
    };

} // namespace rpiasgige

#endif