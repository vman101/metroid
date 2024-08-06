CC := cc

CFLAGS := -Wall -Wextra -g3

LDFLAGS := -lSDL2 -lSDL2_image

SRC := main.c

OBJDIR := obj

OBJ := $(OBJDIR)/$(notdir $(SRC:.c=.o))

NAME := metroid

$(shell mkdir -p $(OBJDIR))

all: $(NAME)

$(NAME) : $(OBJ)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

fclean: clean
	rm -f $(NAME)

clean:
	rm -rf $(OBJDIR)

re: fclean all
