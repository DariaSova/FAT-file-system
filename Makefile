CFLAGS = -Wall -g -std=c99  
LDFLAGS = -lreadline

OBJS = diskinfo.o disklist.o

all: diskinfo disklist

diskinfo: $(OBJS)
	$(CC) $(CFLAGS) -o diskinfo $(OBJS) $(LDFLAGS) 

disklist: $(OBJS)
	$(CC) $(CFLAGS) -o disklist $(OBJS) $(LDFLAGS) 
clean: 
	rm -rf $(OBJS) diskinfo disklist
