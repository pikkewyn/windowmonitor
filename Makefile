APP						=	windowmonitor
SRC						=	$(wildcard src/*.c)
OBJECTS				= $(patsubst src/%.c,%.o,$(SRC))
SRCDIR				=	$(PWD)/src
OBJDIR				=	$(PWD)/obj

CC						=	gcc 
PKCONFIG_FLAGS=$(shell pkg-config --cflags  x11 xmu)
PKCONFIG_LIBS	=$(shell pkg-config --libs  x11 xmu)
CFLAGS				=	-std=gnu99 -g3 -Wall --pedantic $(PKCONFIG_FLAGS) 
LDFLAGS				= -lXext -lXss $(PKCONFIG_LIBS)

all: $(OBJDIR)/$(APP)

$(OBJDIR)/$(APP): $(addprefix $(OBJDIR)/, $(OBJECTS))
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.o : $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(PKCONFIG_LIBS) -o $@ -c $^


$(OBJDIR):
	mkdir -p $@

clean:
	rm -rf $(APP) $(OBJDIR)
