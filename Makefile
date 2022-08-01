HOSTTYPE ?= $(shell uname -m)_$(shell uname -s)

#-Make-#
MALLOC_NAME = libft_malloc_$(HOSTTYPE).so
CFLAGS ?= -Wall -Wextra -Werror -g -fsanitize=address
LDFLAGS ?= -shared

all: $(MALLOC_NAME)

$(MALLOC_NAME): src/malloc.c includes/malloc.h
	$(CC) -c $< -o $@ $(CFLAGS) $(LDFLAGS) -Iincludes

clean:
	@sleep 1

fclean:
	@rm -f $(MALLOC_NAME)

.PHONY: all fclean clean re

re: fclean all
