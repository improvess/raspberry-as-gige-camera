#ifndef RPIASGIGE_CONSTANTS_HPP
#define RPIASGIGE_CONSTANTS_HPP

namespace rpiasgige
{
    static const int STATUS_SIZE = 4;
    static const int KEEP_ALIVE_SIZE = 1;
    static const int DATA_SIZE = 4;
    static const int STATUS_ADDRESS = 0;
    static const int DATA_SIZE_ADDRESS = 5;
    static const int KEEP_ALIVE_ADDRESS = 4;
    static const int HEADER_SIZE = STATUS_SIZE + KEEP_ALIVE_SIZE + DATA_SIZE;
    static const int IMAGE_META_DATA_SIZE = 3 * sizeof(int);

}


#endif