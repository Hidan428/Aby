IFLAGS = -I../include/
CPPFLAGS = -std=c++11 -O3
CV_LIBS = -L../lib/ -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_core -lopencv_hal -lopencv_videoio
SHARED_LIBS = -L../lib/ -llibjpeg -llibwebp -llibpng -llibtiff -llibjasper -lIlmImf -lzlib -lippicv
OSX_LIBS = -framework OpenCL -framework Cocoa -framework QTKit -framework QuartzCore -framework AppKit 

CXX=clang++

all : TP1

afficheImage : afficheImage.cpp
	$(CXX) -c $(IFLAGS) $(CPPFLAGS) afficheImage.cpp

TP1 : afficheImage 
	$(CXX) -o tp1 afficheImage.o $(CPPFLAGS) $(CV_LIBS) $(SHARED_LIBS) $(OSX_LIBS)

clean :
	rm *.o TP1
