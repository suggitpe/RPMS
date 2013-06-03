MFR_DIR=../..
include $(MFR_DIR)/_makefile.rules

.PHONY: idls

CC=$(MASTER_CC)
CC_FLAGS=$(MASTER_FLAGS)

INCLUDE_PATH=$(MASTER_INCL_PATH)

# set the package paths
PKG_DIR=$(MASTER_BASE)/$(LIB_SRC)/sockets

OBJ_FILES=\
    sx_socket.o \
    sx_sockettimeout.o \
    sx_socketconnectrefused.o \
    sx_socketnoendpoint.o \
    sx_socketinterrupt.o \
    sa_socket.o \
    sc_udpsocket.o \
    sc_tcpsocket.o \
    sc_inetaddress.o


TARGET=$(MASTER_BASE)/$(LIB_DST)/libsockets.so

$(TARGET): $(OBJ_FILES)
	-@echo "        - linking ... $(TARGET)"
	$(CC) $(CC_FLAGS) $(MASTER_LIB_FLAGS) -o $(TARGET) $(OBJ_FILES)

%.o: $(PKG_DIR)/%.cpp $(PKG_DIR)/%.hpp
	-@echo "        - building $@"
	$(CC) -c $(CC_FLAGS) $(MASTER_LIB_FLAGS) $(MASTER_LIB_FLAGS) $(INCLUDE_PATH) -o $@ $<

idls:

.SUFFIXES:	.o .cpp .cc


