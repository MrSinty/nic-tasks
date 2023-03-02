#include "ServerAsync.h"

using boost::asio::ip::tcp;

class session
    : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket, int numOfClient)
        : socket_(std::move(socket)), numInServer(numOfClient)
    {
    }

    void start()
    {
        std::cout << "connection established: " << numInServer << std::endl;
        do_read();
    }

private:
    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    do_write(length);
                }
                else
                    std::cout << "connection lost: " << numInServer << std::endl;
            });
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
            [this, self](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {
                    do_read();
                }
                else
                    std::cout << "connection lost: " << numInServer << std::endl;
            });
    }

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    int numInServer = -1;
};

class server
{
public:
    server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        std::cout << "Server started!" << std::endl;
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    ++numOfClient;
                    std::make_shared<session>(std::move(socket), numOfClient)->start();
                }

                do_accept();
            });
    }

    tcp::acceptor acceptor_;
    int numOfClient = 0;
};

int main(int argc, char* argv[])
{
    try
    {
        /*if (argc != 2)
        {
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }*/

        boost::asio::io_context io_context;

        server s(io_context, std::atoi("5000"));

        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}