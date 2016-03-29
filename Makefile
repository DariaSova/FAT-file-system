CFLAGS = -Wall -g -std=c99  
LDFLAGS = -lreadline

OBJS = diskinfo.o disklist.o diskget.o diskput.o

all: diskinfo disklist diskget diskput

diskinfo: diskinfo.o
	$(CC) $(CFLAGS) diskinfo.o -o diskinfo $(LDFLAGS) 

disklist: disklist.o
	$(CC) $(CFLAGS) disklist.o -o disklist $(LDFLAGS) 

diskget: diskget.o
	$(CC) $(CFLAGS) diskget.o -o diskget $(LDFLAGS) 

diskput: diskput.o
	$(CC) $(CFLAGS) diskput.o -o diskput $(LDFLAGS) 
clean: 
	rm -rf $(OBJS) diskinfo disklist diskget diskput
