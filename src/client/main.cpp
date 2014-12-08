/* Client - main.cpp */

#include <common/common.h>

//boost test for separate thread
void Thread()
{
	 int i=0;
    while (i<100) 
	 {
        std::cout << "From Child Thread" << std::endl;
	 	  i++;	
    }

}
int main()
{
    boost::thread t(Thread);
    std::cout << "From Main Thread" << std::endl;
	 t.join();
    return 0;
}
