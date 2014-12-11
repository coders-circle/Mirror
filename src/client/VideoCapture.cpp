#include "client/VideoCapture.h"



std::vector<VideoCaptureDevice> VideoCapture::Enumerate()
{ 

    //Apparently it is not possible due to cross-flatform issue
    //So, I guess we have to do it individually for different platforms 
    //without using opencv
    
    std::vector<VideoCaptureDevice> capDevices;
    return capDevices;
    
}

void VideoCapture::Initialize(int deviceID)
{
    m_captureDevice.open(deviceID);
    if (!m_captureDevice.isOpened()) throw Exception("error opening the video capturing device");
    m_fw = static_cast<int>(m_captureDevice.get(CV_CAP_PROP_FRAME_WIDTH));
    m_fh = static_cast<int>(m_captureDevice.get(CV_CAP_PROP_FRAME_HEIGHT));
}

int VideoCapture::GetFrameWidth(){ return m_fw; }
int VideoCapture::GetFrameHeight(){ return m_fh; }

unsigned char* VideoCapture::GetBGRAFrame()
{
    m_captureDevice >> m_capturedFrame;
    cv::cvtColor(m_capturedFrame, m_capturedFrame, CV_RGB2RGBA, 4);
    return m_capturedFrame.data;
}