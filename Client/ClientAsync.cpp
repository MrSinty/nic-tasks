#include "ClientAsync.h"

using boost::asio::ip::tcp;

enum { max_length = 1024 };

void connection(std::string ip, std::string port, int numOfRW)
{
    try
    {
        boost::asio::io_context io_context;

        tcp::socket s(io_context);
        tcp::resolver resolver(io_context);
        boost::asio::connect(s, resolver.resolve(ip, port));
        for (int i = 0; i < numOfRW; ++i) {
            static const char alphanum[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";

            std::string request;
            ////char request[max_length];
            /*for (int i = 0; i < 4; ++i) {
                request += alphanum[rand() % (sizeof(alphanum) - 1)];
            }*/

            std::cout << "Enter message: "; //<< request << std::endl;
            std::cin >> request;
            //std::cin.getline(request, max_length);
            size_t request_length = request.length();//std::strlen(request);
            boost::asio::write(s, boost::asio::buffer(request, request_length));

            char reply[max_length];
            size_t reply_length = boost::asio::read(s,
                boost::asio::buffer(reply, request_length));
            std::cout << "Reply is: ";
            std::cout.write(reply, reply_length);
            std::cout << "\n";
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    std::string ip = "127.0.0.1";
    //std::cout << "Enter the IPv4: ";
    //std::cin >> ip;

    std::string port = "5000";
    //std::cout << "Enter the port: ";
    //std::cin >> port;

    int numOfClients = 0;
    std::cout << "Enter the number of clients (threads): ";
    std::cin >> numOfClients;

    int numOfRW = 0;
    std::cout << "Enter the number of read+write: ";
    std::cin >> numOfRW;

    boost::thread_group threads;
    for (int i = 0; i < numOfClients; ++i) {
        threads.create_thread(boost::bind(connection, ip, port, numOfRW));
        boost::this_thread::sleep(boost::posix_time::millisec(5));
    }
    threads.join_all();
    //boost::asio::connect(s, resolver.resolve(ip, port));

    return 0;
}


