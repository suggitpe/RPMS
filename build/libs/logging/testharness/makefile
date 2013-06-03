MFR_DIR=../../..
include $(MFR_DIR)/_makefile.rules

.PHONY: idls

CC=$(MASTER_CC)
CC_FLAGS=$(MASTER_FLAGS)

INCLUDE_PATH=$(MASTER_INCL_PATH)

LIB_PATH=$(MASTER_LIB_PATH)

LIB_FILES=$(MASTER_LIB_FILES)

PKG_DIR=$(MASTER_BASE)/$(LIB_SRC)/logging/testharness

OBJ_FILES0=test.o
OBJ_FILES1=play.o

TARGET0=$(MASTER_BASE)/$(APP_DST)/test/liblogging_test
TARGET1=$(MASTER_BASE)/$(APP_DST)/play/liblogging_play

# make sure they both get built
all: $(TARGET0) $(TARGET1)

$(TARGET0): $(OBJ_FILES0)
	-@echo "        - linking ... $(TARGET0)"
	-@$(CC) $(CC_FLAGS) -o $(TARGET0) $(OBJ_FILES0) $(LIB_PATH) $(LIB_FILES)

$(TARGET1): $(OBJ_FILES1)
	-@echo "        - linking ... $(TARGET1)"
	-@$(CC) $(CC_FLAGS) -o $(TARGET1) $(OBJ_FILES1) $(LIB_PATH) $(LIB_FILES)

%.o: $(PKG_DIR)/%.cpp
	-@echo "        - building $@"
	-@$(CC) -c $(CC_FLAGS) $(INCLUDE_PATH) -o $@ $<

idls:

cleanall:
	-@rm $(OBJ_FILES0)
	-@rm $(OBJ_FILES1)
	-@rm $(TARGET0)
	-@rm $(TARGET1)

