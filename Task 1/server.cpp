#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

void startServer(int port);

int main(int argc, char* argv[]){
    startServer(stoi(argv[1]));
    return 0;
}

void startServer(int port) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0) {
        cerr << "Error creating socket" << endl;
        return;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Error binding socket" << endl;
        close(serverSocket);
        return;
    }

    if (listen(serverSocket, 5) < 0) {
        cerr << "Error listening on socket" << endl;
        close(serverSocket);
        return;
    }

    cout << "Server listening on port " << port << endl;

    sockaddr_in clientAddress;
    socklen_t clientLen = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientLen);
    
    if (clientSocket < 0) {
        cerr << "Error accepting connection" << endl;
        return;
    }
    
    while (true) {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);

        if (bytesRead < 0) {
            cerr << "Error reading from socket" << endl;
            close(clientSocket);
            continue;
        }

        string message(buffer);

        if(message == "terminate"){
            close(clientSocket);
            return;
        }else{
            cout << message << endl;
        }
        // const char* response = "Message received";
        // write(clientSocket, response, strlen(response));

        
    }
}