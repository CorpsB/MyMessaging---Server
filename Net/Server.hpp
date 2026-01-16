#pragma once

#include "../Include.hpp"
#include <unordered_set>

class MessageRepository;

class Server {
    public:
        using tcp = boost::asio::ip::tcp;
        Server(boost::asio::io_context &io, int port, MessageRepository &msgRep);

        void start();
        void do_accept();
        void do_read(std::shared_ptr<tcp::socket> client);
        void broadcast(const std::string& msg);
        void broadcast_except(const std::string& msg, const std::shared_ptr<tcp::socket>& except);


    private:
        boost::asio::io_context &_io;
        tcp::acceptor _acceptor;

        std::unordered_set<std::shared_ptr<tcp::socket>> _clients;
        MessageRepository &_msgRepo;

};