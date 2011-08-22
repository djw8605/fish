#  These should really be defined in autoconf
FREETYPE_CFLAGS = $(shell freetype-config --cflags)
FREETYPE_LIBS = $(shell freetype-config --libs)



CPPFLAGS = $(FREETYPE_CFLAGS) -g -I/usr/local/include
CXXFLAGS = 
LIBS = -lglut -lm -lGL -lGLU -lpthread $(FREETYPE_LIBS) -lxerces-c 

CXX = g++

CASTLEMODEL_SRC = castlemodel.cpp partialcylinder.cpp  solidRectangle.cpp
COMM_SRC = comClass.cpp
MAIN_SRC = aquarium.cpp configParser.cpp fishmodel.cpp FreeType.cpp \
			picking.cpp collision.cpp CTargaImage.cpp FishSplash.cpp \
			imageLoader.cpp collisionMap.cpp ErrorDisplay.cpp fishStats.cpp \
			lighthousemodel.cpp shaders.cpp LabelDisplay.cpp

DATA_FILES = bubblefragment.shader bubblevertex.shader config config.xml fishfragment.shader fishvertex.shader lighthouse.tga drawbridge.tga Test.ttf wall.tga wallthin.tga

SOURCES = $(MAIN_SRC) $(COMM_SRC:%=com/%) $(CASTLEMODEL_SRC:%=castle/%)
OBJS =  $(MAIN_SRC:%.cpp=%.o) $(COMM_SRC:%.cpp=com/%.o) $(CASTLEMODEL_SRC:%.cpp=castle/%.o)

all: aquarium

aquarium: $(OBJS)
	$(CXX) -o aquarium $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS)


install: all
	install -m 755 -d $(DESTDIR)/$(BINDIR)
	install -m 755 aquarium.start $(DESTDIR)/$(BINDIR)/aquarium
	install -m 755 -d $(DESTDIR)/etc/fish-monitoring/
	install -m 744 config.xml $(DESTDIR)/etc/fish-monitoring/
	install -m 755 aquarium $(DESTDIR)/etc/fish-monitoring/
	install -m 744 Test.ttf $(DESTDIR)/etc/fish-monitoring/
	install -m 744 bubblefragment.shader $(DESTDIR)/etc/fish-monitoring/
	install -m 744 bubblevertex.shader $(DESTDIR)/etc/fish-monitoring/
	install -m 744 fishfragment.shader $(DESTDIR)/etc/fish-monitoring/
	install -m 744 fishvertex.shader $(DESTDIR)/etc/fish-monitoring/
	install -m 744 lighthouse.tga $(DESTDIR)/etc/fish-monitoring/
	install -m 744 wall.tga $(DESTDIR)/etc/fish-monitoring/
	install -m 744 wallthin.tga $(DESTDIR)/etc/fish-monitoring/
	install -m 744 drawbridge.tga $(DESTDIR)/etc/fish-monitoring/



