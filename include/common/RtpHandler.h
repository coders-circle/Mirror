#pragma once
#define RTP_HEADER_SIZE 12 

class RtpTransmissionException : public Exception
{
public:
    RtpTransmissionException(const std::string &errorString) 
        : Exception("RTP Transmission Error: " + errorString)
    {}
};
class RtpReceptionError : public Exception
{
public:
    RtpReceptionError(std::string errorString)
        : Exception("RTP Reception Error: " + errorString)
    {}
};

class RtpHandler
{
public:
    RtpHandler();
    ~RtpHandler();

    void Initialize(boost::shared_ptr<udp::socket> socket, const udp::endpoint &remoteEndpoint);
    void CleanUp();

    int GetTimeStamp() const { return m_timeStamp; }
    void SetTimeStamp(int timeStamp) { m_timeStamp = timeStamp; }

    int GetTimeStampIncrement() const { return m_timeStampIncrement; }
    void SetTimeStampIncrement(int timeStampIncrement) { m_timeStampIncrement = timeStampIncrement; }

    uint16_t GetSequenceNumber() const { return m_sequenceNumber; }
    void SetSequenceNumber(uint16_t sequenceNumber) { m_sequenceNumber = sequenceNumber; }

    uint8_t GetPayloadType() const { return m_payloadType; }
    void SetPayloadType(uint8_t payloadType) { m_payloadType = payloadType; }

    const udp::endpoint& GetRemoteEndpoint() const { return m_remoteEndpoint; }

    void Send(const char *data, size_t size);
    void Receive(char* data, size_t maxSize = 1500);
private:
    boost::shared_ptr<udp::socket> m_socket;
    udp::endpoint m_remoteEndpoint;

    uint16_t m_sequenceNumber;
    int m_timeStamp, m_timeStampIncrement;
    uint8_t m_payloadType;
};