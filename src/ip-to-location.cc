#include <iostream>
#include "IPToLocation.hh"

int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++)
    {
        std::cout << get_location_from_ip(argv[i]) << '\n';
    }
    return 0;
}
