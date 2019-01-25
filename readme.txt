We have to read data from DHT11 that is client.
Send the data to the server .If temperature is greater than 30 then reply critical.

Note:
1.Port Number in Server and Client are same
    	In server_DHT11_Temp.java
	7808 is the port no.
	ServerSocket ss=new ServerSocket(7808);

        In DHT11_Temp_client.ino
	#define port 7808      

2.IP address is that of Server 

	In DHT11_Temp_client.ino
        byte server[] = {13,233,237,38};
make sure that IP address is seperated by comma