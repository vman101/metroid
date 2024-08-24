CC := cc

CFLAGS := -Wall -Wextra -g3 -I./include

LDFLAGS := -lSDL2 -L./lib -lSDL2_image

LIB := lib/*

SRC := main.c physics.c signals.c init.c client.c

OBJDIR := obj

OBJ := $(SRC:%.c=$(OBJDIR)/%.o)

NAME := metroid

$(shell mkdir -p $(OBJDIR))

SERV := server

all: $(NAME)

$(NAME) : $(OBJ) $(SERV)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

$(SERV): server.c
	cc server.c -lSDL2 -lSDL2_net -o server

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

fclean: clean
	rm -f $(NAME)

clean:
	rm -rf $(OBJDIR)

re: fclean all
