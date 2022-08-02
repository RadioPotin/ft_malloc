HOSTTYPE ?= $(shell uname -m)_$(shell uname -s)

#-Make-#
MALLOC_NAME = libft_malloc_$(HOSTTYPE).so
CFLAGS ?= -Wall -Wextra -Werror -g
LDFLAGS ?= -shared
LIBFTNAME := libft/libft.a

test: src/main.c all
	$(CC) $< -o main.exe -Iincludes -lft_malloc_x86_64_Linux -L.
	./main.exe
	rm main.exe

all: $(MALLOC_NAME)


$(LIBFTNAME):
	make -C libft

$(MALLOC_NAME): src/ft_malloc.c includes/ft_malloc.h $(LIBFTNAME)
	$(CC) -c $< -o $@ $(CFLAGS) $(LDFLAGS) -Iincludes

clean:
	make clean -C libft

fclean:
	@rm -f $(MALLOC_NAME)
	make fclean -C libft

.PHONY: all fclean clean re

re: fclean all
