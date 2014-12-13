#pragma once
#include "RTPCommon.h"

class RtpTransmissionException : public Exception
{
public:
    RtpTransmissionException(const std::string &errorString) 
        : Exception("RTP Transmission Error: " + errorString)
    {}
};

class RtpTransmitter
{
public:
    RtpTransmitter();
    ~RtpTransmitter();

    void Initialize(boost::shared_ptr<udp::socket> socket, const udp::endpoint &destination);
    void CleanUp();

    int GetTimeStamp() const { return m_timeStamp; }
    void SetTimeStamp(int timeStamp) { m_timeStamp = timeStamp; }

    int GetTimeStampIncrement() const { return m_timeStampIncrement; }
    void SetTimeStampIncrement(int timeStampIncrement) { m_timeStampIncrement = timeStampIncrement; }

    uint16_t GetSequenceNumber() const { return m_sequenceNumber; }
    void SetSequenceNumber(uint16_t sequenceNumber) { m_sequenceNumber = sequenceNumber; }

    uint8_t GetPayloadType() const { return m_payloadType; }
    void SetPayloadType(uint8_t payloadType) { m_payloadType = payloadType; }

    void Send(const char *data, size_t size);
private:
    boost::shared_ptr<udp::socket> m_socket;
    udp::endpoint m_destination;

    uint16_t m_sequenceNumber;
    int m_timeStamp, m_timeStampIncrement;
    uint8_t m_payloadType;
};