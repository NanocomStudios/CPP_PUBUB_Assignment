#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

void startClient(string ip, int port);

int main(int argc, char* argv[]) {
    startClient(argv[1], stoi(argv[2]));
    return 0;
}

void startClient(string ip, int port){
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip.c_str(), &serverAddress.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return;
    }


    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Error connecting to server" << std::endl;
        close(clientSocket);
        return ;
    }

    while(1){
        string message;
        getline(cin, message);

        send(clientSocket, message.c_str(), message.length(), 0);

        if(message == "terminate"){
            close(clientSocket);
            break;
        }
    }
}