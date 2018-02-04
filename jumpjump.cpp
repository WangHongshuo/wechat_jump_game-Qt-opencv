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

void JumpJump::setInputImage(cv::Mat &inputMat)
{
    if(inputMat.data && isLoadTemplateImage)
    {
        if(inputMat.channels() == 4)
            cv::cvtColor(inputMat,inputImage,CV_RGBA2RGB);
        else
            inputImage = inputMat.clone();
        isLoadImage = true;
        inputImageWidth = inputImage.cols;
        inputImageHeight = inputImage.rows;
        isDrawPointAreaCopied = false;
        mainTask();

    }
    else
        isLoadImage = false;
}

void JumpJump::setTemplateImage(cv::Mat &templateImg)
{
    if(templateImg.channels() == 4)
        cv::cvtColor(templateImg,templateImage,CV_RGBA2RGB);
    else
        templateImage = templateImg.clone();
    isLoadTemplateImage = true;
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
    return  manPos.x;
}

int JumpJump::manLocationY()
{
    return  manPos.y;
}

cv::Point JumpJump::manLocationPoint()
{
    return manPos;
}

void JumpJump::setLeftClickedPos(int x, int y)
{
    if(x >= 0 && y >= 0 && isLoadImage)
    {
        leftClickedPosX = x;
        leftClickedPosY = y;
        // 修正要拷贝的画图区域
        int x1,y1,x2,y2;
        x1 = std::min(x,manLocationX())-3;
        y1 = std::min(y,manLocationY())-3;
        x2 = std::max(x,manLocationX())+3;
        y2 = std::max(y,manLocationY())+3;
        if(x1 < 0)
            x1 = 0;
        if(y1 < 0)
            y1 = 0;
        if(x2 > inputImage.cols)
            x2 = inputImage.cols;
        if(y2 > inputImage.rows)
            y2 = inputImage.rows;

        if(!isDrawPointAreaCopied)
        {
            cv::Mat roiTemp = outputImage(cv::Rect(x1,y1,x2-x1+1,y2-y1+1));
            copyOfDrawPointArea = roiTemp.clone();
            isDrawPointAreaCopied = true;
        }
        else
        {
            cv::Mat roiTemp = outputImage(cv::Rect(lastAreaX1,lastAreaY1,lastAreaX2-lastAreaX1+1,lastAreaY2-lastAreaY1+1));
            copyOfDrawPointArea.copyTo(roiTemp);
            roiTemp = outputImage(cv::Rect(x1,y1,x2-x1+1,y2-y1+1));
            copyOfDrawPointArea = roiTemp.clone();
        }
        cv::line(outputImage,cv::Point(x,y),manLocationPoint(),cv::Scalar(0,0,255),2);
        cv::circle(outputImage,cv::Point(leftClickedPosX,leftClickedPosY),2,cv::Scalar(255,0,0),CV_FILLED);
        lastAreaX1 = x1;
        lastAreaY1 = y1;
        lastAreaX2 = x2;
        lastAreaY2 = y2;
        distance = std::sqrt(double(std::pow(x-manLocationX(),2)+std::pow(y-manLocationY(),2)));
    }

}

void JumpJump::mainTask()
{
    outputImage = inputImage.clone();
    roiImage = outputImage(cv::Rect((0),int(0.3*(double)inputImageHeight),
                                    (inputImageWidth),int(0.4*(double)inputImageHeight)));
    getEdge(roiImage,edgeImage,cannyThreshold1,cannyThreshold2);
    getTemplatePos(roiImage,templateImage,manPos);
    getTargetRectCornerPos(edgeImage,rectTopCornerPos,rectLeftCornerPos,rectRightCornerPos);

    // 修正点
    rectTopCornerPos += cv::Point(0, int(0.3*(double)inputImageHeight));
    rectLeftCornerPos += cv::Point(0, int(0.3*(double)inputImageHeight));
    rectRightCornerPos += cv::Point(0, int(0.3*(double)inputImageHeight));
    manPos += cv::Point(0,int(0.3*(double)inputImageHeight));

    drawAllPoints();
}

void JumpJump::getEdge(cv::Mat &src, cv::Mat &dst, double threshold1, double threshold2)
{
    if(src.channels() == 3)
        cv::cvtColor(src,dst,CV_RGB2GRAY);
    cv::Canny(src,dst,threshold1,threshold2);
}

void JumpJump::getTemplatePos(const cv::Mat &src, const cv::Mat &target, cv::Point &location)
{
    cv::Mat matchResult;
    cv::matchTemplate(src,target,matchResult,cv::TM_CCORR_NORMED);
    cv::Point maxValueLoaction;
    cv::minMaxLoc(matchResult,NULL,NULL,NULL,&maxValueLoaction);
    maxValueLoaction.x += int(double(target.cols)/2);
    maxValueLoaction.y += int(double(target.rows)/2)+
                          int(double(target.rows)*0.45);
    location = maxValueLoaction;
}

void JumpJump::drawAllPoints()
{
    cv::circle(outputImage,manLocationPoint(),4,cv::Scalar(0,255,0),CV_FILLED);

    cv::circle(outputImage,rectRightCornerPos,4,cv::Scalar(0,0,255),CV_FILLED);
    cv::circle(outputImage,rectLeftCornerPos,4,cv::Scalar(0,0,255),CV_FILLED);
    cv::circle(outputImage,rectTopCornerPos,4,cv::Scalar(0,0,255),CV_FILLED);
}

void JumpJump::getTargetRectCornerPos(const cv::Mat &edgeImage, cv::Point &topCorner, cv::Point &leftCorner, cv::Point &rightCorner)
{
    bool flag = false;
    for (int i = 0; i < edgeImage.rows; i++)
    {
        for (int j = 0; j < edgeImage.cols; j++)
        {
            if (edgeImage.ptr<uchar>(i)[j] == 255)
            {
                topCorner = cv::Point(j, i);
                flag = true;
                break;
            }
        }
        if (flag)
            break;
    }

    leftCorner = topCorner;
    rightCorner = topCorner;
    flag = true;
    do
    {
        if (edgeImage.ptr<uchar>(leftCorner.y + 1)[leftCorner.x - 1] == 255)
        {
            leftCorner += cv::Point(-1, 1);
            continue;
        }
        else if (edgeImage.ptr<uchar>(leftCorner.y)[leftCorner.x - 1] == 255)
        {
            leftCorner += cv::Point(-1, 0);
            continue;
        }
        else
        {
            flag = false;
        }
    } while (flag);

    flag = true;
    do
    {
        if (edgeImage.ptr<uchar>(rightCorner.y + 1)[rightCorner.x + 1] == 255)
        {
            rightCorner += cv::Point(1, 1);
            continue;
        }
        else if (edgeImage.ptr<uchar>(rightCorner.y)[rightCorner.x + 1] == 255)
        {
            rightCorner += cv::Point(1, 0);
            continue;
        }
        else
        {
            flag = false;
        }
    } while (flag);
}

