#include <winsock2.h>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

#define MAX_CLIENTS 10
#define DEFAULT_BUFLEN 4096

#pragma comment(lib, "ws2_32.lib") // Winsock library
#pragma warning(disable:4996)

SOCKET server_socket;

vector<string> "history";

int main() {
	system("title Server");

	puts("Start server... DONE.");
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("Failed. Error Code: %d", WSAGetLastError());
		return 1;
	}

	// create a socket
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		printf("Could not create socket: %d", WSAGetLastError());
		return 2;
	}
	// puts("Create socket... DONE.");

	// prepare the sockaddr_in structure
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);

	// bind socket
	if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		printf("Bind failed with error code: %d", WSAGetLastError());
		return 3;
	}

	// puts("Bind socket... DONE.");

	// listen to incoming connections
	listen(server_socket, MAX_CLIENTS);

	// accept and incoming connection
	puts("Server is waiting for incoming connections...\nPlease, start one or more client-side app.");

	// size of our receive buffer, this is string length
	// set of socket descriptors
	// fd means "file descriptors"
	fd_set readfds; // https://docs.microsoft.com/en-us/windows/win32/api/winsock/ns-winsock-fd_set
	SOCKET client_socket[MAX_CLIENTS] = {};

	while (true) {
		// clear the socket fdset
		FD_ZERO(&readfds);

		// add master socket to fdset
		FD_SET(server_socket, &readfds);

		// add child sockets to fdset
		for (int i = 0; i < MAX_CLIENTS; i++) {
			SOCKET s = client_socket[i];
			if (s > 0) {
				FD_SET(s, &readfds);
			}
		}

		// wait for an activity on any of the sockets, timeout is NULL, so wait indefinitely
		if (select(0, &readfds, NULL, NULL, NULL) == SOCKET_ERROR) {
			printf("select function call failed with error code : %d", WSAGetLastError());
			return 4;
		}

		// if something happened on the master socket, then its an incoming connection
		SOCKET new_socket; // new client socket
		sockaddr_in address;
		int addrlen = sizeof(sockaddr_in);
		if (FD_ISSET(server_socket, &readfds)) {
			if ((new_socket = accept(server_socket, (sockaddr*)&address, &addrlen)) < 0) {
				perror("accept function error");
				return 5;
			}

			for (int i = 0; i < history.size(); i++)
			{
				cout << history[i] << "\n";
				send(new_socket, history[i].c_str(), history[i].size(), 0);
			}

			// inform server side of socket number - used in send and recv commands
			printf("New connection, socket fd is %d, ip is: %s, port: %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

			// add new socket to array of sockets
			for (int i = 0; i < MAX_CLIENTS; i++) {
				if (client_socket[i] == 0) {
					client_socket[i] = new_socket;
					printf("Adding to list of sockets at index %d\n", i);
					break;
				}
			}
		}

		// if some of client sockets sends something
		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			SOCKET s = client_socket[i];
			// if client presend in read sockets
			if (FD_ISSET(s, &readfds))
			{
				// get details of the client
				getpeername(s, (sockaddr*)&address, (int*)&addrlen);

				// check if it was for closing, and also read the incoming message
				// recv does not place a null terminator at the end of the string (whilst printf %s assumes there is one)

				char client_message[DEFAULT_BUFLEN];

				int client_message_length = recv(s, client_message, DEFAULT_BUFLEN, 0);
				client_message[client_message_length] = '\0';

				string check_exit = client_message;
				if (check_exit == "off")
				{
					cout << "Client #" << i << " is off\n";
					client_socket[i] = 0;
				}

				string temp = client_message;
				// temp += "\n";
				history.push_back(temp);

				for (int i = 0; i < MAX_CLIENTS; i++) {
					if (client_socket[i] != 0) {
						send(client_socket[i], client_message, client_message_length, 0);
					}
				}

			}
		}
	}

	WSACleanup();
}