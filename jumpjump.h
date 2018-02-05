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
    void setInputImage(cv::Mat &inputMat);
    void setTemplateImage(cv::Mat &templateImg);
    cv::Mat edgeImage;
    cv::Mat outputImage;
    void updateEdgeImage();
    void update();
    double cannyThreshold1 = 54.0;
    double cannyThreshold2 = 174.0;
    bool isLoadInputImage();
    int manLocationX() const;
    int manLocationY() const;
    int blockLocationX() const;
    int blockLocationY() const;
    double jumpDistance() const;
    cv::Point manLocationPoint();
    void setLeftClickedPos(int x,int y);

private:
    void mainTask();
    void getEdge(cv::Mat &src,cv::Mat &dst, double threshold1, double threshold2);
    void getTemplatePos(const cv::Mat &src, const cv::Mat &target, cv::Point &location);
    void drawAllPoints();
    void getBlockCornersPos(const cv::Mat &edgeImage, cv::Point &topCorner, cv::Point &leftCorner, cv::Point &rightCorner);
    void getBlockCenterPos(const cv::Point &topCorner, const cv::Point &leftConer, const cv::Point &rightCorner, cv::Point &centerPoint);

    double distance = -1.0;

    int inputImageWidth;
    int inputImageHeight;

    cv::Mat inputImage;
    cv::Mat roiImage;
    cv::Mat templateImage;
    cv::Mat copyOfDrawPointArea;
    int lastAreaX1, lastAreaX2, lastAreaY1, lastAreaY2;

    cv::Point manPos;
    cv::Point blockLeftCornerPos;
    cv::Point blockTopCornerPos;
    cv::Point blockRightCornerPos;
    cv::Point blockCenterPos;

    bool isLoadImage = false;
    bool isLoadTemplateImage = false;
    bool isDrawPointAreaCopied = false;

    int leftClickedPosX = -1;
    int leftClickedPosY = -1;
};

#endif // JUMPJUMP_H
