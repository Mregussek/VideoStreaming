//   Written by Mateusz Rzeczyca.
//   Student - AGH University of Science and Technology
//   rzeczyca@student.agh.edu.pl
//   09.05.2019

#include "UDP.h"

namespace mrz
{
    UDPserver::UDPserver() {}

    void UDPserver::init_object(const char* set_port)
    {
        sock_system_call = new int;
        port = new uint16_t;
        broadcast_permission = new int(1);
        server = new sockaddr_in;
        client = new sockaddr_in;
        address_length = new socklen_t( sizeof(sockaddr_in) );
        recv_message = new int;
        max_recv_message = new int(4096);

        char_to_uint16(set_port, port);
    }

    UDPserver::~UDPserver()
    {
        delete max_recv_message;
        delete recv_message;
        delete address_length;
        delete client;
        delete server;
        delete broadcast_permission;
        delete port;
        delete sock_system_call;
    }

    void UDPserver::define_socket()
    {
        *sock_system_call = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

        if(*sock_system_call < 0)
        {
            std::cerr << "Cannot create UDP server socket!" << std::endl;
            exit(1);
        }

        server ->sin_family = AF_INET;
        server ->sin_addr.s_addr = htonl(INADDR_ANY);
        server ->sin_port = htons(*port);
    }

    void UDPserver::create_server_then_listen()
    {
        auto conv = reinterpret_cast<sockaddr*>(server);

        int result = bind(*sock_system_call, conv, *address_length);

        if(result < 0)
        {
            std::cerr << "Cannot bind UDP server socket!" << std::endl;
            exit(1);
        }

        setsockopt(*sock_system_call, SOL_SOCKET, SO_BROADCAST,
                   (void*) broadcast_permission, sizeof(int) );

        std::cout << "UDP-based Server Started!\n" <<
                  "Server Address: " << inet_ntoa(server ->sin_addr) <<
                  " Server Port: " << *port << "\n";
    }

    void UDPserver::receive_data(void* buffer, int buffer_length)
    {
        auto conv = reinterpret_cast<sockaddr*>(client);

        *recv_message = recvfrom(*sock_system_call, buffer,
                                     buffer_length, 0, conv, address_length);

        if(*recv_message < 0)
        {
            std::cerr << "Cannot receive data from client!" << std::endl;
            exit(1);
        }
    }

    void UDPserver::close_connection()
    {
        close(*sock_system_call);
    }
}