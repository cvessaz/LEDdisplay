//
//  remoteControl.cpp
//  displayWrapper
//
//  Created by Christian Vessaz on 17.01.18.
//  Copyright © 2018 Christian Vessaz. All rights reserved.
//

#include <arpa/inet.h>
#include "global.h"
#include "remoteControl.h"

void communicate() {
  //initialize socket and structure
  int socket_info;
  struct sockaddr_in server;
  char incoming_message[100];

  //create socket
  socket_info = socket(AF_INET, SOCK_DGRAM, 0);
  if (socket_info == -1) {
    perror("Could not create socket...\n");
  }
  
  // Set timout
  struct timeval timeout={3,0}; //set timeout for 3 seconds
  setsockopt(socket_info,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));

  //assign values
#ifdef __APPLE__
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
#else
  server.sin_addr.s_addr = inet_addr("10.0.0.1");
#endif
  server.sin_family = AF_INET;
  server.sin_port = htons( 1111 );

  //checks connection
  if (bind(socket_info, (struct sockaddr *)&server, sizeof(server)) < 0) {
    perror("Connection error...\n");
  }

  //Receive an incoming message
  while (true) {
    // exit
    if (params.isStopped) {
      break;
    }
    
    // Wait for message
    if (recv(socket_info, incoming_message, sizeof(incoming_message), 0) > 0) {
      std::string cmd = incoming_message;
      int cmdLength = (int)cmd.length();
      if (cmdLength<=0) continue;
      
      // Assign params
      switch (cmd[0]) {
        // Received message
        case 'm':
          if (cmdLength>2 && cmd[1]=='/') {
            mu.lock();
            params.text = cmd.substr(2, cmd.size()-2);
            params.isCleared = true;
            mu.unlock();
          }
          break;
         
        // Received color
        case 'c':
          if (cmdLength==13 && cmd[1]=='/' && cmd[5]=='/' && cmd[9]=='/') {
            std::string rs, gs, bs;
            for (int i=0; i<3; ++i) {
              rs.push_back(cmd[2+i]);
              gs.push_back(cmd[6+i]);
              bs.push_back(cmd[10+i]);
            }
            int r = std::min(255,std::max(0,std::stoi(rs)));
            int g = std::min(255,std::max(0,std::stoi(gs)));
            int b = std::min(255,std::max(0,std::stoi(bs)));
            mu.lock();
            params.color = Color(r,g,b);
            params.isCleared = true;
            mu.unlock();
          }
          break;
          
        // Received speed
        case 's':
          if (cmdLength==5 && cmd[1]=='/') {
            std::string ss;
            for (int i=0; i<3; ++i) {
              ss.push_back(cmd[2+i]);
            }
            int s = std::min(100.0,std::max(0.0,std::stod(ss)));
            mu.lock();
            params.speed = s;
            mu.unlock();
          }
          break;
          
        // Received restart
        case 'r':
          if (cmdLength==1) {
            mu.lock();
            params.isCleared = true;
            mu.unlock();
          }
          break;
          
        // Received stop
        case 'p':
          if (cmdLength==1) {
            mu.lock();
            params.isStopped = true;
            mu.unlock();
          }
          break;
          
        default:
          break;
      }
    }
  }
}
