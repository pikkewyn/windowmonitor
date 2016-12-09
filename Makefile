APP						=	windowmonitor
SRC						=	$(wildcard *.c)
OBJDIR				=	$(PWD)/obj
OBJECTS 			= $(SRC:.c=.o)
CC						=	gcc 
CFLAGS				=	-std=gnu99 -g3 -Wall --pedantic -lXext -lXss
PKCONFIG_FLAGS=$(shell pkg-config --cflags  x11 xmu)
PKCONFIG_LIBS	=$(shell pkg-config --libs  x11 xmu)

all: $(APP)

$(APP): $(addprefix $(OBJDIR)/, $(OBJECTS))
	$(CC) $(CFLAGS) $(PKCONFIG_FLAGS) $(PKCONFIG_LIBS) $(addprefix $(OBJDIR)/, *.o) -o $(APP)

$(OBJDIR)/%.o : %.c | $(OBJDIR)
	$(CC) -c $(CFLAGS) $(PKCONFIG_FLAGS) $(PKCONFIG_LIBS) $< -o $@


$(OBJDIR):
	mkdir -p $@

clean:
	rm -rf $(APP) $(OBJDIR)
