/* Client - main.cpp */

#include <common/common.h>
#include <client/Client.h>



Client client;
int main(int argc, char *argv[])
{
    try
    {
        std::string ip;
        std::cout << "Enter server ip: ";
        std::cin >> ip;
    
    
        client.SetServer(client.Connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), 10011)));
        client.JoinChat(client.GetServer());
        client.JoinVideoChat(client.GetServer());

        client.HandleRequests();
        
        return 0;
    }
    catch (std::exception err)
    {
        std::cout << "\n:/ \n" << err.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cout << "\n:/ \n" << "Unexpected exception occured"  << std::endl;
        return 1;
    }
}

#ifdef _WIN32
#pragma comment(lib, "gtk-win32-3.0.lib")
#pragma comment(lib, "gobject-2.0.lib")
#pragma comment(lib, "cairo.lib")
#pragma comment(lib, "glib-2.0.lib")
#pragma comment(lib, "gdk-win32-3.0.lib")
#endif

