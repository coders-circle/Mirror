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
        
        MediaStreamer mediaStreamer;
    
        client.SetServer(client.Connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), 10011)));
        client.JoinChat(client.GetServer());
        client.JoinVideoChat(client.GetServer());

        client.HandleRequestsAsync();
        client.StartStreaming(mediaStreamer);

        int i=0;
        while(true)
        {
            boost::this_thread::sleep(boost::posix_time::milliseconds(500));
            if (i<10)
            {
                std::vector<char> data;
                data.resize(50000);
                for (size_t i=0; i<data.size(); ++i)
                    data[i] = i%255;
                mediaStreamer.Send((uint8_t*)&data[0], data.size());
                std::cout << "Sending: " << data.size() << std::endl;
                i++;
            }
        }

        
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

