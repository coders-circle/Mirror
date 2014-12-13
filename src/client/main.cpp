/* Client - main.cpp */

#include <common/common.h>
#include <client/TcpClient.h>


int main(int argc, char *argv[])
{
    try
    {
        boost::asio::io_service io;
        TcpClient client(io);
        uint16_t port;
        std::cout << "Enter port to listen to: ";
        std::cin >> port;
        client.StartListening(tcp::endpoint(tcp::v4(), port));


        {
            std::cout << "Enter port to connect to: ";
            std::cin >> port;
            client.Connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port));
            while (true);
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
