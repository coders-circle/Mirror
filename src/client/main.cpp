/* Client - main.cpp */

#include <common/common.h>

//boost test for separate thread
void Thread()
{
    while (true) 
        std::cout << "From Child Thread" << std::endl;
}
int main()
{
    boost::thread t(Thread);
    while (true)
        std::cout << "From Main Thread" << std::endl;
    return 0;
}