#pragma once

#include "../Include.hpp"
#include <unordered_set>



class Server {
    public:
        using tcp = boost::asio::ip::tcp;
        Server(boost::asio::io_context &io, int port);

        void start();
        void do_accept();
        void do_read(std::shared_ptr<tcp::socket> client);
        void broadcast(const std::string& msg);

    private:
        boost::asio::io_context &_io;
        tcp::acceptor _acceptor;


        std::unordered_set<std::shared_ptr<tcp::socket>> _clients;

};