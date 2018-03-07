#ifndef JUMPJUMP_H
#define JUMPJUMP_H
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <fstream>

class JumpJump
{
public:
    JumpJump();
    ~JumpJump();    

    cv::Mat edgeImage;
    cv::Mat outputImage;

    cv::Point manLocationPoint();
    cv::Mat returnInputImage() const;
    void setInputImage(cv::Mat &inputMat);
    void setTemplateImage(cv::Mat &templateImg);
    void setPressScreenTimeParameter(double t);
    void setEnableOutputLog(bool flag);
    void updateEdgeImage();
    void update();
    bool isLoadInputImage();
    int manLocationX() const;
    int manLocationY() const;
    int blockLocationX() const;
    int blockLocationY() const;
    double jumpDistance() const;
    int getPressScreenTime() const;
    double getPressScreenTimeParameter() const;
    bool loadInifile(std::string path);

    void setLeftClickedPos(int x,int y);

private:
    void mainTask();
    void getBinaryImage(const cv::Mat &src, cv::Mat &dst);
    void getTemplatePos(cv::Mat &src, const cv::Mat &target, cv::Point &targetLocation, cv::Point &oriTargetLocation);
    void drawAllPoints();
    void removeManEdge(cv::Mat &edgeImg, const cv::Mat &manImg, const cv::Point &manPos);
    void getBlockCornersPos(const cv::Mat &edgeImage, cv::Point &topCorner, cv::Point &leftCorner, cv::Point &rightCorner);
    void getBlockCenterPos(const cv::Point &topCorner, const cv::Point &leftConer, const cv::Point &rightCorner, cv::Point &centerPoint);
    double getFixedPressScreenTimeParameterCorrections(double distance);
    bool readCorrectionsFromIniFile(std::ifstream &reader);
    template<typename A>
    void copyArray(const A (&src), A (&dst), int from, int to);
    double distance = -1.0;
    double pressScreenTimeParameter = 1.447;
    double fixedPressScreenTimeParameter = -1.0;
    cv::Point cvPointTemp;
    double lineSlope = 0;
    int pressScreenTime = 0;

    int inputImageWidth;
    int inputImageHeight;

    cv::Mat inputImage;
    cv::Mat roiImage;
    cv::Mat templateImage;
    cv::Mat copyOfDrawPointArea;
    int lastAreaX1, lastAreaX2, lastAreaY1, lastAreaY2;

    cv::Point manPosInRoiEdgeImage;
    cv::Point manPos;
    cv::Point blockLeftCornerPos;
    cv::Point blockTopCornerPos;
    cv::Point blockRightCornerPos;
    cv::Point blockCenterPos;

    bool isLoadImage = false;
    bool isLoadTemplateImage = false;
    bool isDrawPointAreaCopied = false;
    bool isOutputLog = false;

    int leftClickedPosX = -1;
    int leftClickedPosY = -1;

    std::ofstream outputTxtFile;
    bool isOutputTxtFileOpened = false;
    int jumpCount = 1;

    std::ifstream iniLoader;
    double corrections[20] = {0.0};
    double correctionsBuffer[20] = {0.0};
    bool isCorrectionsBufferLoaded = false;
};

#endif // JUMPJUMP_H
