#include "ixm/session.hpp"
#include <iostream>


int main(int argc, char const *argv[])
{
    ixm::session::arguments args;
    bool argc_check = args.argc() == argc;
    auto a1 = args[0];
    auto a2 = args[1];

    return 0;
}
