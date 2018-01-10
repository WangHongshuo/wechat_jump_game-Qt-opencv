#include "jumpjump.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

JumpJump::JumpJump()
{

}

JumpJump::~JumpJump()
{

}

void JumpJump::setImage(cv::Mat &input)
{
    if(input.data)
    {
        inputImage = input.clone();
        mainTask();
        isLoadImage = true;
    }
    else
        isLoadImage = false;
}

void JumpJump::update()
{
    mainTask();
}

bool JumpJump::isLoadInputImage()
{
    return isLoadImage;
}

void JumpJump::mainTask()
{
    getEdge(inputImage,outputImage,cannyThreshold1,cannyThreshold2);
}

void JumpJump::getEdge(cv::Mat &src, cv::Mat &dst, double threshold1, double threshold2)
{
    if(src.channels() == 3)
        cv::cvtColor(src,dst,CV_RGB2GRAY);
    cv::Canny(src,dst,threshold1,threshold2);
}
