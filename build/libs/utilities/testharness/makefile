MFR_DIR=../../..
include $(MFR_DIR)/_makefile.rules

.PHONY: idls

CC=$(MASTER_CC)
CC_FLAGS=$(MASTER_FLAGS)

INCLUDE_PATH=$(MASTER_INCL_PATH)

LIB_PATH=$(MASTER_LIB_PATH)

LIB_FILES=$(MASTER_LIB_FILES)

PKG_DIR=$(MASTER_BASE)/$(LIB_SRC)/utilities/testharness

OBJ_FILES=test.o
OBJ0_FILES=play.o

TARGET=$(MASTER_BASE)/$(APP_DST)/test/libutilities_test
TARGET0=$(MASTER_BASE)/$(APP_DST)/test/libutilities_test0

all: $(TARGET) $(TARGET0)

$(TARGET): $(OBJ_FILES)
	-@echo "        - linking ... $(TARGET)"
	-@$(CC) $(CC_FLAGS) -o $(TARGET) $(OBJ_FILES) $(LIB_PATH) $(LIB_FILES)

$(TARGET0): $(OBJ0_FILES)
	-@echo "        - linking ... $(TARGET0)"
	-@$(CC) $(CC_FLAGS) -o $(TARGET0) $(OBJ0_FILES) $(LIB_PATH) $(LIB_FILES)

%.o: $(PKG_DIR)/%.cpp
	-@echo "        - building $@"
	-@$(CC) -c $(CC_FLAGS) $(INCLUDE_PATH) -o $@ $<

idls:

cleanall:
	-@rm $(OBJ_FILES)
	-@rm $(TARGET)

