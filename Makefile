# Makefile (Top level)

include Makefile.common

.PHONY: all clean debug release

all: release

debug: CFLAGS += $(DEBUG_FLAGS)
debug: LDFLAGS += $(DEBUG_FLAGS)  # Add sanitizer flags to LDFLAGS
debug: drn_shell $(BIN_DIR)/cat $(BIN_DIR)/ls

release: CFLAGS += $(RELEASE_FLAGS)
release: drn_shell $(BIN_DIR)/cat $(BIN_DIR)/ls

# Shell target
drn_shell: $(OBJ_DIR)/shell/main.o $(OBJ_DIR)/shell/wrappers.o
	$(MKDIR) $(BIN_DIR)
	$(CC) -o $@ $^ $(LDFLAGS)

# Cat target
$(BIN_DIR)/cat: $(OBJ_DIR)/utils/cat/cat.o
	$(MKDIR) $(BIN_DIR)
	$(CC) -o $@ $^ $(LDFLAGS)

# LS target
$(BIN_DIR)/ls: $(OBJ_DIR)/utils/ls/ls.o
	$(MKDIR) $(BIN_DIR)
	$(CC) -o $@ $^ $(LDFLAGS)

# Generic object file rule
$(OBJ_DIR)/shell/%.o: shell/%.c
	$(MKDIR) $(OBJ_DIR)/shell
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(OBJ_DIR)/utils/cat/%.o: utils/cat/%.c
	$(MKDIR) $(OBJ_DIR)/utils/cat
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

$(OBJ_DIR)/utils/ls/%.o: utils/ls/%.c
	$(MKDIR) $(OBJ_DIR)/utils/ls
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

clean:
	$(CLEAN)
