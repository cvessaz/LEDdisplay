CXXFLAGS=-Wall -O3 -g -fno-strict-aliasing
BINARIES=led-matrix minimal-example text-example bonzesPoste displayText rgbmatrix.so

# Where our library resides. It is split between includes and the binary
# library in lib
displayWrapper=../displayWrapper/displayWrapper
RGB_INCDIR=include
RGB_LIBDIR=lib
RGB_LIBRARY_NAME=rgbmatrix
RGB_LIBRARY=$(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).a
LDFLAGS+=-L$(RGB_LIBDIR) -l$(RGB_LIBRARY_NAME) -lrt -lm -lpthread

all : $(BINARIES)

$(RGB_LIBRARY):
	$(MAKE) -C $(RGB_LIBDIR)

led-matrix : demo-main.o $(RGB_LIBRARY)
	$(CXX) $(CXXFLAGS) demo-main.o -o $@ $(LDFLAGS)

minimal-example : minimal-example.o $(RGB_LIBRARY)
	$(CXX) $(CXXFLAGS) minimal-example.o -o $@ $(LDFLAGS)

text-example : text-example.o $(RGB_LIBRARY)
	$(CXX) $(CXXFLAGS) text-example.o -o $@ $(LDFLAGS)

bonzesPoste : bonzesPoste.o $(RGB_LIBRARY)
	$(CXX) $(CXXFLAGS) bonzesPoste.o -o $@ $(LDFLAGS)

remoteControl.o : $(displayWrapper)/remoteControl.cpp
  $(CXX) $(CXXFLAGS) -c -o $@ $<

data.o : $(displayWrapper)/data.cpp
  $(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -DADAFRUIT_RGBMATRIX_HAT -c -o $@ $<

localControl.o : $(displayWrapper)/localControl.cpp
  $(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -DADAFRUIT_RGBMATRIX_HAT -c -o $@ $<

displayText : displayText.o remoteControl.o data.o localControl.o $(RGB_LIBRARY)
	$(CXX) $(CXXFLAGS) displayText.o -o $@ $(LDFLAGS)

# Python module
rgbmatrix.so: rgbmatrix.o $(RGB_LIBRARY)
	$(CXX) -s -shared -lstdc++ -Wl,-soname,librgbmatrix.so -o $@ $< $(LDFLAGS)

%.o : %.cc
	$(CXX) -I$(RGB_INCDIR) $(CXXFLAGS) -DADAFRUIT_RGBMATRIX_HAT -c -o $@ $<

clean:
	rm -f *.o $(OBJECTS) $(BINARIES)
	$(MAKE) -C lib clean
