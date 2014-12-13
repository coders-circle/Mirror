#pragma once
#include "RTPCommon.h"

class RtpReceptionError : public Exception
{
public:
    RtpReceptionError(std::string errorString)
        : Exception("RTP Reception Error: " + errorString)
    {}
};

class RtpReceiver
{
public:
    RtpReceiver();
    ~RtpReceiver();

    void Initialize(boost::shared_ptr<udp::socket> socket);
    void CleanUp();

    int GetTimeStamp() const { return m_timeStamp; }
    void SetTimeStamp(int timeStamp) { m_timeStamp = timeStamp; }

    uint16_t GetSequenceNumber() const { return m_sequenceNumber; }
    void SetSequenceNumber(uint16_t sequenceNumber) { m_sequenceNumber = sequenceNumber; }

    uint8_t GetPayloadType() const { return m_payloadType; }
    void SetPayloadType(uint8_t payloadType) { m_payloadType = payloadType; }

    const udp::endpoint& GetSource() const { return m_source; }

    void Receive(char* data, size_t maxSize = 1500);
private:
    boost::shared_ptr<udp::socket> m_socket;
    udp::endpoint m_source;

    uint16_t m_sequenceNumber;
    int m_timeStamp;
    uint8_t m_payloadType;
};