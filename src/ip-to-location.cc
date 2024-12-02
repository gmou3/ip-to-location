#include <iostream>
#include "IPToLocation.hh"

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        std::cout << get_location_from_ip(argv[1]) << std::endl;
    }
	return 0;
}
