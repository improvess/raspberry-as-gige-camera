#include <chrono>

#include <opencv2/opencv.hpp>

/**
 * A basic opencv camera grabber for sanity testing
 **/

int main(int argc, char **argv)
{

    const std::string keys =

        "{camera-path           | /dev/video0    | camera path such as /dev/video0         }"
        "{frame-width           | 640    | camera frame width         }"
        "{frame-height           | 640    | camera frame height         }"
        "{fps           | 30    | camera fps         }"
        "{codec           | mjpg    | video codec        }"
        "{auto-focus           | off    | auto-focus on/off       }"

        ;

    cv::CommandLineParser parser(argc, argv, keys);

    cv::String camera_path = parser.get<cv::String>("camera-path");

    cv::VideoCapture usb_camera;

    if (usb_camera.open(camera_path))
    {

        double WIDTH = parser.get<double>("frame-width");
        double HEIGHT = parser.get<double>("frame-height");
        double FPS = parser.get<double>("fps");

        usb_camera.set(cv::CAP_PROP_FRAME_WIDTH, WIDTH);
        usb_camera.set(cv::CAP_PROP_FRAME_HEIGHT, HEIGHT);
        usb_camera.set(cv::CAP_PROP_FPS, FPS);


        std::cout << "Resolution set to " << usb_camera.get(cv::CAP_PROP_FRAME_WIDTH) << "x" << usb_camera.get(cv::CAP_PROP_FRAME_HEIGHT) << "\n";

        std::cout << "FPS set to " << usb_camera.get(cv::CAP_PROP_FPS) << "\n";

        cv::String codec = parser.get<cv::String>("codec");

        if (codec.length() >= 4) {
            if (usb_camera.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc(codec[0], codec[1], codec[2], codec[3]))) {
                std::cout << "CODEC set to " << codec.substr(0, 5) << "\n";
            }
        }

        cv::String autofocus = parser.get<cv::String>("auto-focus");

        if (autofocus.compare("on") == 0) {
            if (usb_camera.set(cv::CAP_PROP_AUTOFOCUS, 1)) {
                std::cout << "AUTO FOCUS set to ON\n";
            } else {
                usb_camera.set(cv::CAP_PROP_AUTOFOCUS, 0);
            }
        }

        int key = 0;

        std::chrono::high_resolution_clock::time_point begin_time_ref;
        std::chrono::high_resolution_clock::time_point end_time_ref;
        int frames_count = 1;

        while (key != 27)
        {
            usb_camera.grab();

            cv::Mat mat;
            usb_camera.retrieve(mat);

            if (!mat.empty())
            {
                cv::imshow("mat", mat);
                key = cv::waitKey(1);
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