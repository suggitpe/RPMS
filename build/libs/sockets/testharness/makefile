MFR_DIR=../../..
include $(MFR_DIR)/_makefile.rules

.PHONY: idls

CC=$(MASTER_CC)
CC_FLAGS=$(MASTER_FLAGS)

INCLUDE_PATH=$(MASTER_INCL_PATH)

LIB_PATH=$(MASTER_LIB_PATH)

LIB_FILES=$(MASTER_LIB_FILES)

PKG_DIR=$(MASTER_BASE)/$(LIB_SRC)/sockets/testharness

OBJ_FILES0=test_client.o
OBJ_FILES1=test_server.o
OBJ_FILES2=play_server.o
OBJ_FILES3=play_client.o

TARGET0=$(MASTER_BASE)/$(APP_DST)/test/libsockets_test_client
TARGET1=$(MASTER_BASE)/$(APP_DST)/test/libsockets_test_server
TARGET2=$(MASTER_BASE)/$(APP_DST)/play/libsockets_play_server
TARGET3=$(MASTER_BASE)/$(APP_DST)/play/libsockets_play_client

# make sure they both get built
all: $(TARGET0) $(TARGET1) $(TARGET2) $(TARGET3)

$(TARGET0): $(OBJ_FILES0)
	-@echo "        - linking ... $(TARGET0)"
	$(CC) $(CC_FLAGS) -o $(TARGET0) $(OBJ_FILES0) $(LIB_PATH) $(LIB_FILES)

$(TARGET1): $(OBJ_FILES1)
	-@echo "        - linking ... $(TARGET1)"
	$(CC) $(CC_FLAGS) -o $(TARGET1) $(OBJ_FILES1) $(LIB_PATH) $(LIB_FILES)

$(TARGET2): $(OBJ_FILES2)
	-@echo "        - linking ... $(TARGET2)"
	$(CC) $(CC_FLAGS) -o $(TARGET2) $(OBJ_FILES2) $(LIB_PATH) $(LIB_FILES)

$(TARGET3): $(OBJ_FILES3)
	-@echo "        - linking ... $(TARGET3)"
	$(CC) $(CC_FLAGS) -o $(TARGET3) $(OBJ_FILES3) $(LIB_PATH) $(LIB_FILES)

%.o: $(PKG_DIR)/%.cpp
	-@echo "        - building $@"
	$(CC) -c $(CC_FLAGS) $(INCLUDE_PATH) -o $@ $<

idls:

cleanall:
	-@rm $(OBJ_FILES0)
	-@rm $(OBJ_FILES1)
	-@rm $(OBJ_FILES2)
	-@rm $(OBJ_FILES3)
	-@rm $(TARGET0)
	-@rm $(TARGET1)
	-@rm $(TARGET2)
	-@rm $(TARGET3)

