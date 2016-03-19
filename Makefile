CFLAGS = -Wall -g -std=c99  
LDFLAGS = -lreadline

OBJS = fat.o

all: diskinfo

diskinfo: $(OBJS)
	$(CC) $(CFLAGS) -o diskinfo $(OBJS) $(LDFLAGS) 

clean: 
	rm -rf $(OBJS) diskinfo
