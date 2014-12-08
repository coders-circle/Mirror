/* Server - main.cpp */

#include <common/common.h>
int main()
{
    try
    {
        throw Exception("Aafnai bariko exception, aina herera");
    }
    catch (Exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    std::cin.get();
    return 0;
}