/* Client - main.cpp */

#include <common/common.h>
#include <client/TcpClient.h>


int main(int argc, char *argv[])
{
    try
    {
        boost::asio::io_service io;
        TcpClient client(io);
        //uint16_t port;
        //std::cout << "Enter port of this client: ";
        //std::cin >> port;
        client.StartListening(/*tcp::endpoint(tcp::v4(), port)*/);

        // Connect to another peer
        /*{
            std::cout << "Enter port to connect to: ";
            std::cin >> port;
            client.Connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port));
            while (true);
         }*/

        // Connect to server
        {
            std::string ip;
            std::cout << "Enter ip of server: "; std::cin >> ip;
            client.Connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), 10011/*8183*/));  // 10011 for Ankit's Server
            uint32_t groupId;
            std::cout << "Enter group-id to join: ";
            std::cin >> groupId;
            client.JoinGroup(groupId);
            client.StartChatSession(groupId);
            
            while (true)
                boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
                ;
        }
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
        std::cin.get();
    }
    std::cin.get();
    return 0;
}
