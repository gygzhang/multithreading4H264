
ROOT := ..
INCDIR := -I$(ROOT)/include -I$(ROOT)/lib/x86/expat -I$(ROOT)/lib/x86/hc4000_sdk
          
#             -I$(ROOT)/lib/expat/include -I$(ROOT)/lib/cunit/include \
#             -I$(ROOT)/lib/hisilicon/msp-include -I$(ROOT)/lib/hisilicon/extdrv-include \

LIBDIR := -L$(ROOT)/lib/x86/expat -L$(ROOT)/lib/x86/hc4000_sdk
TARGET := $(ROOT)/bin/vs

#CC :=arm-uclibc-linux-gcc
#LD :=arm-uclibc-linux-ld

CC :=gcc
LD :=ld

CFLAGS :=   -c  -Wall -g -O  $(INCDIR)
#LDFLAGS :=  -static -lpthread -lhiapi -lexpatw -lds40xxsdk -lrt $(LIBDIR)
LDFLAGS :=  -static -lpthread  -lexpatw -lds40xxsdk -lrt $(LIBDIR)

TESTLDFLAGS := $(LDFLAGS) -lcunit
SOURCES := $(wildcard *.c)
OBJS   := $(patsubst %.c,%.o,$(SOURCES))

$(TARGET):$(OBJS)
	echo $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@ 

%.o:    %.c
	$(CC) $< $(CFLAGS) -o $@ 

.PHONY:clean
clean:
	rm -f $(TARGET) *.o *~
