MFR_DIR=../../..
include $(MFR_DIR)/_makefile.rules

.PHONY: idls

CC=$(MASTER_CC)
CC_FLAGS=$(MASTER_FLAGS)

INCLUDE_PATH=$(MASTER_INCL_PATH)

LIB_PATH=$(MASTER_LIB_PATH)

LIB_FILES=$(MASTER_LIB_FILES) $(DAEMON_LIBS)

PKG_DIR=$(MASTER_BASE)/$(LIB_SRC)/daemonlib/testharness

OBJ_FILES0=test.o
OBJ_FILES1=play.o
OBJ_FILES2=processmgr.o

TARGET0=$(MASTER_BASE)/$(APP_DST)/test/libdaemon_test
TARGET1=$(MASTER_BASE)/$(APP_DST)/play/libdaemon_play
TARGET2=$(MASTER_BASE)/$(APP_DST)/play/libdaemon_process

# make sure they both get built
all: $(TARGET0) $(TARGET1) $(TARGET2)

$(TARGET0): $(OBJ_FILES0)
	-@echo "        - linking ... $(TARGET0)"
	$(CC) $(CC_FLAGS) -o $(TARGET0) $(OBJ_FILES0) $(LIB_PATH) $(LIB_FILES)

$(TARGET1): $(OBJ_FILES1)
	-@echo "        - linking ... $(TARGET1)"
	$(CC) $(CC_FLAGS) -o $(TARGET1) $(OBJ_FILES1) $(LIB_PATH) $(LIB_FILES)

$(TARGET2): $(OBJ_FILES2)
	-@echo "        - linking ... $(TARGET2)"
	$(CC) $(CC_FLAGS) -o $(TARGET2) $(OBJ_FILES2) $(LIB_PATH) $(LIB_FILES)

%.o: $(PKG_DIR)/%.cpp
	-@echo "        - building $@"
	$(CC) -c $(CC_FLAGS) $(INCLUDE_PATH) -o $@ $<

idls:

cleanall:
	-@rm $(OBJ_FILES0)
	-@rm $(OBJ_FILES1)
	-@rm $(OBJ_FILES2)
	-@rm $(TARGET0)
	-@rm $(TARGET1)
	-@rm $(TARGET2)

