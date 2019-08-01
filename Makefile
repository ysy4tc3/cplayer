LIBS = -lavdevice -lavformat -lavfilter -lavcodec -lswscale -lavutil -ldl -lswresample -lm -lz -lpthread
LDFLAGS = -L/usr/local/opt/ffmpeg/bin
CPPFLAGS = -I/usr/local/opt/ffmpeg/include

myplayer: cplayer.o
	gcc -o myplayer cplayer.o -g  $(CPPFLAGS) $(LDFLAGS) $(LIBS)
myplayer.o: cplayer.c cfconst.h cframe.h
	gcc -c cplayer.c
clean:
	rm -rf myplayer *.o

