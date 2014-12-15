/* Server - main.cpp */

#include <common/common.h>
#include <server/ClientsManager.h>

int main()
{
    boost::asio::io_service io_service;
    ClientsManager manager(io_service);

    size_t clientsNumber = 0;
    // 8183 is the port of this server; 8183 == BIBE(K) :P
    manager.StartListening(tcp::endpoint(tcp::v4(), 10011));
    manager.StartProcessing();
    
    //while (true)
    //{
    //    // for every new connection, print its address
    //    for (; clientsNumber < manager.GetClients().size(); ++clientsNumber)
    //        std::cout << "Client Connected: " << manager.GetClients()[clientsNumber].connection.GetDestinationAddress() << std::endl;
    //}

    std::cin.get();
    return 0;
}