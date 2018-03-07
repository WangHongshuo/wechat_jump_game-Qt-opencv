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

void JumpJump::setPressScreenTimeParameter(double t)
{
    pressScreenTimeParameter = t;
}



void JumpJump::updateEdgeImage()
{
    getBinaryImage(roiImage,edgeImage);
}

void JumpJump::update()
{
    mainTask();
}

bool JumpJump::isLoadInputImage()
{
    return isLoadImage;
}

int JumpJump::manLocationX() const
{
    return  manPos.x;
}

int JumpJump::manLocationY() const
{
    return  manPos.y;
}

int JumpJump::blockLocationX() const
{
    return blockCenterPos.x;
}

int JumpJump::blockLocationY() const
{
    return blockCenterPos.y;
}

double JumpJump::jumpDistance() const
{
    return distance;
}

int JumpJump::getPressScreenTime() const
{
    return pressScreenTime;
}

double JumpJump::getPressScreenTimeParameter() const
{
    return pressScreenTimeParameter;
}

cv::Point JumpJump::manLocationPoint()
{
    return manPos;
}

cv::Mat JumpJump::returnInputImage() const
{
    return inputImage;
}

void JumpJump::setLeftClickedPos(int x, int y)
{
    if(x >= 0 && y >= 0 && isLoadImage)
    {
        leftClickedPosX = x;
        leftClickedPosY = y;
        // ����Ҫ�����Ļ�ͼ����
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
        // fixed press time when the block center pos is close to the man pos
        if(distance < 500 && distance > 200)
        {
            pressScreenTimeParameterTemp = pressScreenTimeParameter+0.01*(6-(int(distance)/50-4));
            qDebug() << 0.01*(6-(int(distance)/50-4));
        }
        else if(distance <= 200)
        {
            pressScreenTimeParameterTemp = pressScreenTimeParameter+0.02*(4-int(distance)/50);
            qDebug() << 0.02*(4-int(distance)/50);
        }
        else
        {
            pressScreenTimeParameterTemp = pressScreenTimeParameter;
            qDebug() << 0;
        }

        pressScreenTime = (int)(distance*pressScreenTimeParameterTemp);

        blockCenterPos.x = x;
        blockCenterPos.y = y;
    }

}

void JumpJump::mainTask()
{
    outputImage = inputImage.clone();
    roiImage = inputImage(cv::Rect((0),int(0.3*(double)inputImageHeight),
                                    (inputImageWidth),int(0.4*(double)inputImageHeight)));
    getBinaryImage(roiImage,edgeImage);
    getTemplatePos(roiImage,templateImage,manPos,manPosInRoiEdgeImage);
    removeManEdge(edgeImage,templateImage,manPosInRoiEdgeImage);
    getBlockCornersPos(edgeImage,blockTopCornerPos,blockLeftCornerPos,blockRightCornerPos);
    getBlockCenterPos(blockTopCornerPos,blockLeftCornerPos,blockRightCornerPos,blockCenterPos);
    // fix Pos
    blockTopCornerPos += cv::Point(0, int(0.3*(double)inputImageHeight));
    blockLeftCornerPos += cv::Point(0, int(0.3*(double)inputImageHeight));
    blockRightCornerPos += cv::Point(0, int(0.3*(double)inputImageHeight));
    blockCenterPos += cv::Point(0, int(0.3*(double)inputImageHeight));
    manPos += cv::Point(0,int(0.3*(double)inputImageHeight));

    drawAllPoints();
    setLeftClickedPos(blockCenterPos.x,blockCenterPos.y);
}

void JumpJump::getBinaryImage(const cv::Mat &src, cv::Mat &dst)
{
    if(src.channels() == 3)
        cv::cvtColor(src,dst,CV_RGB2GRAY);
    else
        dst = src.clone();

    uchar temp;
    int temp2;
    for (int i = 0; i < dst.rows; i++)
    {
        temp = dst.ptr<uchar>(i)[0];
        for (int j = 1; j < dst.cols; j++)
        {
            temp2 = temp - dst.ptr<uchar>(i)[j];
            if (temp2 >= -1 && temp2 <= 1)
                dst.ptr<uchar>(i)[j] = 0;
            else
                dst.ptr<uchar>(i)[j] = 255;
        }
        dst.ptr<uchar>(i)[0] = 0;
    }
}

void JumpJump::getTemplatePos(cv::Mat &src, const cv::Mat &target, cv::Point &targetLocation, cv::Point &oriTargetLocation)
{
    cv::Mat matchResult;
    cv::matchTemplate(src,target,matchResult,cv::TM_CCORR_NORMED);
    cv::minMaxLoc(matchResult,NULL,NULL,NULL,&oriTargetLocation);
    targetLocation = oriTargetLocation+cv::Point(int(double(target.cols)/2),
                                                 int(double(target.rows)/2)+int(double(target.rows)*0.45));
}

void JumpJump::drawAllPoints()
{
    cv::circle(outputImage,manPos,6,cv::Scalar(0,255,0),CV_FILLED);
    cv::circle(outputImage,blockCenterPos,6,cv::Scalar(255,0,0),CV_FILLED);

    cv::circle(outputImage,blockRightCornerPos,6,cv::Scalar(0,0,255),CV_FILLED);
    cv::circle(outputImage,blockLeftCornerPos,6,cv::Scalar(0,0,255),CV_FILLED);
    cv::circle(outputImage,blockTopCornerPos,6,cv::Scalar(0,0,255),CV_FILLED);
}

void JumpJump::removeManEdge(cv::Mat &dst, const cv::Mat &manImg, const cv::Point &manPos)
{
    cv::rectangle(dst,cv::Rect(manPos.x-1,manPos.y-1,manImg.cols+1,manImg.rows+1),cv::Scalar(0),CV_FILLED);
}

void JumpJump::getBlockCornersPos(const cv::Mat &edgeImage, cv::Point &topCorner, cv::Point &leftCorner, cv::Point &rightCorner)
{
    topCorner = leftCorner = rightCorner = cv::Point(0,0);
    // find top center pos
    bool flag = false;
    int length = 0;
    for (int i = 0; i < edgeImage.rows; i++)
    {
        for (int j = 0; j < edgeImage.cols; j++)
        {
            if (edgeImage.ptr<uchar>(i)[j] == 255)
            {
                length++;
                flag = true;
                continue;
            }
            else if(flag)
            {
                topCorner = cv::Point(j-1,i);
                break;
            }
        }
        if (flag)
        {
            topCorner -= cv::Point(int(length*0.5),0);
            break;
        }
    }
    // didn't find top corner, stop searching.
    if(!flag)
        return;

    leftCorner = topCorner;
    rightCorner = topCorner;
    flag = true;
    do
    {
        if (edgeImage.ptr<uchar>(leftCorner.y)[leftCorner.x - 1] == 255)
        {
            leftCorner += cv::Point(-1,0);
            continue;
        }
        else if (edgeImage.ptr<uchar>(leftCorner.y + 1)[leftCorner.x - 1] == 255)
        {
            leftCorner += cv::Point(-1,1);
            continue;
        }
        else
        {
            flag = false;
        }
    } while (flag);

    int t = 0;
    flag = true;
    do
    {
        if (edgeImage.ptr<uchar>(rightCorner.y)[rightCorner.x + 1] == 255)
        {
            rightCorner += cv::Point(1,0);
            t = 0;
            continue;
        }
        else if (edgeImage.ptr<uchar>(rightCorner.y + 1)[rightCorner.x + 1] == 255)
        {
            rightCorner += cv::Point(1,1);
            t = 0;
            continue;
        }
        else if (edgeImage.ptr<uchar>(rightCorner.y + 1)[rightCorner.x] == 255)
        {
            rightCorner += cv::Point(0,1);
            t++;
            if(t > 3)
            {
                rightCorner -= cv::Point(0,4);
                flag = false;
            }
            continue;
        }
        else
        {
            flag = false;
        }
    } while (flag);
}

void JumpJump::getBlockCenterPos(const cv::Point &topCorner, const cv::Point &leftCorner, const cv::Point &rightCorner, cv::Point &centerPoint)
{
    centerPoint.x = topCorner.x;
    if(leftCorner.y-rightCorner.y > 45 || rightCorner.y-leftCorner.y > 45)
        centerPoint.y = (int)(leftCorner.y+rightCorner.y)*0.5;
    else
        centerPoint.y = std::max(leftCorner.y,rightCorner.y);

//    int x1 = topCorner.x, y1 = topCorner.y;
//    int x2 = leftCorner.x, y2 = leftCorner.y;
//    int x3 = rightCorner.x, y3 = rightCorner.y;
//    double a = x1-x2, b = y1-y2;
//    double c = x1-x3, d = y1-y3;
//    double e = ((x1*x1-x2*x2)-(y2*y2-y1*y1))*0.5;
//    double f = ((x1*x1-x3*x3)-(y3*y3-y1*y1))*0.5;
//    double delta = b*c-a*d;
//    int y4 = (int)(c*e-a*f)/delta;
//    centerPoint.x = (int)(x2+x3)*0.5;
//    centerPoint.y = (int)(y1+y4)*0.5;
}

