MFR_DIR=../../..
include $(MFR_DIR)/_makefile.rules

.PHONY: idls

CC=$(MASTER_CC)
CC_FLAGS=$(MASTER_FLAGS)

INCLUDE_PATH=$(MASTER_INCL_PATH)

LIB_PATH=$(MASTER_LIB_PATH)

LIB_FILES=$(MASTER_LIB_FILES) $(DAEMON_LIBS)

PKG_DIR=$(MASTER_BASE)/$(LIB_SRC)/distributor/testharness

OBJ_FILES0=test.o

TARGET0=$(MASTER_BASE)/$(APP_DST)/test/libdistributor_test

# make sure they both get built
all: $(TARGET0)

$(TARGET0): $(OBJ_FILES0)
	-@echo "        - linking ... $(TARGET0)"
	$(CC) $(CC_FLAGS) -o $(TARGET0) $(OBJ_FILES0) $(LIB_PATH) $(LIB_FILES)

%.o: $(PKG_DIR)/%.cpp
	-@echo "        - building $@"
	$(CC) -c $(CC_FLAGS) $(INCLUDE_PATH) -o $@ $<

idls:

cleanall:
	-@rm $(OBJ_FILES0)
	-@rm $(TARGET0)

