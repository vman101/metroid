CC := cc

CFLAGS := -Wall -Wextra -g3 -I/usr/local/include/SDL2

LDFLAGS := -lSDL2 -lSDL2_image -lSDL2_net -I/usr/local/include/SDL2

SRC := main.c physics.c signals.c init.c

OBJDIR := obj

OBJ := $(SRC:%.c=$(OBJDIR)/%.o)

NAME := metroid

$(shell mkdir -p $(OBJDIR))

all: $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

fclean: clean
	rm -f $(NAME)

clean:
	rm -rf $(OBJDIR)

re: fclean all
