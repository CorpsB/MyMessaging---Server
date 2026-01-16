#include "Server.hpp"
#include "iostream"

Server::Server(boost::asio::io_context& io, int port, MessageRepository& msgRepo) : _io(io), _acceptor(io, tcp::endpoint(tcp::v4(), port)), _msgRepo(msgRepo)
{
}

void Server::start()
{
    do_accept();
}

void Server::do_accept()
{
    auto client = std::make_shared<tcp::socket>(_io);

    _acceptor.async_accept(*client,
        [this, client](boost::system::error_code ec)
        {
            if (!ec) {
                _clients.insert(client);
                do_read(client);
            }

            do_accept();
        });
}

void Server::do_read(std::shared_ptr<tcp::socket> client)
{
    auto buffer = std::make_shared<boost::asio::streambuf>();

    boost::asio::async_read_until(*client, *buffer, '\n',
        [this, client, buffer](boost::system::error_code ec, std::size_t)
        {
            if (ec) {
                _clients.erase(client);
                return;
            }

            std::istream is(buffer.get());
            std::string line;
            std::getline(is, line);

            const int authorId  = 1;

            try {
                _msgRepo.CreateMessage(line, authorId);
            } catch (const Error& e) {
                std::cerr << "DB insert failed at " << e.where()
                        << ": " << e.what() << "\n";

                boost::asio::async_write(
                    *client,
                    boost::asio::buffer(std::string("SERVER: DB error\n")),
                    [](boost::system::error_code, std::size_t) {}
                );

                do_read(client);
                return; 
            }
            broadcast_except("> " + line + "\n", client);

            do_read(client);
        });
}


void Server::broadcast(const std::string& msg)
{
    for (auto& client : _clients) {
        boost::asio::async_write(
            *client,
            boost::asio::buffer(msg),
            [](boost::system::error_code, std::size_t) {}
        );
    }
}

void Server::broadcast_except(const std::string& msg, const std::shared_ptr<tcp::socket>& except)
{
    for (auto& c : _clients) {
        if (c == except)
            continue;

        boost::asio::async_write(*c, boost::asio::buffer(msg), [](boost::system::error_code, std::size_t) {}
        );
    }
}
