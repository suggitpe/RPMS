MFR_DIR=../../..
include $(MFR_DIR)/_makefile.rules

.PHONY: idls

CC=$(MASTER_CC)
CC_FLAGS=$(MASTER_FLAGS)

INCLUDE_PATH=$(MASTER_INCL_PATH)

LIB_PATH=$(MASTER_LIB_PATH)

LIB_FILES=$(MASTER_LIB_FILES)

PKG_DIR=$(MASTER_BASE)/$(LIB_SRC)/tools/testharness

OBJ_FILES=test.o

TARGET=$(MASTER_BASE)/$(APP_DST)/test/libtools_test

$(TARGET): $(OBJ_FILES)
	-@echo "        - linking ... $(TARGET)"
	-@$(CC) $(CC_FLAGS) -o $(TARGET) $(OBJ_FILES) $(LIB_PATH) $(LIB_FILES)

%.o: $(PKG_DIR)/%.cpp
	-@echo "        - building $@"
	-@$(CC) -c $(CC_FLAGS) $(INCLUDE_PATH) -o $@ $<

idls:

cleanall:
	-@rm $(OBJ_FILES)
	-@rm $(TARGET)

