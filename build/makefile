# include the makefile rules
include _makefile.rules

.SILENT:

# this makefile is a phony makefile in that it does not actually make anything
#   instead it simply iterates through the system calling make in the correct places.
.PHONY : all dirs testdirs libs apps docs tests cleanall cleanlibs cleanapps cleantests cleandocs web example_apps


all: dirs libs apps example_apps
	-@echo "... build complete"
	-@echo "***************************************"

install: dirs idls libs apps example_apps tests web
	-@echo "... build install complete"
	-@echo "***************************************"


dirs:
	-@echo "--> BUILDING INSTALL DIR";
	-@mkdir -p $(MASTER_BASE)/$(LIB_DST)
	-@mkdir -p $(MASTER_BASE)/$(DOC_DST)
	-@mkdir -p $(MASTER_BASE)/$(APP_DST)
	-@echo "... build install dirs complete"
	-@echo "***************************************";

testdirs:
	-@echo "--> BUILDING TEST INSTALL DIRS";
	-@mkdir -p $(MASTER_BASE)/$(APP_DST)/test
	-@mkdir -p $(MASTER_BASE)/$(APP_DST)/play
	-@echo "***************************************";

idls:
	-@echo "--> BUILDING LIBRARY IDLS";
	-@cd $(MASTER_BASE)/$(LIB_BLD); \
        for a in `ls`; \
            do make -C $${a} idls || echo "####### FAILED TO BUILD $(MASTER_BASE)/$(LIB_BLD)/$${a}"; \
        done
	-@echo "***************************************";

libs:
	-@echo "--> BUILDING LIBRARIES";
	-@cd $(MASTER_BASE)/$(LIB_BLD); \
        for a in `ls`; \
            do echo "     - building `pwd`/$${a}"; \
            make -C $${a} || echo "####### FAILED TO BUILD $(MASTER_BASE)/$(LIB_BLD)/$${a}"; \
        done
	-@echo "***************************************";

tests: testdirs
	-@echo "--> BUILDING TESTHARNESS IDLS";
	-@cd $(MASTER_BASE)/$(LIB_BLD); \
        for a in `find . -name testharness | sort`; \
            do make -C $${a} idls || echo "####### FAILED TO BUILD $(MASTER_BASE)/$(LIB_BLD)/$${a} idls "; \
        done
	-@echo "--> BUILDING TESTHARNESSES ";
	-@cd $(MASTER_BASE)/$(LIB_BLD); \
        for a in `find . -name testharness | sort`; \
            do echo "     - building `pwd`/$${a}"; \
            make -C $${a} || echo "####### FAILED TO BUILD $(MASTER_BASE)/$(LIB_BLD)/$${a}"; \
        done
	-@echo "... build tests complete"
	-@echo "***************************************";

apps:
	-@echo "--> BUILDING EXECUTABLES"
	-@cd  $(MASTER_BASE)/$(APP_BLD); \
		for a in `ls`; \
		do \
			(cd $${a}; \
			echo "    - building `pwd`"; \
			make; \
			if [ $${?} -ne 0 ]; \
			then echo "###### FAILED TO BUILD $${a}"; \
			fi); \
		done
	-@echo "... build apps complete"
	-@echo "***************************************"

example_apps:
	-@echo "--> BUILDING EXAMPL_APPS"
	-@cd  $(MASTER_BASE)/$(EX_APP_BASE)/build; make;
	-@echo "... example apps build complete"
	-@echo "***************************************"

docs:
	-@echo ".. building documentation with Doxygen"
	-@$(DOX_EXE) $(DOX_CFG)
	-@echo "***************************************"

   
#cleaning section
cleanall: cleanlibs cleanapps cleantests cleandocs
	-@echo "... clean complete"

cleanlibs:
	-@echo "--> CLEANING LIBRARIES";
	-@rm -f $(MASTER_BASE)/$(LIB_DST)/*
	-@cd $(MASTER_BASE)/$(LIB_BLD); \
		for a in `ls`; \
		do \
			(cd $${a}; \
			echo "     - cleaning `pwd`"; \
			rm -f *\.o); \
		done
	-@echo "... clean libs complete"
	-@echo "***************************************";

cleanapps:
	-@echo "--> CLEANING APPS"
	-@cd  $(MASTER_BASE)/$(APP_BLD); \
		for a in `ls`; \
		do \
			(cd $${a}; \
			echo "    - cleaning `pwd`"; \
			rm -f *\.o); \
		done
	-@echo "--> CLEANING EXECUTABLES"
	-@cd  $(MASTER_BASE)/$(APP_DST); \
        chmod -R a+w *; \
		for a in `ls`; \
		do \
			( echo "    - removing `pwd`/$${a}"; \
            rm -fr $${a}); \
		done;
	-@echo "--> CLEANING EXAMPLE APPS";
	-@cd  $(MASTER_BASE)/$(EX_APP_BASE)/build; make cleanall;
	-@echo "... clean apps complete"
	-@echo "***************************************"

cleantests:
	-@echo "--> CLEANING TESTHARNESSES";
	-@cd $(MASTER_BASE)/$(LIB_BLD); \
        for a in `find . -name testharness | sort`; \
		do \
			(cd $${a}; \
			echo "     - cleaning `pwd`"; \
			rm -f *\.o; \
			rm -f *_test; \
			rm -f play_*); \
		done
	-@echo "... clean tests complete"
	-@rm -f $(MASTER_BASE)/$(APP_DST)/test
	-@rm -f $(MASTER_BASE)/$(APP_DST)/play
	-@echo "***************************************";

cleandocs:
	-@echo ".. cleaning documentation"
	-@rm -fr $(MASTER_BASE)/$(DOC_DST)/*
	-@echo "***************************************"

web: docs
	-@echo ".. installing web server to $(WEB_BASE)/doxygen/"
	-@rm -f $(WEB_BASE)/doxygen/*
	-@cp -f $(MASTER_BASE)/$(DOC_DST)/html/* $(WEB_BASE)/doxygen/
	-@echo "Web server installed"
