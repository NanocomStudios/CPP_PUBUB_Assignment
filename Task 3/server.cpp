#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <vector>
#include <thread>
#include <unordered_map>

#include <mutex>

#include <regex>

using namespace std;

unordered_map<string, vector<int>> subscribers;

mutex subMutex;

mutex publishMutex;

void handleClient(int clientSocket){
    bool isInit = false;
    string topic = "";

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

        if(!isInit){

            regex pattern(R"((PUBLISHER|SUBSCRIBER) (\w+))");
            smatch matches;

            if (regex_match(message, matches, pattern)) {
                string clientType = matches[1];
                topic = matches[2];

                if(clientType == "PUBLISHER"){
                    cout << "New publisher connected for topic: " << topic << endl;
                }else if(clientType == "SUBSCRIBER"){
                    subMutex.lock();
                    subscribers[topic].push_back(clientSocket);
                    cout << "New subscriber connected for topic: " << topic << endl;
                    subMutex.unlock();
                    return;
                }
            } else {
                cout << "Invalid client type or topic format" << endl;
                close(clientSocket);
                return;
            }

            isInit = true;
            continue;
        }
        

        if(message == "terminate"){
            close(clientSocket);
            return;
        }else{
            
            publishMutex.lock();
            for(auto sub: subscribers[topic]){
                send(sub, message.c_str(), message.length(), 0);
            }
            publishMutex.unlock();

        }
    }
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

    while(1){
        sockaddr_in clientAddress;
        socklen_t clientLen = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientLen);

        if (clientSocket < 0) {
            cerr << "Error accepting connection" << endl;
            return;
        }

        thread t([clientSocket](){
            handleClient(clientSocket);
        });
        t.detach();
    }
    
    
}

int main(int argc, char* argv[]){
    startServer(stoi(argv[1]));
    return 0;
}