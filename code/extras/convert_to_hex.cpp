
#include <stdio.h>

#include <opencv2/opencv.hpp>

int main(int argc, char ** argv)
{

    int propId = cv::CAP_PROP_FOCUS;
    double value = 0;

    char propIdData[4];
    char valueData[8];

    memcpy(propIdData, &propId, sizeof(int));
    memcpy(valueData, &value, sizeof(double));

    for(char v : propIdData) {
        printf("%d\n", v);
    }
    printf("--------------\n");
    for(char v : valueData) {
        printf("%d\n", v);
    }

    return 0;
}