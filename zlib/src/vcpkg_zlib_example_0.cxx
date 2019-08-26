#include <iostream>
#include <zlib.h>
int main(int argc, char *argv[])
{
    std::cout << zlibVersion() << std::endl;
    return 0;
}