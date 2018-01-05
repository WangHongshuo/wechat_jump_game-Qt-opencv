#ifndef JUMPJUMP_H
#define JUMPJUMP_H
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

class JumpJump
{
public:
    JumpJump();
    ~JumpJump();
    void setImage(cv::Mat &input);
    cv::Mat outputImage;
    void update();
    double cannyThreshold1 = 127.0;
    double cannyThreshold2 = 255.0;

private:
    void mainTask();
    void getEdge(cv::Mat &src,cv::Mat &dst, double threshold1, double threshold2);
    cv::Mat inputImage;

};

#endif // JUMPJUMP_H
