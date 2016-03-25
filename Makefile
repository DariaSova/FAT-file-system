CFLAGS = -Wall -g -std=c99  
LDFLAGS = -lreadline

OBJS = diskinfo.o disklist.o diskget.o

all: diskinfo disklist diskget

diskinfo: diskinfo.o
	$(CC) $(CFLAGS) diskinfo.o -o diskinfo $(LDFLAGS) 

disklist: disklist.o
	$(CC) $(CFLAGS) disklist.o -o disklist $(LDFLAGS) 

diskget: diskget.o
	$(CC) $(CFLAGS) diskget.o -o diskget $(LDFLAGS) 
clean: 
	rm -rf $(OBJS) diskinfo disklist diskget
