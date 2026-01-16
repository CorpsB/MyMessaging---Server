#include "../Include.hpp"

#include <boost/asio.hpp>

int main()
{
    boost::asio::io_context io;

    Server server(io, 5555);
    server.start();

    io.run();
}