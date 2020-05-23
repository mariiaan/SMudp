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
#include <vector>

namespace SMudp
{
	bool wsaOnline = false; // Indicates if winsocks is initialized

	int Startup(int versionMajor = 2, int versionMinor = 2)
	{
		if (wsaOnline)
		{
			return 0;
		}

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

	inline int Shutdown() // Shutdown winsocks
	{
		if (!wsaOnline)
		{
			return 0;
		}

		wsaOnline = false;
		return WSACleanup();
	}
	
	inline int GetWSAError() // Returns last winsocks error
	{
		return WSAGetLastError();
	}

	namespace UDP
	{
		int Receive(SOCKET& socket, char* buffer, int bufferSize, sockaddr* from)
		{
			int fromSize = sizeof(*from);
			ZeroMemory(buffer, bufferSize);

			int bytesIn = recvfrom(socket, buffer, bufferSize, 0, from, &fromSize);
			if (bytesIn == SOCKET_ERROR)
			{
				return -1;
			}

			return bytesIn;
		}

		int Send(SOCKET& socket, const char* buffer, int count, sockaddr* to)
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

		class UdpHost
		{

		};
		
		class UdpClient
		{

		};
	}

	namespace TCP
	{
		SOCKET CreateListeningSocket(int port, bool loopback = false)
		{
			SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
			if (listening == INVALID_SOCKET)
			{
				return -1;
			}

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

			if (bind(listening, (sockaddr*)serverHint, sizeof(*serverHint)))
			{
				return -1;
			}

			return listening;
		}

		inline int ListenForConnections(SOCKET& listeningSocket)
		{
			return listen(listeningSocket, SOMAXCONN);
		}

		inline sockaddr_in* CreateClient()
		{
			return new sockaddr_in();
		}

		SOCKET AcceptConnection(SOCKET& listeningSocket, sockaddr_in* client)
		{
			int clientSize = sizeof(*client);

			return accept(listeningSocket, (sockaddr*)client, &clientSize);
		}

		void CreateClientInformationBuffers(char** host, char** service)
		{
			*host = new char[NI_MAXHOST];
			*service = new char[NI_MAXSERV];
		}

		inline int FillClientInformationBuffers(sockaddr_in* client, char* host, char* service)
		{
			if (getnameinfo((sockaddr*)client, sizeof(*client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
			{
				return 0;
			}
			return -1;
		}

		inline int CloseSocket(const SOCKET& socket)
		{
			return closesocket(socket);
		}

		int Receive(SOCKET& client, char* buffer, int bufferSize)
		{
			ZeroMemory(buffer, bufferSize);

			int bytesIn = recv(client, buffer, bufferSize, 0);
			if (bytesIn == SOCKET_ERROR)
			{
				return -1;
			}

			return bytesIn;
		}

	    int Send(SOCKET& client, const char* buffer, int count)
		{
			int sendOk = send(client, buffer, count, 0);
			if (sendOk == SOCKET_ERROR)
			{
				return -1;
			}
			return 0;
		}

		SOCKET CreateClientSocket()
		{
			SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
			if (sock == INVALID_SOCKET)
			{
				return -1;
			}
			return sock;
		}

		sockaddr_in* CreateConnectionTarget(const char* address, int port)
		{
			sockaddr_in* target = new sockaddr_in();
			inet_pton(AF_INET, address, &target->sin_addr);
			target->sin_family = AF_INET;
			target->sin_port = htons(port);

			return target;
		}

		inline int ConnectSocket(SOCKET& socket, sockaddr_in* target)
		{
			return connect(socket, (sockaddr*)target, sizeof(*target));
		}

		class TcpClient
		{
		private:
			SOCKET clientSocket = 0;
			sockaddr_in* clientAddress = nullptr;
			sockaddr_in* serverAddress = nullptr;
			char* inetAddr = nullptr;
			char* inetPort = nullptr;

		public:
			TcpClient(SOCKET cSocket, sockaddr_in* address)
			{
				clientSocket = cSocket;
				clientAddress = address;

				CreateClientInformationBuffers(&inetAddr, &inetPort);
				FillClientInformationBuffers(clientAddress, inetAddr, inetPort);
			}

			TcpClient(const char* address, int port)
			{
				clientSocket = CreateClientSocket();
				CreateClientInformationBuffers(&inetAddr, &inetPort);
				FillClientInformationBuffers(clientAddress, inetAddr, inetPort);

				serverAddress = SMudp::TCP::CreateConnectionTarget(address, port);

				int connectionResult = SMudp::TCP::ConnectSocket(clientSocket, serverAddress);
				if (connectionResult == -1)
				{
					throw std::exception("Error connecting to server");
				}
			}

			~TcpClient()
			{
				SMudp::TCP::CloseSocket(clientSocket);

				delete[] inetAddr;
				delete[] inetPort;

				delete clientAddress;
				delete serverAddress;
			}

			const char* GetIPAddress()
			{
				return inetAddr;
			}

			const char* GetPort()
			{
				return inetPort;
			}

			SOCKET GetSocket()
			{
				return clientSocket;
			}

			sockaddr_in* GetAddress()
			{
				return clientAddress;
			}

			void Send(const char* buffer, int count)
			{
				int sendOk = SMudp::TCP::Send(clientSocket, buffer, count);
				if (sendOk == SOCKET_ERROR)
				{
					throw std::exception("socket error");
				}
			}

			int Receive(char* buffer, int bufferSize)
			{
				ZeroMemory(buffer, bufferSize);

				int bytesIn = SMudp::TCP::Receive(clientSocket, buffer, bufferSize);
				if (bytesIn == SOCKET_ERROR)
				{
					throw std::exception("socket error");
				}
				if (bytesIn == 0)
				{
					throw std::exception("socket disconnect");
				}

				return bytesIn;
			}
		};

		class TcpHost
		{
		private:
			std::vector<TcpClient*> connectedClients;
			SOCKET listeningSocket = 0;
			int port = -1;
			bool onlyLoopback = false;

		public:
			TcpHost(int port, bool onlyLoopback)
			{
				this->port = port;
				this->onlyLoopback = onlyLoopback;

				if (!wsaOnline)
				{
					throw std::exception("WSA not online");
				}

				listeningSocket = SMudp::TCP::CreateListeningSocket(port, onlyLoopback);
				if (listeningSocket == -1)
				{
					throw std::exception("Error creating or binding socket");
				}

			}
			~TcpHost()
			{
				CloseSocket(listeningSocket);
				for (auto& i : connectedClients)
				{
					delete i;
				}
			}

			TcpClient* WaitForConnection()
			{
				SMudp::TCP::ListenForConnections(listeningSocket); // Wait for a connection

				sockaddr_in* client = SMudp::TCP::CreateClient(); // Create a structure containing client information
				SOCKET connected = SMudp::TCP::AcceptConnection(listeningSocket, client); // Accept connection, store data in client

				TcpClient* clientStream = new TcpClient(connected, client);
				connectedClients.push_back(clientStream);

				return clientStream;
			}

			std::vector<TcpClient*>* GetConnectedClients()
			{
				return &connectedClients;
			}

			inline SOCKET GetListeningSocket()
			{
				return listeningSocket;
			}

			inline bool GetLoopback()
			{
				return onlyLoopback;
			}

			inline int GetPort()
			{
				return port;
			}
		};
	}
}
