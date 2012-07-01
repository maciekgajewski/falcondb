#include "console_frontend/frontend.hpp"

int main(int argc, char** argv)
{
    falcondb::console_frontend::frontend frontend;

    frontend.execute();
}
