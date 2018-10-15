#include "tcpUserSocket.h"
#include <memory> 
#include <cstring> 
#include <mutex> 
#include <iostream>


using namespace std; 

//Constructor:
cs457::tcpUserSocket::tcpUserSocket() {};

void cs457::tcpUserSocket::setSocket(int sckt)  
{ 
    userSocket=sckt;
};





socklen_t cs457::tcpUserSocket::getLengthPointer()
{
    socklen_t len = sizeof(userAddress);  
    return len;
};

int cs457::tcpUserSocket::getSocket()
{
    return userSocket;
}; 
int cs457::tcpUserSocket::closeSocket()
{
    return close(userSocket);
}; 

void cs457::tcpUserSocket::setUserInfoIPv4(string address, uint16_t port)
{
    clientAddressIPv4 = address;
    clientPortIPv4 = port; 
}; 







std::tuple<string,ssize_t> cs457::tcpUserSocket::recvString(int bufferSize, bool useMutex)
{
    char stringBuffer[bufferSize]; 
    memset(stringBuffer, 0, sizeof(stringBuffer));    //change made here. Zeros out buffer.

    ssize_t recvMsgSize;

    if (useMutex)
    {        cout <<"gets here"<<std::endl;

        lock_guard<mutex> lock(recvMutex);
        recvMsgSize = recv(userSocket, stringBuffer, bufferSize, 0); 
    }    
    else
    {
        recvMsgSize = recv(userSocket, stringBuffer, bufferSize, 0); 
    }
    
                                                                                                                                                                                                                                                                                    
   
    return make_tuple(string(stringBuffer),recvMsgSize);     
};
        








ssize_t cs457::tcpUserSocket::sendString(const string & data, bool useMutex)
{
    //https://stackoverflow.com/questions/7352099/stdstring-to-char
    if (data.size() == 0) return 0;                 
    int bufferSize = data.size()+1; 
    vector<char> stringBuffer(data.c_str(), data.c_str() + data.size() + 1u);   //Variable size buffer to hold the string value

    //Utilizing mutex for multithreading
    ssize_t rval; 
    if (useMutex)
    {
       lock_guard<mutex> lock(sendMutex);   //attempting to lock the mutex next because the mutex value is currently locked
       rval = send(userSocket, &stringBuffer[0], bufferSize, 0);
    }
    else
    {   //mutex is not locked and message can be sent
       rval = send(userSocket, &stringBuffer[0], bufferSize, 0);     
    }

    return rval;
}







string cs457::tcpUserSocket::getUniqueIdentifier()
{ 
    //this unique identifier is arbitrary 
    //and it may not be useful for chat program (see other classes)
    //it has not been tested to be unique but for now, we don't need it. 
     string identifier  = "[" + clientAddressIPv4 + "," + to_string(clientPortIPv4) + "]"; 
     return identifier; 
};




struct sockaddr * cs457::tcpUserSocket::getAddressPointer()
{
    return ((struct sockaddr *) &userAddress);
};