MFR_DIR=../../..
include $(MFR_DIR)/_makefile.rules

.PHONY: idls

CC=$(MASTER_CC)
CC_FLAGS=$(MASTER_FLAGS)

INCLUDE_PATH=$(MASTER_INCL_PATH)

LIB_PATH=$(MASTER_LIB_PATH)

LIB_FILES=$(MASTER_LIB_FILES)

PKG_DIR=$(MASTER_BASE)/$(LIB_SRC)/xmlutil/testharness

OBJ_FILES0=domparserplay.o
OBJ_FILES1=test_xml.o
OBJ_FILES2=play.o

TARGET0=$(MASTER_BASE)/$(APP_DST)/play/libxmlutil_play
TARGET1=$(MASTER_BASE)/$(APP_DST)/test/libxmlutil_test
TARGET2=$(MASTER_BASE)/$(APP_DST)/play/libxmlutil_play2

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
	$(CC) $(CC_FLAGS) -o $(TARGET2) $(OBJ_FILES2) $(LIB_PATH) -lxerces-c

%.o: $(PKG_DIR)/%.cpp
	-@echo "        - building $@"
	$(CC) -c $(CC_FLAGS) $(INCLUDE_PATH) -o $@ $<

idls:

cleanall:
	-@rm $(OBJ_FILES0)
	-@rm $(TARGET0)

.SUFFIXES: .idl .o .cpp .hpp .hh .cc .h
