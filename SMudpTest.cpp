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

// A sample client example
void Client()
{
	std::cout << "Starting client...";
	SMudp::Startup();
	std::string buf; // Create input buffer

	char responeBuf[1024]; // Create response buffer
	
	SOCKET sock = SMudp::CreateClientSocket(); // Create socket for the client
	sockaddr_in* target = SMudp::CreateTarget("127.0.0.1", 54000); // Create the target / information about the server

	std::cout << "Ready for input!" << std::endl;
	while (true)
	{
		std::cin >> buf; // Input some characters
		int sendOk = SMudp::Send(sock, buf.c_str(), buf.size(), (sockaddr*)target); // Send input data to the target

		ZeroMemory(responeBuf, 1024); // Prepare buffer for response

		int bytesIn = SMudp::Receive(sock, responeBuf, 1024, (sockaddr*)target); // Receive data into response buffer
		if (bytesIn == -1)
		{
			std::cout << "Error receiving" << std::endl;
			continue;
		}
		std::cout << responeBuf << std::endl; // Display server response
	}

	SMudp::Shutdown();
}

// A sample server example
void Server()
{
	std::cout << "Starting server...";

	SMudp::Startup(); // Startup Winsocks

	// Create socket for the server on port 54000 for loopback only
	SOCKET sock = SMudp::CreateHostSocket(54000, true); 

	sockaddr_in client; // Create data buffer for receiving containing client information
	int clientSize = sizeof(client);
	ZeroMemory(&client, clientSize);

	char receiveBuf[1024]; // Create data buffer for receiving containing received data
	std::string response = "okay!"; 

	std::cout << "Now receiving data." << std::endl;
	while (true)
	{
		ZeroMemory(receiveBuf, 1024); // Prepare receive data buffer

		int bytesIn = SMudp::Receive(sock, receiveBuf, 1024, (sockaddr*)&client); // Receive data into buffer
		if (bytesIn == -1)
		{
			std::cout << "Error receiving" << std::endl;
			continue;
		}

		std::cout << "Message: " << receiveBuf << std::endl; // Display received data

		int sendOk = SMudp::Send(sock, response.c_str(), response.size(), (sockaddr*)&client); // Respond by sending "okay!"
		if (sendOk == -1)
		{
			std::cout << "Error sending/responding" << std::endl;
		}
	}

	SMudp::Shutdown(); // We won't get to this but anyway
}

// Our application entry point
int main(int argc, char* argv[])
{
	// Basic mode selection
	while (true)
	{
		std::cout << "Select Mode:" << std::endl;
		std::cout << "\t1: Client" << std::endl;
		std::cout << "\t2: Server" << std::endl;

		std::cout << "Input: ";

		// Inputting into int is not safe and invalid input like a string will result in unwanted behaviour but anyway
		int selection = 0;
		std::cin >> selection;
		std::cout << std::endl;

		switch (selection)
		{
		case 1:
			Client();
			return 0;
			break;
		case 2:
			Server();
			return 0;
			break;
		default:
			std::cout << "Invalid Selection!\n" << std::endl;
			break;
		}	
	}
	
	return 0;
}
