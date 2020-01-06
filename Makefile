
-include not_exst.mk
VPATH=./src:./inc:../

CC=gcc
SRCDIR=src
OBJDIR=obj
INCDIR=inc
LIBDIR=lib
EXEDIR=run
EXE   =pc
VERSION=v1
DATA_STRUCTURE = LINUX_LIST
#DATA_STRUCTURE=CARRAY
ifeq ($(VERSION),v0)
	SOURCE=oneprodcons.c 
	   #queue.c
else #ifeq ($(VERSION),v1)
	SOURCE= extBB.c \
			misc.c \
		    msg.c 
		    #misc.c

	#echo "aaa"
endif
#CFLAGS=-MM

PTHREAD=-lpthread

OBJECTS=$(patsubst %.c,$(OBJDIR)/%.o,$(SOURCE))

all:exe recv_process


exe:$(OBJECTS)
ifeq ($(CC),gcc)
	@mkdir -p $(EXEDIR)
	$(CC) -g -D $(DATA_STRUCTURE)=1 $(PTHREAD) $(OBJDIR)/*.o -o $(EXE)
else
	$(CC) -g $(OBJDIR)/*.o -o $(EXE)
endif


depend: .depend
.depend: src/extBB.c
	rm -f ./.depend
	$(CC) $(CFLAGS) -I$(INCDIR) -MM $^ -MF  ./.depend;
include .depend




$(OBJDIR)/%.o:$(SRCDIR)/%.c $(INCDIR)/%.h
	@mkdir -p $(OBJDIR)
	$(CC) -c -g $(CFLAGS) $(OPTS) $(INCLUDES) -I$(INCDIR) -o "$@" "$<"

recv_process:$(SRCDIR)/recv.c
	gcc -g -o "$@" "$<"

.PHONY: cs
cs:
	gcc $(SRCDIR)/server.c -o server
	gcc $(SRCDIR)/client.c -o client

.PHONY: clean
clean:
	-rm -f $(OBJDIR)/*.o $(EXEDIR)/$(EXE)
	-rm -rf *.mp4 *.h264
