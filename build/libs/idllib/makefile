MFR_DIR=../..
include $(MFR_DIR)/_makefile.rules

.PHONY: idls

CC=$(MASTER_CC)
CC_FLAGS=$(MASTER_FLAGS)

INCLUDE_PATH=$(MASTER_INCL_PATH)

# set the package paths
PKG_DIR=$(MASTER_BASE)/$(LIB_SRC)/idllib

OBJ_FILES=\
    sr_daemontask.o \
    sr_directortask.o 


TARGET=$(MASTER_BASE)/$(LIB_DST)/libidllib.so

$(TARGET): $(OBJ_FILES)
	-@echo "        - linking ... $(TARGET)"
	$(CC) $(CC_FLAGS) $(MASTER_LIB_FLAGS) -o $(TARGET) $(OBJ_FILES)

%.o: $(PKG_DIR)/idls/%.cc
	-@echo "        - building $@"
	$(CC) -c $(CC_FLAGS) $(INCLUDE_PATH) -o $@ $<

idls:
	-@echo "        - building idllib idls"
	-@cd $(PKG_DIR)/idls; \
    for a in $(PKG_DIR)/*.idl; \
        do $(IDL) $(IDL_FLAGS) $${a}; \
    done


.SUFFIXES:	.o .cpp .cc


