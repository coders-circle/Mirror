#pragma once

#include "common/common.h"
#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>

// TODO: Implement this class
class VideoCaptureDevice
{
    
};



class VideoCapture
{
public:
    VideoCapture():m_fw(0), m_fh(0), m_fps(0){}
    std::vector<VideoCaptureDevice> Enumerate();
    void CaptureFrame();
    unsigned char* GetBGRAFrame();
    int GetFrameWidth();
    int GetFrameHeight();
    void SetFrameWidth();
    void SetFrameHeight();
    void Initialize(int deviceID = 0);
    double GetFPS();
    ~VideoCapture();
private:
    cv::VideoCapture m_captureDevice;
    cv::VideoWriter m_videoWriter;
    cv::Mat m_capturedFrame;
    int m_fw, m_fh;
    double m_fps;
};

