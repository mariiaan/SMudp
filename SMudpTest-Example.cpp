/*
	SMudp - A simple and lightweight network library
	"SMudpTest.cpp" - (C) Copyright 2020, samplefx

	Copyright 2020 samplefx

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation 
	files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, 
	modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
	Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
	WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
	COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
	ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "SMudp.h"
#include <string>
#include <iostream>

#pragma region UDP
// A sample client example
void UDPClient()
{
	std::cout << "Starting client...";
	SMudp::Startup();
	std::string buf; // Create input buffer

	char responseBuf[1024]; // Create response buffer
	
	SOCKET sock = SMudp::UDP::CreateClientSocket(); // Create socket for the client
	if (sock == -1)
	{
		std::cout << "Error binding or creating socket" << std::endl;
		std::cout << SMudp::GetWSAError() << std::endl;
		return; 
	}
	sockaddr_in* target = SMudp::UDP::CreateTarget("127.0.0.1", 54000); // Create the target / information about the server

	std::cout << "Ready for input!" << std::endl;
	while (true)
	{
		std::cin >> buf; // Input some characters
		int sendOk = SMudp::UDP::Send(sock, buf.c_str(), buf.size() + 1, (sockaddr*)target); // Send input data to the target

		ZeroMemory(responseBuf, 1024); // Prepare buffer for response

		int bytesIn = SMudp::UDP::Receive(sock, responseBuf, 1024, (sockaddr*)target); // Receive data into response buffer
		if (bytesIn == -1)
		{
			std::cout << "Error receiving" << std::endl;
			std::cout << SMudp::GetWSAError() << std::endl;
			continue;
		}
		std::cout << responseBuf << std::endl; // Display server response
	}

	SMudp::Shutdown();
}

// A sample server example
void UDPServer()
{
	std::cout << "Starting server...";

	SMudp::Startup(); // Startup Winsocks

	// Create socket for the server on port 54000 for loopback only
	SOCKET sock = SMudp::UDP::CreateHostSocket(54000, true); 

	sockaddr_in client; // Create data buffer for receiving containing client information
	int clientSize = sizeof(client);
	ZeroMemory(&client, clientSize);

	char receiveBuf[1024]; // Create data buffer for receiving containing received data
	std::string response = "okay!"; 

	std::cout << "Now receiving data." << std::endl;
	while (true)
	{
		ZeroMemory(receiveBuf, 1024); // Prepare receive data buffer

		int bytesIn = SMudp::UDP::Receive(sock, receiveBuf, 1024, (sockaddr*)&client); // Receive data into buffer
		if (bytesIn == -1)
		{
			std::cout << "Error receiving" << std::endl;
			std::cout << SMudp::GetWSAError() << std::endl;
			continue;
		}

		std::cout << "Message: " << receiveBuf << std::endl; // Display received data

		int sendOk = SMudp::UDP::Send(sock, response.c_str(), response.size(), (sockaddr*)&client); // Respond by sending "okay!"
		if (sendOk == -1)
		{
			std::cout << "Error sending/responding" << std::endl;
			std::cout << SMudp::GetWSAError() << std::endl;
		}
	}

	SMudp::Shutdown(); // We won't get to this but anyway
}
#pragma endregion

#pragma region TCP

void TCPClient()
{
	SMudp::Startup();

	SOCKET sock = SMudp::TCP::CreateClientSocket(); // Create a socket for the client
	if (sock == -1)
	{
		std::cout << "Error creating socket" << std::endl;
		std::cout << SMudp::GetWSAError() << std::endl;
		SMudp::Shutdown();
		return;
	}
	sockaddr_in* server = SMudp::TCP::CreateConnectionTarget("127.0.0.1", 54000);
	int connectionResult = SMudp::TCP::ConnectSocket(sock, server); // Connect socket to server

	if (connectionResult == -1)
	{
		std::cout << "Error connection to server" << std::endl;
		std::cout << SMudp::GetWSAError() << std::endl;
		SMudp::Shutdown();
		return;
	}

	std::cout << "Connected" << std::endl;

	char responseBuf[1024];
	std::string userInput;

	while (true)
	{
		std::cin >> userInput; // Get some input
		ZeroMemory(responseBuf, 1024); // Prepare our response buffer

		SMudp::TCP::Send(sock, userInput.c_str(), userInput.size() + 1); // Send inputted data
		int bytesIn = SMudp::TCP::Receive(sock, responseBuf, 1024); // Receive data from server
		if (bytesIn == 0)
		{
			std::cout << "Disconnected" << std::endl;
			break;
		}
		else if (bytesIn == -1)
		{
			int wsaErr = SMudp::GetWSAError();
			if (wsaErr == 10054)
			{
				std::cout << "Connection Reset by peer" << std::endl;
				break;
			}
			std::cout << wsaErr << std::endl;
		}

		std::cout << responseBuf << std::endl; // Finally: Display data
	}

	SMudp::TCP::CloseSocket(sock);

	SMudp::Shutdown();
}

void TCPServer()
{
	SMudp::Startup();

	// Create a listening socket for incoming connections
	SOCKET listening = SMudp::TCP::CreateListeningSocket(54000);
	if (listening == -1)
	{
		std::cout << "Error binding or creating socket" << std::endl;
		std::cout << SMudp::GetWSAError() << std::endl;
		return;
	}

	SMudp::TCP::ListenForConnections(listening); // Wait for a connection

	sockaddr_in* client = SMudp::TCP::CreateClient(); // Create a structure containing client information
	SOCKET connected = SMudp::TCP::AcceptConnection(listening, client); // Accept connection, store data in client

	char* host = nullptr;
	char* service = nullptr;

	SMudp::TCP::CreateClientInformationBuffers(&host, &service); // Create buffers for IP Address / Port
	int result = SMudp::TCP::FillClientInformationBuffers(client, host, service); // Get IP Address / Port
	if (result == -1)
	{
		std::cout << "Address resolution error" << std::endl;
		std::cout << SMudp::GetWSAError() << std::endl;
	}

	std::cout << "Connection from " << host << " on " << service << std::endl;

	char buffer[1024];

	while (true)
	{
		ZeroMemory(buffer, 1024); // Prepare data buffer for incoming data

		int bytesIn = SMudp::TCP::Receive(connected, buffer, 1024); // Receive data into buffer
		if (bytesIn == -1)
		{
			std::cout << "Error receiving" << std::endl;
			std::cout << SMudp::GetWSAError() << std::endl;
		}
		if (bytesIn == 0)
		{
			std::cout << "Client disconnect" << std::endl;
			break;
		}
		else if (bytesIn == -1)
		{
			int wsaErr = SMudp::GetWSAError();
			if (wsaErr == 10054)
			{
				std::cout << "Connection Reset by peer" << std::endl;
				break;
			}
			std::cout << wsaErr << std::endl;
		}

		int sendOk = SMudp::TCP::Send(connected, buffer, bytesIn); // Send back data
		if (sendOk == -1)
		{
			std::cout << "Error sending" << std::endl;
			std::cout << SMudp::GetWSAError() << std::endl;
		}
	}
	// We won't get here but anyway

	// Close all sockets
	SMudp::TCP::CloseSocket(connected); 
	SMudp::TCP::CloseSocket(listening);

	// Shutdown winsocks
	SMudp::Shutdown();
}

#pragma endregion

#pragma region Object Oriented

void OOTcpClient()
{
	SMudp::Startup();

	try
	{
		SMudp::TCP::TcpClient client("127.0.0.1", 54000); // Connect to localhost on 54000
		std::string input; // Prepare a input buffer
		char buffer[1024]; // Prepare a response buffer

		while (true)
		{
			std::cin >> input; // Read some input
			try
			{
				client.Send(input.c_str(), input.size() + 1); // Send input to server
			}
			catch (std::exception & ex)
			{
				std::cout << "Error: " << ex.what() << std::endl;
				break;
			}

			try
			{
				client.Receive(buffer, 1024); // Receive response
			}
			catch (std::exception & ex)
			{
				std::cout << "Error: " << ex.what() << std::endl;
				break;
			}

			std::cout << buffer << std::endl; // Display response
		}
	}
	catch (std::exception& ex)
	{
		std::cout << "Error: " << ex.what() << std::endl;
	}
	// TcpClient's socket will automatically be closed

	SMudp::Shutdown();
}

void OOTcpServer()
{
	SMudp::Startup();

	SMudp::TCP::TcpHost host(54000, false); // Host a server on *:54000
	SMudp::TCP::TcpClient* client = host.WaitForConnection(); // Wait for a single TCP connection and accept it

	std::cout << "Client connected: " << client->GetIPAddress() << " on " << client->GetPort() << std::endl;

	char buffer[1024]; // A buffer for receiving data

	while (true)
	{
		int bytesIn = 0;
		try
		{
			bytesIn = client->Receive(buffer, 1024);
		}
		catch (std::exception& ex)
		{
			std::cout << "Error: " << ex.what() << std::endl;
			break;
		}
		std::cout << "Message from client: " << buffer << std::endl;

		try
		{
			client->Send(buffer, bytesIn); // Reply by echoing back
		}
		catch (std::exception& ex)
		{
			std::cout << "Error: " << ex.what() << std::endl;
			break;
		}	
	}

	// All sockets are automatically closed when "host" runs out of scope

	SMudp::Shutdown(); // Shutdown winsocks
}

#pragma endregion

// Our application entry point
int main(int argc, char* argv[])
{
	// Basic mode selection
	while (true)
	{
		std::cout << "Select Mode:" << std::endl;
		std::cout << "\t1: UDP: Client" << std::endl;
		std::cout << "\t2: UDP: Server" << std::endl;
		std::cout << "\t3: TCP: Client" << std::endl;
		std::cout << "\t4: TCP: Server" << std::endl;

		std::cout << "Input: ";

		// Inputting into int is not safe and invalid like a string will result in unwanted behaviour but anyway
		int selection = 0;
		std::cin >> selection;
		std::cout << std::endl;

		switch (selection)
		{
		case 1:
			UDPClient();
			return 0;
			break;
		case 2:
			UDPServer();
			return 0;
			break;
		case 3:
			OOTcpClient();
			return 0;
			break;
		case 4:
			OOTcpServer();
			return 0;
			break;
		default:
			std::cout << "Invalid Selection!\n" << std::endl;
			break;
		}	
	}

	std::cout << "exit 0" << std::endl;
	std::cin.get();
	return 0;
}
