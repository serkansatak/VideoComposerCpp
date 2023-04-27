CC=g++
CFLAGS=-std=c++17 -Wall -Wextra -O3 -DTHREADED_PROCESS -DTEST_VIDEO
LDFLAGS=-L/opt/homebrew/opt/opencv/lib/ -L/opt/homebrew/opt/boost/lib/ -L/opt/homebrew/opt/ \
-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_videoio -lboost_filesystem -lpthread -lopencv_imgcodecs

EXECUTABLE=VideoComposer

SRCDIR=src
INCDIR=include -Iargparse/include/argparse/ -I/opt/homebrew/include/opencv4/ -I/opt/homebrew/include/
OBJDIR=obj
BINDIR=bin

SOURCES=$(wildcard $(SRCDIR)/*.cpp)
OBJECTS=$(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

.PHONY: all clean

all: $(BINDIR)/$(EXECUTABLE)

$(BINDIR)/$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -I$(INCDIR) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(BINDIR)/$(EXECUTABLE)
