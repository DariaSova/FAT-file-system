CFLAGS = -Wall -g -std=c99  
LDFLAGS = -lreadline

OBJS = diskinfo.o disklist.o

all: diskinfo disklist

diskinfo: diskinfo.o
	$(CC) $(CFLAGS) diskinfo.o -o diskinfo $(LDFLAGS) 

disklist: disklist.o
	$(CC) $(CFLAGS) disklist.o -o disklist $(LDFLAGS) 
clean: 
	rm -rf $(OBJS) diskinfo disklist
