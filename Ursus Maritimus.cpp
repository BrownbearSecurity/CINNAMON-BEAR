#include <iostream>
#include <fstream>
#include <string>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 12345
#define BUFFER_SIZE 1024

void sendMessage(const char* ip, const char* message) {
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        std::cerr << "Error: Failed to initialize Winsock\n";
        return;
    }

    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Error: Could not create socket\n";
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, ip, &serverAddr.sin_addr);

    if (connect(sockfd, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error: Could not connect to peer\n";
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    send(sockfd, message, strlen(message), 0);
    std::cout << "Message sent successfully\n";

    closesocket(sockfd);
    WSACleanup();
}

void sendFile(const char* ip, const char* filePath) {
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        std::cerr << "Error: Failed to initialize Winsock\n";
        return;
    }

    // Open the file
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file\n";
        WSACleanup();
        return;
    }

    // Get the size of the file
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Allocate memory for the file content
    char* fileBuffer = new char[fileSize];

    // Read the file into the buffer
    if (!file.read(fileBuffer, fileSize)) {
        std::cerr << "Error: Could not read file\n";
        delete[] fileBuffer;
        file.close();
        WSACleanup();
        return;
    }

    file.close();

    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Error: Could not create socket\n";
        delete[] fileBuffer;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, ip, &serverAddr.sin_addr);

    if (connect(sockfd, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error: Could not connect to peer\n";
        delete[] fileBuffer;
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    // Send the file size first
    if (send(sockfd, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0) == SOCKET_ERROR) {
        std::cerr << "Error: Could not send file size\n";
        delete[] fileBuffer;
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    // Send the file content
    if (send(sockfd, fileBuffer, fileSize, 0) == SOCKET_ERROR) {
        std::cerr << "Error: Could not send file data\n";
        delete[] fileBuffer;
        closesocket(sockfd);
        WSACleanup();
        return;
    }

    std::cout << "File sent successfully\n";

    delete[] fileBuffer;
    closesocket(sockfd);
    WSACleanup();
}

void receiveMessage() {
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        std::cerr << "Error: Failed to initialize Winsock\n";
        return;
    }

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Error: Could not create socket\n";
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error: Could not bind socket\n";
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Error: Could not listen on socket\n";
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    std::cout << "Listening for incoming connections...\n";

    while (true) {
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Error: Could not accept incoming connection\n";
            closesocket(listenSocket);
            WSACleanup();
            return;
        }

        char buffer[BUFFER_SIZE];
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::cout << "Received message: " << buffer << std::endl;
        }

        closesocket(clientSocket);
    }

    closesocket(listenSocket);
    WSACleanup();
}

void receiveFile() {
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        std::cerr << "Error: Failed to initialize Winsock\n";
        return;
    }

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Error: Could not create socket\n";
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error: Could not bind socket\n";
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Error: Could not listen on socket\n";
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    std::cout << "Listening for incoming connections...\n";

    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    SOCKET clientSocket = accept(listenSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error: Could not accept incoming connection\n";
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    // Receive the file size first
    std::streamsize fileSize;
    if (recv(clientSocket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0) == SOCKET_ERROR) {
        std::cerr << "Error: Could not receive file size\n";
        closesocket(clientSocket);
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    // Receive the file content
    char buffer[BUFFER_SIZE];
    std::ofstream outputFile("received_file", std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not create output file\n";
        closesocket(clientSocket);
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    std::streamsize bytesReceived = 0;
    while (bytesReceived < fileSize) {
        int bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesRead <= 0) {
            std::cerr << "Error: Connection closed prematurely\n";
            outputFile.close();
            closesocket(clientSocket);
            closesocket(listenSocket);
            WSACleanup();
            return;
        }

        outputFile.write(buffer, bytesRead);
        bytesReceived += bytesRead;
    }

    outputFile.close();
    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();

    std::cout << "File received successfully\n";
}

//ʕ´• ᴥ•̥`ʔ UTF8
void wprint(std::wstring message)
{
#ifdef _WIN32
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD n_written;
    WriteConsoleW(handle, message.c_str(), (DWORD)message.size(), &n_written, NULL);
#else
    std::wcout << message;
#endif
}

int main() {
    int choice;

    do {
        std::cout << "============================================================================================\n";
        std::cout << "                                  C I N N A M O N B E A R\n";
        std::cout << "                                    -------------------";
        std::cout << " \n";
        wprint(L"                                         \u0295\u00b4\u2022\u0020\u1d25\u2022\u0325\u0060\u0294\n");
        std::cout << " \n";
        std::cout << "============================================================================================\n";
        std::cout << "   \n";
        std::cout << " [1] SEND MESSAGE\n";
        std::cout << " [2] SEND FILE\n";
        std::cout << " [3] RECEIVE MESSAGE\n";
        std::cout << " [4] RECEIVE FILE\n";
        std::cout << " \n";
        std::cout << " [5] EXIT\n";
        std::cout << " \n";
        std::cout << "   \n";
        std::cout << "============================================================================================\n";
        std::cout << "                        Enter your choice and press enter: \n";
        std::cout << "                 USE A TOOL LIKE BROWNBEAR TO ENCRYPT YOUR MESSAGES\n";
        std::cout << "============================================================================================\n";
        std::cout << "   \n";
        std::cin >> choice;
        std::cin.ignore(); // Clear the input buffer

        switch (choice) {
        case 1:
        {
            std::string recipientIp, message;
            std::cout << "Enter recipient's IP address: ";
            std::getline(std::cin, recipientIp);
            std::cout << "Enter message: ";
            std::getline(std::cin, message);
            sendMessage(recipientIp.c_str(), message.c_str());
        }
        break;
        case 2:
        {
            std::string recipientIp, filePath;
            std::cout << "Enter recipient's IP address: ";
            std::getline(std::cin, recipientIp);
            std::cout << "Enter path to the file: ";
            std::getline(std::cin, filePath);
            sendFile(recipientIp.c_str(), filePath.c_str());
        }
        break;
        case 3:
            receiveMessage();
            break;
        case 4:
            receiveFile();
            break;
        case 5:
            std::cout << "Exiting application...\n";
            break;
        default:
            std::cout << "Invalid choice. Please enter a valid option.\n";
            break;
        }
    } while (choice != 5);

    return 0;
}