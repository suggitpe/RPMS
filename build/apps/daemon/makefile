MFR_DIR=../..
include $(MFR_DIR)/_makefile.rules

CC=$(MASTER_CC)
CC_FLAGS=$(MASTER_FLAGS)

INCLUDE_PATH=$(MASTER_INCL_PATH)

LIB_PATH=$(MASTER_LIB_PATH)

LIB_FILES=$(MASTER_LIB_FILES) $(DAEMON_LIBS)

PKG_DIR=$(MASTER_BASE)/$(APP_SRC)/daemon
BASE_PKG_DIR=$(MASTER_BASE)/$(APP_SRC)/base

OBJ_FILES=sc_daemonapp.o

BASE_OBJ_FILES=sa_baseapp.o

TARGET=$(MASTER_BASE)/$(APP_DST)/RpmsDaemon

$(TARGET): $(OBJ_FILES) $(BASE_OBJ_FILES)
	-@echo "        - linking ... $(TARGET)"
	$(CC) $(CC_FLAGS) -o $(TARGET) $(OBJ_FILES) $(BASE_OBJ_FILES) $(LIB_PATH) $(LIB_FILES)

%.o: $(PKG_DIR)/%.cpp $(PKG_DIR)/%.hpp
	-@echo "        - building $@"
	$(CC) -c $(CC_FLAGS) $(INCLUDE_PATH) -o $@ $<

%.o: $(BASE_PKG_DIR)/%.cpp $(BASE_PKG_DIR)/%.hpp
	-@echo "        - building $@"
	$(CC) -c $(CC_FLAGS) $(INCLUDE_PATH) -o $@ $<

cleanall:
	-@rm $(OBJ_FILES)
	-@rm $(TARGET)

.SUFFIXES: .o .cpp .cc
