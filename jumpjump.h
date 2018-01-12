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
    void setInputImageAndTemplateImage(cv::Mat &inputMat, cv::Mat &templateMat);
    cv::Mat edgeImage;
    cv::Mat outputImage;
    void update();
    double cannyThreshold1 = 127.0;
    double cannyThreshold2 = 255.0;
    bool isLoadInputImage();
    int manLocationX();
    int manLocationY();
    cv::Point manLocationPoint();
    void setLeftClickedPos(int x,int y);

private:
    int inputImageWidth;
    int inputImageHeight;
    void mainTask();
    void getEdge(cv::Mat &src,cv::Mat &dst, double threshold1, double threshold2);
    void findTemplateLocation(cv::Mat &src, cv::Mat &dst, const cv::Mat &target, cv::Point &location);
    cv::Mat inputImage;
    cv::Mat roiImage;
    cv::Mat templateImage;
    cv::Point manLocation;
    bool isLoadImage = false;
    int leftClickedPosX = -1;
    int leftClickedPosY = -1;

};

#endif // JUMPJUMP_H
