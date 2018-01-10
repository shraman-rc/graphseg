# Ubuntu Linux build
#
TARGETS = main

CXX_VER = `$(CXX) -v 2>&1 | tail -n 1`
CIMG_VER = `grep 'cimg_version\ ' ./CImg.h | tail -c4 | head -c3`

CFLAGS = -std=c++11 -I. -Wall -Wfatal-errors
LIBS = -lm

# optimize
CFLAGS += -O3 -mtune=generic

# CImg flags
CFLAGS += -Dcimg_use_vt100 -Dcimg_display=1

# X11 flags (for CImg)
CFLAGS += `pkg-config --cflags x11 || echo -I/usr/X11R6/include`
LIBS += `pkg-config --libs x11 || echo -L/usr/X11R6/lib -lX11` -lpthread

.cpp:
	@echo
	@echo "== Compiling $* with CIimg v$(CIMG_VER), '$(CXX_VER)'"
	@echo
	$(CXX) -o $* $< $(CFLAGS) $(LIBS)

default:
	@$(MAKE) $(TARGETS)

clean:
	rm -rf *.o *~ $(TARGETS)
