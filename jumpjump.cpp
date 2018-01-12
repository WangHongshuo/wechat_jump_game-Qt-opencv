#include "jumpjump.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <QDebug>
#include <iostream>

JumpJump::JumpJump()
{

}

JumpJump::~JumpJump()
{

}

void JumpJump::setInputImageAndTemplateImage(cv::Mat &inputMat, cv::Mat &templateMat)
{
    if(inputMat.data && templateMat.data)
    {
        if(inputMat.channels() == 4)
            cv::cvtColor(inputMat,inputImage,CV_RGBA2RGB);
        else
            inputImage = inputMat.clone();
        if(templateMat.channels() == 4)
            cv::cvtColor(templateMat,templateImage,CV_RGBA2RGB);
        else
            templateImage = templateMat.clone();
        isLoadImage = true;
        inputImageWidth = inputImage.cols;
        inputImageHeight = inputImage.rows;
        mainTask();

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

int JumpJump::manLocationX()
{
    return  manLocation.x;
}

int JumpJump::manLocationY()
{
    return  manLocation.y;
}

cv::Point JumpJump::manLocationPoint()
{
    return manLocation;
}

void JumpJump::mainTask()
{
    outputImage = inputImage;
    roiImage = outputImage(cv::Rect(int(0.2*(double)inputImageWidth),int(0.3*(double)inputImageHeight),
                                    int(0.6*(double)inputImageWidth),int(0.4*(double)inputImageHeight)));
    getEdge(inputImage,edgeImage,cannyThreshold1,cannyThreshold2);
    findTemplateLocation(roiImage,roiImage,templateImage,manLocation);
    manLocation.x += int(0.2*(double)inputImageWidth);
    manLocation.y += int(0.3*(double)inputImageHeight);
}

void JumpJump::getEdge(cv::Mat &src, cv::Mat &dst, double threshold1, double threshold2)
{
    if(src.channels() == 3)
        cv::cvtColor(src,dst,CV_RGB2GRAY);
    cv::Canny(src,dst,threshold1,threshold2);
}

void JumpJump::findTemplateLocation(cv::Mat &src, cv::Mat &dst, const cv::Mat &target, cv::Point &location)
{
    cv::Mat matchResult;
    cv::matchTemplate(src,target,matchResult,cv::TM_CCORR_NORMED);
    cv::Point maxValueLoaction;
    cv::minMaxLoc(matchResult,NULL,NULL,NULL,&maxValueLoaction);
    maxValueLoaction.x += int(double(target.cols)/2);
    maxValueLoaction.y += int(double(target.rows)/2)+
                          int(double(target.rows)*2.6);
    cv::circle(dst,maxValueLoaction,2,cv::Scalar(0,255,0),4);
    location = maxValueLoaction;
}

