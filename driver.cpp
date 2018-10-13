#include <iostream>
#include <string> 
#include <tuple> 
#include <thread> 
#include <vector> 
#include <memory> 
#include "tcpUserSocket.h"
#include "tcpServerSocket.h"

using namespace std;

bool ready = true; 




int cclient(shared_ptr<cs457::tcpUserSocket> clientSocket,int id) //id is the identifier of the client SOCKET sending a msg
{

    cout << "Waiting for message from Client Thread" << id << std::endl;
    string msg;
    ssize_t val;
    bool cont =true ;  


    while (cont) //Set to true above and changed to false when message comes from SERVER?
    {
        tie(msg,val) = clientSocket.get()->recvString();
        if (msg.substr(0,4) == "EXIT")
            cont = false; 
       
        cout << "[SERVER] The client is sending message " << msg << " -- With value return = " << val << endl;
        string s =  "[SERVER REPLY] The client is sending message:" + msg  + "\n"; 
        thread childT1(&cs457::tcpUserSocket::sendString,clientSocket.get(),s,true); //send the string in msg back to the client 


        //thread childT2(&cs457::tcpUserSocket::sendString,clientSocket.get(),msg,true);
        //thread childT3(&cs457::tcpUserSocket::sendString,clientSocket.get(),"\n",true);
        
        childT1.join();
        //childT2.join(); 
        //childT3.join(); 
        //clientSocket.get()->sendString(msg); 
        //clientSocket.get()->sendString("\n"); 

         if (msg.substr(0,6) == "SERVER")
        {
            thread childTExit(&cs457::tcpUserSocket::sendString,clientSocket.get(),"GOODBYE EVERYONE",false);
            thread childTExit2(&cs457::tcpUserSocket::sendString,clientSocket.get(),"\n",false);
            ready = false;   
            cont = false;   
            childTExit.join(); 
            childTExit2.join();
        }
        else
        {
            cout << "waiting for another message" << endl; 
        }
    }

    clientSocket.get()->sendString("goodbye"); 
    
    clientSocket.get()->closeSocket(); 
    return 1; 
}









int main(int argc, char * argv[])
{
    cout << "Initializing Socket" << std::endl; 
    cs457::tcpServerSocket mysocket(2000);  //Set up a TCP socket on port 2000 (FOR SERVER)
    cout << "Binding Socket" << std::endl; 
    mysocket.bindSocket(); //Bind the created SERVER socket "mysocket"
    cout << "Listening Socket" << std::endl; 
    mysocket.listenSocket(); //Listen for messages?
    cout << "Waiting to Accept Socket" << std::endl;
    int id = 0; 
    vector<unique_ptr<thread>> threadList; 
  

    while (ready) //CREATING MULTIPLE CLIENTS TO INTERACT WITH THE SERVER
    { 
        cout <<"gets here"<<std::endl;
        //**NEED A CHECK FOR VAL <0
        shared_ptr<cs457::tcpUserSocket> clientSocket; //CREATING NEW USER/CLIENT SOCKET FOR SERVER - **should come from parsing an argument from main! 
        int val; //val is going to be the socketFileDescriptor after the tuple call below:
        tie(clientSocket,val) = mysocket.acceptSocket(); //creating a tuple and accepting the socket 
        cout << "value for accept is " << val << std::endl; //val is now socketFileDescriptor - because everything is a file on linux
        cout << "Socket Accepted" << std::endl; 

        unique_ptr<thread> t = make_unique<thread>(cclient,clientSocket,id); //Calling the method above to communicate between users and server

        threadList.push_back(std::move(t)); //adding this new socket to the list above of active sockets - why do we need this?
        
        id++; //not the best way to go about it. 
       // threadList.push_back(t); 
       
    
        
    }



    for (auto& t: threadList)
    {
        t.get()->join();    //joining all of the active sockets in threadlist vector together
    }
    
        
    cout << "Server is shutting down after one client" << endl; 
    return 0; 
}