#include "ixm/session.hpp"
#include <iostream>


int main(int argc, char const *argv[])
{
    ixm::session::arguments args;
    bool argc_check = args.argc() == argc;
    auto a1 = args[0];
    
    int i = 0;
    for (auto& a : args)
    {
        std::cout << i++ << ": " << a << '\n';
    }

    return 0;
}
