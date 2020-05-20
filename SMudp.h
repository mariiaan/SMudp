/*  
	SMudp - A simple and lightweight network library
	"SMudp.h" - (C) Copyright 2020, samplefx

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

#pragma once

#ifndef __WS2_32_LIB__
#define __WS2_32_LIB__
#pragma comment(lib, "ws2_32.lib") // ws2_32 library is requiered for winsocks
#endif

#include <WS2tcpip.h>

namespace SMudp
{
	bool wsaOnline = false; // Indicates if winsocks is initialized

	int Startup(int versionMajor = 2, int versionMinor = 2)
	{
		WORD version = MAKEWORD(versionMajor, versionMinor);
		WSAData startupData;
		int wsStatus = WSAStartup(version, &startupData);
		if (wsStatus != 0)
		{
			return -1;
		}

		wsaOnline = true;
		return 0;
	}

	inline int Shutdown() // Shutdown Winsocks
	{
		wsaOnline = false;
		return WSACleanup();
	}
	
	inline int GetWSAError() // Returns last winsocks Error
	{
		return WSAGetLastError();
	}

	int Receive(SOCKET socket, char* buffer, int bufferSize, sockaddr* from)
	{
		int fromSize = sizeof(*from);
		int bytesIn = recvfrom(socket, buffer, bufferSize, 0, from, &fromSize);
		if (bytesIn == SOCKET_ERROR)
		{
			return -1;
		}

		return bytesIn;
	}

	int Send(SOCKET socket, const char* buffer, int count, sockaddr* to)
	{
		int toSize = sizeof(*to);
		int sendOk = sendto(socket, buffer, count, 0, to, toSize);
		if (sendOk == SOCKET_ERROR)
		{
			return -1;
		}

		return sendOk;
	}
	
	SOCKET CreateHostSocket(int port, bool loopback = false) // Creates a socket and binds it to an address and a port
	{
		SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
		sockaddr_in* serverHint = new sockaddr_in();
		if (loopback)
		{
			serverHint->sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
		}
		else
		{
			serverHint->sin_addr.S_un.S_addr = INADDR_ANY;
		}
		serverHint->sin_family = AF_INET;
		serverHint->sin_port = htons(port);

		if (bind(sock, (sockaddr*)serverHint, sizeof(*serverHint)) == SOCKET_ERROR)
		{
			return -1;
		}		
		
		return sock;
	}

	sockaddr_in* CreateTarget(const char* address, int port) // Creates a structure defining data target
	{
		sockaddr_in* target = new sockaddr_in();
		target->sin_family = AF_INET;
		target->sin_port = htons(port);
		inet_pton(AF_INET, address, &target->sin_addr);

		return target;
	}

	inline SOCKET CreateClientSocket() // Creates a socket for the client
	{
		return socket(AF_INET, SOCK_DGRAM, 0);
	}

	inline int CloseSocket(SOCKET socket)
	{
		return closesocket(socket);
	}
}
