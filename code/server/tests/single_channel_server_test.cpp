
#include "gtest/gtest.h"

#include "rpiasgige/single_channel_server.hpp"

static const int TEST_DATA_SIZE = 3 * 1280 * 1024;

class Single_Channel_ServerTest : public ::testing::Test
{
};

/**
 * This class aims to expose the protected methods in order we can test them.
 **/
class Single_Channel_Server_Wrapper : public rpiasgige::Single_Channel_Server
{

public:
    Single_Channel_Server_Wrapper() : 
        rpiasgige::Single_Channel_Server("Single_Channel_Server_Wrapper", 4242, HEADER_SIZE, HEADER_SIZE + TEST_DATA_SIZE) {}
    virtual ~Single_Channel_Server_Wrapper() {};

    const char *get_response_buffer_wrapper() const
    {
        return this->get_response_buffer();
    }

    bool set_buffer_value_wrapper(int from, int size, const void *data)
    {
        return this->set_buffer_value(from, size, data);
    }

    bool clean_output_buffer_wrapper(const int from, const int size)
    {
        return this->clean_output_buffer(from, size);
    }

    void set_status_wrapper(const char *status)
    {
        this->set_status(status);
    }

    void set_response_data_size_wrapper(int size)
    {
        this->set_response_data_size(size);
    }

protected:
    void respond_to_client(int &client_socket, const int data_size)
    {

    }
};

TEST_F(Single_Channel_ServerTest, set_buffer_value_Test)
{

    char data[TEST_DATA_SIZE];

    int char_size = sizeof(char);

    for(int i = 0; i < TEST_DATA_SIZE; ++i) {
        data[i] = (i % char_size) + 1;
    }

    Single_Channel_Server_Wrapper server;

    ASSERT_TRUE(server.set_buffer_value_wrapper(rpiasgige::Single_Channel_Server::HEADER_SIZE, TEST_DATA_SIZE, data));

    const char * data_ro = server.get_response_buffer_wrapper() + rpiasgige::Single_Channel_Server::HEADER_SIZE;

    for(int i = 0; i < TEST_DATA_SIZE; ++i) {
        char expected = (i % char_size) + 1;
        EXPECT_EQ(data_ro[i], expected) << "Expected to get " << expected << " but got " << data_ro[i] << " at index " << i;
    }
    
}

TEST_F(Single_Channel_ServerTest, clean_output_buffer_Test)
{

    char data[TEST_DATA_SIZE];

    int char_size = sizeof(char);

    for(int i = 0; i < TEST_DATA_SIZE; ++i) {
        data[i] = (i % char_size) + 1;
    }

    Single_Channel_Server_Wrapper server;

    server.set_buffer_value_wrapper(rpiasgige::Single_Channel_Server::HEADER_SIZE, TEST_DATA_SIZE, data);

    const char * data_ro = server.get_response_buffer_wrapper() + rpiasgige::Single_Channel_Server::HEADER_SIZE;

    for(int i = 0; i < TEST_DATA_SIZE; ++i) {
        int expected = (i % char_size) + 1;
        ASSERT_EQ(data_ro[i], expected) << "Expected to get " << expected << " but got " << data_ro[i] << " at index " << i;
    }

    ASSERT_TRUE(server.clean_output_buffer_wrapper(rpiasgige::Single_Channel_Server::HEADER_SIZE, TEST_DATA_SIZE));

    for(int i = 0; i < TEST_DATA_SIZE; ++i) {
        EXPECT_EQ(data_ro[i], 0) << "Expected to get 0 but got " << data_ro[i] << " at index " << i;
    }
    
}

TEST_F(Single_Channel_ServerTest, set_status_Test)
{

    char status[rpiasgige::Single_Channel_Server::STATUS_SIZE];

    int char_size = sizeof(char);

    for(int i = 0; i < rpiasgige::Single_Channel_Server::STATUS_SIZE; ++i) {
        status[i] = (i % char_size) + 1;
    }

    Single_Channel_Server_Wrapper server;

    server.set_status_wrapper(status);

    const char *response_buffer_ro = server.get_response_buffer_wrapper();

    for(int i = 0; i < rpiasgige::Single_Channel_Server::STATUS_SIZE; ++i) {
        int expected = (i % char_size) + 1;
        EXPECT_EQ(status[i], (i % char_size) + 1);
    }
    
}

TEST_F(Single_Channel_ServerTest, set_response_data_size_Test)
{

    Single_Channel_Server_Wrapper server;

    server.set_response_data_size_wrapper(TEST_DATA_SIZE);

    const char *response_buffer_ro = server.get_response_buffer_wrapper();

    int data_size_copy;

    memcpy(&data_size_copy, response_buffer_ro + rpiasgige::Single_Channel_Server::DATA_SIZE_ADDRESS, sizeof(int));

    EXPECT_EQ(TEST_DATA_SIZE, data_size_copy) << "Expected to get " << TEST_DATA_SIZE << " but got " << data_size_copy;
    
}