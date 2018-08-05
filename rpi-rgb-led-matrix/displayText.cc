#include "led-matrix.h"
#include "graphics.h"

#include <unistd.h>
#include <ctime>
#include <vector>
#include <sstream>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>

using namespace rgb_matrix;

struct Params {
  std::string text = "Hello world!";
  Color color=Color(255, 0, 0);
  double speed = 0;
  bool isCleared = false;
  bool isStopped = false;
};
Params params;
std::mutex mu;

class Data {
public:
  std::vector<std::pair<int,int>> pixels;
  RGBMatrix *canvas;
  Color *color;
  Font font;
  int fontWidth;
  int fontHeight;
  std::string *text;
  int xMax;
  
  Data(RGBMatrix* _canvas, const int &_fontWidth, const int &_fontHeight);
  
  ~Data();
  
  void initialize();
  
  void update(const int &dx);
};

int main(int argc, char *argv[]) {
  
  GPIO io;
  if (!io.Init()) {	  
	  std::cout << "GPIO ERROR" << std::endl;
	  return 1;
  }
  
  RGBMatrix *canvas = new RGBMatrix(&io, 32, 8); // rows 32 / chain 8
  canvas->SetPWMBits(1); // if all_extreme_colors in text-example

#if 1
  Data text(&canvas, fontWidth, fontHeight);
  std::thread rc(communicate);
  
  // Main loop
  double elapseTime = std::clock();
  while (true) {
    // Cleared
    if (params.isCleared) {
      elapseTime = std::clock();
      mu.lock();
      text.initialize();
      params.isCleared = false;
      mu.unlock();
    }
    
    // Update
    if (params.speed==0) elapseTime = std::clock();
    double pixelDiff = (std::clock()-elapseTime)/CLOCKS_PER_SEC*params.speed;
    if (pixelDiff>=1.0) {
      elapseTime = std::clock();
      mu.lock();
      text.update((int)pixelDiff);
      mu.unlock();
    }
    
    // Exit
    if (params.isStopped) break;
  }
  
  // End communication
  rc.join();
#endif
  
  canvas->Clear();
  delete canvas;

  return 0;
}

Data::Data(RGBMatrix* _canvas, const int &_fontWidth, const int &_fontHeight) {
  canvas = _canvas;
  std::stringstream fontName;
  fontName << _fontWidth << "x" << _fontHeight << ".bdf";
  bool found = font.LoadFont(fontName.str().c_str());
  assert(found);
  color = &params.color;
  text = &params.text;
  fontWidth = _fontWidth;
  fontHeight = _fontHeight;
  xMax = std::max((int)text->size()*fontWidth,canvas->width());
  initialize();
}

Data::~Data() {
}

void Data::initialize() {
  if (pixels.size()>0) {
    for (auto &pixel : pixels) {
      canvas->SetPixel(pixel.first, pixel.second, 0, 0, 0);
    }
  }
  pixels.clear();
  pixels.reserve(xMax*fontHeight);
  int x = 0;
  int y = 38;
  for (const uint32_t cp : *text) {
    x += font.DrawGlyph(pixels, x, y, cp);
  }
  update(0);
}

void Data::update(const int &dx) {
  if (dx>0) {
    for (auto &pixel : pixels) {
      canvas->SetPixel(pixel.first, pixel.second, 0, 0, 0);
    }
    for (auto &pixel : pixels) {
      pixel.first = (pixel.first-dx);
      if (pixel.first<0) pixel.first += xMax;
      canvas->SetPixel(pixel.first, pixel.second, color->r, color->g, color->b);
    }
  } else {
    for (const auto &pixel : pixels) {
      canvas->SetPixel(pixel.first, pixel.second, color->r, color->g, color->b);
    }
  }
#ifdef __APPLE__
  canvas->Refresh();
#endif
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
