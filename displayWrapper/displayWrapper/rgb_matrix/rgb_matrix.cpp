//
//  rgb_matrix.cpp
//  rgb_matrix
//
//  Created by Christian Vessaz on 10.01.18.
//  Copyright © 2018 Christian Vessaz. All rights reserved.
//

#include "rgb_matrix.h"

RGBMatrix::RGBMatrix() {
  Initialize();
}

RGBMatrix::~RGBMatrix(){}

int RGBMatrix::width() {
  return ScreenWidth;
}

int RGBMatrix::height() {
  return ScreenHeight;
}

void RGBMatrix::Clear(const int &x0, const int &y0, const int &nx, const int &ny) {
  for (int y=y0; y<y0+ny; ++y) {
    for (int x=x0; x<x0+nx; ++x) {
      int p = (y*ScreenWidth)+x;
      for (int vert=0; vert<2*3; ++vert) {
        for (int col=0; col<3; ++col) {
          g_color_buffer_data[(p*2*3*3)+(vert*3)+col] = 0.0f;
        }
      }
    }
  }
  Refresh();
}

void RGBMatrix::Fill(const uint8_t &red, const uint8_t &green, const uint8_t &blue) {
  for (int y=0; y<ScreenHeight; ++y) {
    for (int x=0; x<ScreenWidth; ++x) {
      int p = (y*ScreenWidth)+x;
      for (int vert=0; vert<2*3; ++vert) {
        g_color_buffer_data[(p*2*3*3)+(vert*3)+0] = (float)red/255.0f;
        g_color_buffer_data[(p*2*3*3)+(vert*3)+1] = (float)green/255.0f;
        g_color_buffer_data[(p*2*3*3)+(vert*3)+2] = (float)blue/255.0f;
      }
    }
  }
  Refresh();
}

void RGBMatrix::SetPixel(const int &x, const int &y, const uint8_t &red, const uint8_t &green, const uint8_t &blue) {
  if (x>=0 && x<ScreenWidth && y>=0 && y<ScreenHeight) {
    int p = (y*ScreenWidth)+x;
    for (int vert=0; vert<2*3; ++vert) {
      g_color_buffer_data[(p*2*3*3)+(vert*3)+0] = (float)red/255.0f;
      g_color_buffer_data[(p*2*3*3)+(vert*3)+1] = (float)green/255.0f;
      g_color_buffer_data[(p*2*3*3)+(vert*3)+2] = (float)blue/255.0f;
    }
  }
}

int RGBMatrix::Initialize() {
  // Initialize GLFW
  if (!glfwInit()) {
    fprintf( stderr, "Failed to initialize GLFW\n" );
    return -1;
  }
  glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Only modern OpenGL
  
  // Open a window and create OpenGL context
  window = glfwCreateWindow(ScreenWidth*PixelSize, ScreenHeight*PixelSize, "LED", NULL, NULL);
  if (window == NULL) {
    fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  
  // Capture la escape key
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  
  // Initialize GLEW
  glewExperimental=true; // Mendatory
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return -1;
  }
  
  // Vertex Array Object (VAO)
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);
  
  // Create and compile our GLSL program from the shaders
  programID = LoadShaders("vertexShader.vsgl", "fragmentShader.fsgl" );
  
  // An array of pixel vertices (2triangles per pixel)
  GLfloat g_vertex_buffer_data[ScreenWidth*ScreenHeight*2*3*3];
  float dx = 2.0f/(float)ScreenWidth;
  float dy = 2.0f/(float)ScreenHeight;
  for (int y=0; y<ScreenHeight; ++y) {
    for (int x=0; x<ScreenWidth; ++x) {
      int p = (y*ScreenWidth)+x;
      float cx = -1.0f + 2.0f*(float)x/(float)ScreenWidth;
      float cy = 1.0f - 2.0f*(float)y/(float)ScreenHeight;
      // T1 V1
      g_vertex_buffer_data[p*(2*3*3)+0] = cx;
      g_vertex_buffer_data[p*(2*3*3)+1] = cy;
      g_vertex_buffer_data[p*(2*3*3)+2] = 0.0f;
      // T1 V2
      g_vertex_buffer_data[p*(2*3*3)+3] = cx+dx;
      g_vertex_buffer_data[p*(2*3*3)+4] = cy;
      g_vertex_buffer_data[p*(2*3*3)+5] = 0.0f;
      // T1 V3
      g_vertex_buffer_data[p*(2*3*3)+6] = cx;
      g_vertex_buffer_data[p*(2*3*3)+7] = cy-dy;
      g_vertex_buffer_data[p*(2*3*3)+8] = 0.0f;
      // T2 V1
      g_vertex_buffer_data[p*(2*3*3)+9] = cx;
      g_vertex_buffer_data[p*(2*3*3)+10] = cy-dy;
      g_vertex_buffer_data[p*(2*3*3)+11] = 0.0f;
      // T2 V2
      g_vertex_buffer_data[p*(2*3*3)+12] = cx+dx;
      g_vertex_buffer_data[p*(2*3*3)+13] = cy-dy;
      g_vertex_buffer_data[p*(2*3*3)+14] = 0.0f;
      // T2 V3
      g_vertex_buffer_data[p*(2*3*3)+15] = cx+dx;
      g_vertex_buffer_data[p*(2*3*3)+16] = cy;
      g_vertex_buffer_data[p*(2*3*3)+17] = 0.0f;
    }
  }
  /*for (int i=0; i<ScreenWidth*ScreenHeight*2*3*3; ++i) {
    if (i%3==0) printf("\n");
    if (i%9==0) printf("\n");
    printf("%f ",g_vertex_buffer_data[i]);
  }*/
  
  // Generate 1 buffer, put the resulting identifier in vertexbuffer
  glGenBuffers(1, &vertexbuffer);
  // The following commands will talk about our 'vertexbuffer' buffer
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  // Give our vertices to OpenGL.
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
  
  // One color for each vertex. They were generated randomly.
  for (int y=0; y<ScreenHeight; ++y) {
    for (int x=0; x<ScreenWidth; ++x) {
      int p = (y*ScreenWidth)+x;
      for (int vert=0; vert<2*3; ++vert) {
        for (int col=0; col<3; ++col) {
          g_color_buffer_data[(p*2*3*3)+(vert*3)+col] = 0.0f;
        }
      }
    }
  }
  /*for (int i=0; i<ScreenWidth*ScreenHeight*2*3*3; ++i) {
    if (i%3==0) printf("\n");
    if (i%9==0) printf("\n");
    printf("%f ",g_color_buffer_data[i]);
  }*/
  
  glGenBuffers(1, &colorbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
  
  UpdateScreen();
  
  return 0;
}

int RGBMatrix::UpdateScreen() {
  // Clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  // Use our shader
  glUseProgram(programID);
  
  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glVertexAttribPointer(
                        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                        3,                  // size
                        GL_FLOAT,           // type
                        GL_FALSE,           // normalized?
                        0,                  // stride
                        (void*)0            // array buffer offset
                        );
  
  // 2nd attribute buffer : colors
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glVertexAttribPointer(
                        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                        3,                                // size
                        GL_FLOAT,                         // type
                        GL_FALSE,                         // normalized?
                        0,                                // stride
                        (void*)0                          // array buffer offset
                        );
  
  // Draw the triangle !
  glDrawArrays(GL_TRIANGLES, 0, ScreenWidth*ScreenHeight*2*3); // Starting from vertex 0; 3 vertices total -> 1 triangle
  glDisableVertexAttribArray(0);
  
  // Swap buffers
  glfwSwapBuffers(window);
  glfwPollEvents();
  
  return 0;
}

void RGBMatrix::Refresh() {
  // Check close signal from glfw
  if( glfwGetKey(window, GLFW_KEY_ESCAPE ) == GLFW_PRESS || glfwWindowShouldClose(window) == 1 ) exit(1);
  // Copy data to GPU
  glGenBuffers(1, &colorbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
  // Draw
  UpdateScreen();
}

void RGBMatrix::Save(const int &frame) {
  char numstr[6];
  sprintf(numstr, "_%.4d", frame);
  std::string pngname = "../../../displayWrapper/rgb_matrix/images/image" + std::string(numstr) + ".png";
  std::string ppmname = "../../../displayWrapper/rgb_matrix/images/image.ppm";
  
  if (frame==0) {
    std::string cmd_clear = "rm -r ../../../displayWrapper/rgb_matrix/images";
    system(cmd_clear.c_str());
    std::string cmd_create = "mkdir ../../../displayWrapper/rgb_matrix/images";
    system(cmd_create.c_str());
  }
  
  unsigned int numChannels = 3;
  Image image(ScreenWidth, ScreenHeight, numChannels);
  // TODO: copy data all at once
  for (auto j=0; j < ScreenHeight; j++) {
    for (auto i=0; i < ScreenWidth; i++) {
      auto ind = j*ScreenWidth + i;
      for (auto channel = 0; channel < numChannels; channel++)
        image.set_pixel(i, j, channel, g_color_buffer_data[(ind*18)+channel]);
    }
  }
  auto success = image.save(ppmname.c_str());
  if (!success) printf("REMARK: Failed saving image: %s\n", ppmname.c_str());
  
  std::string cmd_convert = std::string("/opt/local/bin/convert '") + ppmname + "' '" + pngname + "'";
  system(cmd_convert.c_str());
  std::string cmd_clean = "rm " + ppmname;
  system(cmd_clean.c_str());
}

void RGBMatrix::Movie() {
  std::string directory("../../../displayWrapper/rgb_matrix/images/image_%04d.png");
  std::string resolution = std::to_string(ScreenWidth*PixelSize) + ":" + std::to_string(ScreenHeight*PixelSize);
  std::string vcodec("h264");
  std::string pix_fmt("yuv420p");
  std::string vquality("100000k");
  std::string movie("../../../displayWrapper/rgb_matrix/images/displayTest.mp4");
  std::string fps("25");
  
  std::string cmd = std::string("/opt/local/bin/ffmpeg")
                  + " -framerate " + fps
                  + " -i " + directory
                  + " -s " + resolution
                  + " -vcodec " + vcodec
                  + " -pix_fmt " + pix_fmt
                  + " -b:v " + vquality
                  + " -r " + fps
                  + " -y " + movie;
  system(cmd.c_str());
}
