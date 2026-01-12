#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

void startClient(string ip, int port, string type){
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

    send(clientSocket, type.c_str(), type.length(), 0);

    if(type == "PUBLISHER"){

        while(1){
            string message;
            getline(cin, message);

            send(clientSocket, message.c_str(), message.length(), 0);

            if(message == "terminate"){
                close(clientSocket);
                break;
            }
        }
    }else{
        while(1){
            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));
            ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);

            if (bytesRead < 0) {
                cerr << "Error reading from socket" << endl;
                close(clientSocket);
                continue;
            }

            string message(buffer);
            cout << message << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    startClient(argv[1], stoi(argv[2]), argv[3]);
    return 0;
}