# Makefile (shell)

include ../Makefile.common

OBJ = $(OBJ_DIR)/shell/main.o $(OBJ_DIR)/shell/wrappers.o

$(OBJ_DIR)/shell/main.o: ../shell/main.c ../shell/wrappers.h
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(OBJ_DIR)/shell/wrappers.o: ../shell/wrappers.c ../shell/wrappers.h ../shell/shell.h
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
