MFR_DIR=../..
include $(MFR_DIR)/_makefile.rules

.PHONY: idls

CC=$(MASTER_CC)
CC_FLAGS=$(MASTER_FLAGS)

INCLUDE_PATH=$(MASTER_INCL_PATH)

# set the package paths
PKG_DIR=$(MASTER_BASE)/$(LIB_SRC)/daemonlib

OBJ_FILES=\
    sx_daemon.o \
    sx_process.o \
    sc_statequeue.o \
    sc_daemon.o \
    sc_processwrapperthread.o \
    sc_heartbeatthread.o \
    sc_processmanager.o 


TARGET=$(MASTER_BASE)/$(LIB_DST)/libdaemonlib.so

$(TARGET): $(OBJ_FILES)
	-@echo "        - linking ... $(TARGET)"
	$(CC) $(CC_FLAGS) $(MASTER_LIB_FLAGS) -o $(TARGET) $(OBJ_FILES)

%.o: $(PKG_DIR)/%.cpp $(PKG_DIR)/%.hpp
	-@echo "        - building $@"
	$(CC) -c $(CC_FLAGS) $(MASTER_LIB_FLAGS) $(MASTER_LIB_FLAGS) $(INCLUDE_PATH) -o $@ $<

idls:

.SUFFIXES:	.o .cpp .cc


