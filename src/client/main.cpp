/* Client - main.cpp */

#include <common/common.h>
#include <client/TcpClient.h>


int main(int argc, char *argv[])
{
    try
    {
        boost::asio::io_service io;
        TcpClient client(io);
        //client.StartAccepting();

        // Connect to another peer
        /*{
            std::cout << "Enter port to connect to: ";
            std::cin >> port;
            client.Connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port));
            while (true);
         }*/

        // Connect to server
        {
            std::string ip, name;
            std::cout << "Enter name: ";
            std::fflush(stdin);
            char namec[200];
            std::cin.getline(namec, 200);
            name = std::string(namec);
            client.SetName(name);


            std::cout << "Enter ip of server: "; std::cin >> ip;
            client.Connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), 10011));

            int choice;
            std::cout << "Enter 0 for group chat, 1 for P2P, 2 to just wait: ";
            std::cin >> choice;

            uint32_t groupId = 0;
            if (choice == 0)
            {
                std::cout << "Enter group-id to join: ";
                std::cin >> groupId;
                client.JoinChat(0, groupId);
            }
            else if (choice == 1)
            {
                uint32_t cid;
                std::cout << "Enter client-id of peer: "; std::cin >> cid;
                client.Connect(cid);
            }
            client.StartChatInput(groupId);
            client.HandleRequests();
            
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
