#include <chrono>

#include "rpi_as_gige/usb_interface.hpp"

int main(int argc, char **argv)
{

    const std::string keys =

        "{camera-path           | /dev/video0    | camera path such as /dev/video0         }"

        ;

    cv::CommandLineParser parser(argc, argv, keys);

    cv::String camera_path = parser.get<cv::String>("camera-path");

    rpiasgige::USB_Interface usb_camera;

    if (usb_camera.open(camera_path))
    {

        usb_camera.set(cv::CAP_PROP_BUFFERSIZE, 2);
        usb_camera.set(cv::CAP_PROP_FRAME_WIDTH, 320);
        usb_camera.set(cv::CAP_PROP_FRAME_HEIGHT, 240);
        usb_camera.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));

        int key = 0;

        std::chrono::high_resolution_clock::time_point begin_time_ref;
        std::chrono::high_resolution_clock::time_point end_time_ref;
        int frames_count = 1;

        while (key != 27)
        {
            usb_camera.grab();

            const auto &mat = usb_camera.get_captured_image();

            if (!mat.empty())
            {
                cv::imshow("mat", mat);
                key = cv::waitKey(10);
            }

            if (frames_count == 1)
            {
                begin_time_ref = std::chrono::high_resolution_clock::now();
                frames_count++;
            }
            else if (frames_count == 120)
            {
                end_time_ref = std::chrono::high_resolution_clock::now();
                std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_ref - begin_time_ref);
                auto time_spent = ms.count();
                if (time_spent > 100)
                {
                    float fps = frames_count * 1000.0 / time_spent;
                    std::cout << "fps: " << fps << "\n";
                }
                frames_count = 1;
            }
            else
                frames_count++;
        }

        usb_camera.release();
    } else {
        std::cerr << "Failed to open device at " << camera_path << "\n";
    }

    return 0;
}