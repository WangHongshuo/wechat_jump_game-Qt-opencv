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

void JumpJump::mainTask()
{
    outputImage = inputImage;
    roiImage = outputImage(cv::Rect(int(0.2*inputImageWidth),int(0.3*inputImageHeight),
                                    int(0.6*inputImageWidth),int(0.4*inputImageHeight)));
    getEdge(inputImage,edgeImage,cannyThreshold1,cannyThreshold2);
    findTemplateLocation(roiImage,roiImage,templateImage);
}

void JumpJump::getEdge(cv::Mat &src, cv::Mat &dst, double threshold1, double threshold2)
{
    if(src.channels() == 3)
        cv::cvtColor(src,dst,CV_RGB2GRAY);
    cv::Canny(src,dst,threshold1,threshold2);
}

void JumpJump::findTemplateLocation(cv::Mat &src, cv::Mat &dst, const cv::Mat &target)
{
    cv::Mat matchResult;
    cv::matchTemplate(src,target,matchResult,cv::TM_CCORR_NORMED);
    cv::Point maxValueLoaction;
    cv::minMaxLoc(matchResult,NULL,NULL,NULL,&maxValueLoaction);
    maxValueLoaction.x += int(double(target.rows)/2);
    maxValueLoaction.y += int(double(target.cols)/2)+
                          int(double(target.cols)*2.5);
    cv::circle(dst,maxValueLoaction,2,cv::Scalar(0,255,0),4);
}

