Contributors: Chaitanya Goyal, Aishvarya Gupta, Siddharth Manu, Preetham Nanjappa

As part of a system programming course, this is a simple client/server program. Over the next 3-4 months,
this repository will be updated to make a distributed program which handles large work requests, which it
shares with multiple client threads.

How to run?
-----------

1. Build the client & server using "make"
2. Run the server using ./server <port_num>
3. Run the client using ./client <port_num> <server_hostname>
4. Select the multicast group for the client to join. Add additional messages.

The server will continue to cater to all clients and will periodically display all messages
