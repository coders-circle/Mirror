/* Client - main.cpp */

#include <common/common.h>
#include <client/Client.h>

// Since this is console, we need to keep track of which connection is going to receive chat messages entered
size_t chatConnectionId = -1;   // (#CONSOLE_PROBLEMS)

// A function to continuously take chat inputs and send it to peer/server defined by chatConnectionId
void ChatInput(Client& client, size_t groupId);
// Function to handle JOIN_CHAT requests
bool JoinChatEventHandler(size_t id);
// Function to handler incoming chat messages
void MessageEventHandler(MessageEventData& data);

int main(int argc, char *argv[])
{
    try
    {
        Client client;

        /* Set Event Handlers */
        client.SetJoinChatEventHandler(JoinChatEventHandler);
        client.SetMessageEventHandler(MessageEventHandler);

        std::string ip, name;
        /* Get name of user */
        std::cout << "Enter name: ";
        std::fflush(stdin);
        char namec[200];
        std::cin.getline(namec, 200);
        name = std::string(namec);
        client.SetName(name);

        /* Connection to server */
        std::cout << "Enter ip of server: "; std::cin >> ip;
        client.Connect(tcp::endpoint(boost::asio::ip::address::from_string(ip), 10011));

        /* Menu of choices */
        int choice;
        std::cout << "Enter 0 for group chat, 1 for P2P, 2 to just wait: ";
        std::cin >> choice;

        uint32_t groupId = 0;
        if (choice == 0)
        {
            /* For group chat, send JOIN_CHAT request to server */
            /* The server will redirect all chat messages to every client that has JOINed_CHAT with it on same groupId */
            std::cout << "Enter group-id to join: ";
            std::cin >> groupId;
            client.JoinChat(0, groupId);
            chatConnectionId = 0;
        }
        else if (choice == 1)
        {
            /* For p2p request, first connect to the peer...*/
            uint32_t cid;
            std::cout << "Enter client-id of peer: "; std::cin >> cid;
            bool successful;
            uint32_t id = client.Connect(cid, &successful);
            /* ... and if successful, send JOIN_CHAT request to it*/
            if (successful)
            {
                if (!client.JoinChat(id))  // this is handled in other peer through JoinChatEventHandler
                    std::cout << "Couldn't connect; other side may not have accepted the request" << std::endl;
                else
                    chatConnectionId = id;
            }
            else
                std::cout << "Invalid peer; couldn't connect" << std::endl;
        }

        boost::thread t(boost::bind(&ChatInput, _1, _2), boost::ref(client), groupId);
        client.HandleRequests();
            
        
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
        std::cin.get();
    }
    std::cin.get();
    return 0;
}


bool JoinChatEventHandler(size_t id)
{
    char c;
    std::cout << "Connection #" << id << " wants to join a conversation. Accept? (y/n) "; std::cin >> c;
    if (c != 'n')
    {
        chatConnectionId = id;
        return true;
    }
    return false;
}

void MessageEventHandler(MessageEventData& data)
{
    // When connected to multiple peers, set chatConnectedId (id to send messages to) 
    //  to whoever sent the last message (#CONSOLE_PROBLEMS)
    chatConnectionId = data.senderId;
    std::cout << "\n\n" << data.message << "\n\nYou: ";
}

void ChatInput(Client& client, size_t groupId)
{
    while (true)
    {
        // Don't take input, unless someone is conneced
        while (chatConnectionId == -1)
            boost::this_thread::sleep(boost::posix_time::milliseconds(500));

        // Take input
        std::cout << "\nYou: ";
        fflush(stdin);
        char input[1024];

        // Create message along with client's name 
        // TODO: donot send message with name: instead append the name on receiving side
        std::cin.getline(input, 1024);
        std::string message = client.GetName() + ": " + input;

        // Send the message
        client.SendMessage(chatConnectionId, message, groupId);
    }
}
