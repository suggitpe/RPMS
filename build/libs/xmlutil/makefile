MFR_DIR=../..
include $(MFR_DIR)/_makefile.rules

.PHONY: idls

CC=$(MASTER_CC)
CC_FLAGS=$(MASTER_FLAGS)

INCLUDE_PATH=$(MASTER_INCL_PATH)

# set the package paths
PKG_DIR=$(MASTER_BASE)/$(LIB_SRC)/xmlutil

OBJ_FILES=\
    sx_xmlutil.o \
    ss_xmlutil.o \
    sa_xmldocument.o \
    sc_xmlnodeconfig.o \
    sc_xmlsysconfig.o \
    sc_xmlerrhandler.o \
    sc_xmlentresolver.o \
    sc_xmlstatus.o \
    sc_xmlprocstats.o \
    sc_xmltransformer.o


TARGET=$(MASTER_BASE)/$(LIB_DST)/libxmlutil.so

$(TARGET): $(OBJ_FILES)
	-@echo "        - linking ... $(TARGET)"
	$(CC) $(CC_FLAGS) $(MASTER_LIB_FLAGS) -o $(TARGET) $(OBJ_FILES)

%.o: $(PKG_DIR)/%.cpp $(PKG_DIR)/%.hpp
	-@echo "        - building $@"
	$(CC) -c $(CC_FLAGS) $(MASTER_LIB_FLAGS) $(MASTER_LIB_FLAGS) $(INCLUDE_PATH) -o $@ $<
    
idls:

.SUFFIXES:	.o .cpp .cc


