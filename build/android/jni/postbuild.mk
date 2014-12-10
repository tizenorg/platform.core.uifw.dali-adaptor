all: $(PREBUILT_LIBDIR)/libdaliport.a
intdir:= $(TARGET_OUT)
$(PREBUILT_LIBDIR)/libdaliport.a: $(intdir)/libdaliport.a
	$(TOP_PATH)/../tools/lib_update_needed.sh $(intdir)/libdaliport.a $(TOP_PATH)
