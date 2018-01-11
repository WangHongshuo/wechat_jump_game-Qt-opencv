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

private:
    int inputImageWidth;
    int inputImageHeight;
    void mainTask();
    void getEdge(cv::Mat &src,cv::Mat &dst, double threshold1, double threshold2);
    void findTemplateLocation(cv::Mat &src, cv::Mat &dst, const cv::Mat &target);
    cv::Mat inputImage;
    cv::Mat roiImage;
    cv::Mat templateImage;

    bool isLoadImage = false;

};

#endif // JUMPJUMP_H
