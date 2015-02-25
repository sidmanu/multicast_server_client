Contributors: Chaitanya Goyal, Siddharth Manu

Server <-> Worker client program with multiple clients belonging to potentially different groups. 

How to run?
-----------

1. Build the client & server using "make" (perform a "make clean" if required before "make")
2. Run the server using ./server <server_port_num>
3. Run the clients using ./client <server_port_num> <server_hostname> <csv-mcast-groups-to-join>
4. Generate "sample.csv" file using "python generate_nums.py"
5. Select task in Server CLI menu

The server will display group membership status and client data buffers periodically.
The server can distribute tasks to clients & collate them. (Map-Reduce)

