APP						=	windowmonitor
SRC						=	$(wildcard *.c)
OBJDIR				=	$(PWD)/obj
OBJECTS 			= $(SRC:.c=.o)
CC						=	gcc 
PKCONFIG_FLAGS=$(shell pkg-config --cflags  x11 xmu)
PKCONFIG_LIBS	=$(shell pkg-config --libs  x11 xmu)
CFLAGS				=	-std=gnu99 -g3 -Wall --pedantic $(PKCONFIG_FLAGS) 
LDFLAGS				= -lXext -lXss $(PKCONFIG_LIBS)

all: $(APP)

$(APP): $(addprefix $(OBJDIR)/, $(OBJECTS))
	$(CC) $(CFLAGS) $(addprefix $(OBJDIR)/, *.o) $(LDFLAGS) -o $(APP)

$(OBJDIR)/%.o : %.c | $(OBJDIR)
	$(CC) -c $(CFLAGS) $(PKCONFIG_LIBS) $(LDFLAGS) $< -o $@


$(OBJDIR):
	mkdir -p $@

clean:
	rm -rf $(APP) $(OBJDIR)
