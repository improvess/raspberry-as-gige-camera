
#include "gtest/gtest.h"

#include "rpi_as_gige/usb_interface.hpp"

class USB_InterfaceTest : public ::testing::Test
{
    public: 
    const std::string device_path = "../samples/sample_1280x720.mp4";
};

TEST_F(USB_InterfaceTest, OpenCloseTest)
{

    rpiasgige::USB_Interface device;

    ASSERT_FALSE(device.isOpened());

    EXPECT_TRUE(device.open(USB_InterfaceTest::device_path));

    EXPECT_TRUE(device.isOpened());

    ASSERT_TRUE(device.release());

}

TEST_F(USB_InterfaceTest, GrabTest)
{

    rpiasgige::USB_Interface device;

    ASSERT_TRUE(device.open(USB_InterfaceTest::device_path));

    ASSERT_TRUE(device.isOpened());

    EXPECT_TRUE(device.grab());

    cv::Mat mat;

    EXPECT_TRUE(device.retrieve(mat));

    EXPECT_FALSE(mat.empty());

    const auto size = mat.size();

    EXPECT_EQ(size.width, 1280) << "Wrong size width";

    EXPECT_EQ(size.height, 720) << "Wrong size height";

    ASSERT_TRUE(device.release());

}

TEST_F(USB_InterfaceTest, GetSetTest)
{

    rpiasgige::USB_Interface device;

    ASSERT_FALSE(device.isOpened());

    ASSERT_FALSE(device.set(cv::CAP_PROP_POS_FRAMES, 2));

    ASSERT_TRUE(device.open(USB_InterfaceTest::device_path));

    EXPECT_TRUE(device.set(cv::CAP_PROP_POS_FRAMES, 2));

    ASSERT_TRUE(device.release());

}