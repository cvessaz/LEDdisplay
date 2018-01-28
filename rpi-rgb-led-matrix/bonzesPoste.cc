#include "led-matrix.h"
#include "graphics.h"

#include <unistd.h>
#include <ctime>
#include <map>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>

using namespace rgb_matrix;

struct Params {
  float t=0.0;
  float tMax=60.0;
  float waitMin=1.0;
  float waitMax=5.0;
  int i=0;
  int N=100;
  bool paused=false;
  Color color=Color(255, 0, 0);
};
Params params;
std::string fontName="10x20.bdf";
int desk[4] = {0,0,0,0};
char label[4] = {'A','B','C','D'};
std::map<int,std::string> specialChar = {
  {9, "  √81"},
  {18, " 180%"}
};
std::mutex mu;

void InitializeDeskLoop(RGBMatrix &canvas, const Font &font);
void UpdateDeskLoop(RGBMatrix &canvas, const Font &font, const std::map<int,std::string> &specialChar);
void communicate();

int main(int argc, char *argv[]) {
  Font font;
  if (!font.LoadFont(fontName.c_str()))
    return 1;
  
  GPIO io;
  if (!io.Init())
    return 1;
  
  RGBMatrix *canvas = new RGBMatrix(&io, 32, 8); // rows 32 / chain 8
#if 0
  canvas->SetPWMBits(1); // if all_extreme_colors in text-example
#endif

#if 1
  int waitTest = 5;
  canvas->Clear();
  canvas->Fill(255, 0, 0);
  sleep(waitTest);
  canvas->Clear();
  canvas->Fill(0, 255, 0);
  sleep(waitTest);
  canvas->Clear();
  canvas->Fill(0, 0, 255);
  sleep(waitTest);
  canvas->Clear();
  canvas->Fill(255, 255, 255);
  sleep(waitTest);
  canvas->Clear();
#else
  srand((unsigned int)time(NULL));
  std::thread rc(communicate);
  while (true) {
    mu.lock();
    if (params.i==0||params.i>params.N) {
      InitializeDeskLoop(*canvas,font);
      params.i=0;
    }
    auto startTime = std::clock();
    if (!params.paused) {
      ++params.i;
      UpdateDeskLoop(*canvas,font,specialChar);
    }
    auto waitTime = params.waitMin + (rand()/(RAND_MAX/(params.waitMax-params.waitMin)));
    params.t += waitTime;
    mu.unlock();
    auto endTime = (std::clock() - startTime) / (float)CLOCKS_PER_SEC;
    if (endTime<waitTime) {
      sleep(waitTime-endTime);
    }
    if (params.t>=params.tMax) {
      break;
    }
  }
  rc.join();
#endif
  
  return 0;
}

void InitializeDeskLoop(RGBMatrix &canvas, const Font &font) {
  for (int i=0; i<4; ++i) {
    char line[2];
    sprintf(line, "%c", label[i]);
    DrawText(&canvas, font, 2, (i*32)+6 + font.baseline(),  params.color, line);
  }
}

void UpdateDeskLoop(RGBMatrix &canvas, const Font &font, const std::map<int,std::string> &specialChar) {
  unsigned int a = rand();
  int d = a % 4;
  char line[100];
  auto it = specialChar.find(params.i);
  if (it!=specialChar.end()) {
    sprintf(line, "%c%s", label[d],it->second.c_str());
  } else {
    sprintf(line, "%c %4d", label[d],params.i);
  }
#if 0
  // FIXME add partial clear method or redraw all frame
  canvas.Clear(12, (d*32), 52, 32);
#else
  canvas.Clear();
#endif
  DrawText(&canvas, font, 2, (d*32)+6 + font.baseline(),  params.color, line);
}

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
  server.sin_addr.s_addr = inet_addr("127.0.0.1");
  server.sin_family = AF_INET;
  server.sin_port = htons( 1111 );
  
  //checks connection
  if (bind(socket_info, (struct sockaddr *)&server, sizeof(server)) < 0) {
    perror("Connection error...\n");
  }
  
  //Receive an incoming message
  while (true) {
    // exit
    if (params.t>=params.tMax) {
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
            int s = std::min(100,std::max(0,std::stoi(ss)));
            mu.lock();
            params.waitMax = params.waitMin + (double)(100-s)/10.0;
            mu.unlock();
          }
          break;
          
          // Received restart
        case 'r':
          if (cmdLength==1) {
            mu.lock();
            params.i=0;
            mu.unlock();
          }
          break;
          
          // Received pause
        case 'p':
          if (cmdLength==1) {
            mu.lock();
            params.paused = params.paused ? false : true;
            mu.unlock();
          }
          break;
          
        default:
          break;
      }
    }
  }
}

