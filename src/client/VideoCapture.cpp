#include "client/VideoCapture.h"


//
//std::vector<VideoCaptureDevice> VideoCapture::Enumerate()
//{ 
//
//    //Apparently it is not possible due to cross-flatform issue
//    //So, I guess we have to do it individually for different platforms 
//    //without using opencv
//    
//    std::vector<VideoCaptureDevice> capDevices;
//    return capDevices;
//    
//}
//
//void VideoCapture::Initialize(int deviceID)
//{
//    m_captureDevice.open(deviceID);
//    
//    if (!m_captureDevice.isOpened()) throw Exception("error opening the video capturing device");
//    m_fw = static_cast<int>(m_captureDevice.get(CV_CAP_PROP_FRAME_WIDTH));
//    m_fh = static_cast<int>(m_captureDevice.get(CV_CAP_PROP_FRAME_HEIGHT));
//    m_fps = m_captureDevice.get(CV_CAP_PROP_FPS);   //not working :/
//    m_videoWriter.open("test.avi", CV_FOURCC('x', '2', '6', '4'), 10.0, cv::Size(m_fw, m_fh));
//    if (m_videoWriter.isOpened()) std::cout << "oh yeah!";
//}
//
//int VideoCapture::GetFrameWidth(){ return m_fw; }
//int VideoCapture::GetFrameHeight(){ return m_fh; }
//
//unsigned char* VideoCapture::GetBGRAFrame()
//{
//    cv::cvtColor(m_capturedFrame, m_capturedFrame, CV_RGB2RGBA, 4);
//    return m_capturedFrame.data;
//}
//
//void VideoCapture::CaptureFrame()
//{
//    m_captureDevice >> m_capturedFrame;
//    m_videoWriter << m_capturedFrame;
//    //m_videoWriter.write(m_capturedFrame);
//    
//}
//
//VideoCapture::~VideoCapture()
//{
//    if (m_captureDevice.isOpened()) m_captureDevice.release();
//    if (m_videoWriter.isOpened()) m_videoWriter.release();
//    
//}
