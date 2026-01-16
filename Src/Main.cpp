#include "../Include.hpp"

#include <boost/asio.hpp>

int main()
{
    boost::asio::io_context io;

    DataBase db("db.db");
    MessageRepository msgRepo(db);

    Server server(io, 5555, msgRepo);
    server.start();

    io.run();
}