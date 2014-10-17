# Copyright (c) 2014 The xsd2cc Authors. All rights reserved.
# Use of this source code is governed by GPL license that can be
# found in the LICENSE file. See the AUTHORS file for names of contributors.
###############################################################################

include ../build_config.mk

Static_lib= libxsd2cc.$(PLATFORM_STATIC_EXT)

CC_static_lib_SRC_cc= \
	xml_document.cc \
	xml_node.cc \
	utility.cc \
	parse.cc

CC_bin= xsd2cc

CC_bin_SRC_cc= \
	$(CC_static_lib_SRC_cc) \
	element.cc \
	simple_type.cc \
	type_base.cc \
	attribute.cc \
	application.cc \
	complex_type.cc \
	xsd.cc \
	xsd2cc.cc

XSD2CC_HEADER_DIR= ../include
LIBXML_HEADER_DIR= /usr/include/libxml2

MY_COMMON_FLAGS= -I $(XSD2CC_HEADER_DIR) -I$(LIBXML_HEADER_DIR)

MY_LIBS= -lxml2

XSD2CC_SLIB_HEADERS= \
	$(XSD2CC_HEADER_DIR)/xsd2cc/xsd2cc.h \
	$(XSD2CC_HEADER_DIR)/xsd2cc/xml_document.h \
	$(XSD2CC_HEADER_DIR)/xsd2cc/xml_node.h \
	$(XSD2CC_HEADER_DIR)/xsd2cc/utility.h

XSD2CC_HEADERS= \
	$(XSD2CC_SLIB_HEADERS) \
	$(XSD2CC_HEADER_DIR)/xsd2cc/application.h \
	$(XSD2CC_HEADER_DIR)/xsd2cc/attribute.h \
	$(XSD2CC_HEADER_DIR)/xsd2cc/complex_type.h \
	$(XSD2CC_HEADER_DIR)/xsd2cc/element.h \
	$(XSD2CC_HEADER_DIR)/xsd2cc/parse.h \
	$(XSD2CC_HEADER_DIR)/xsd2cc/simple_type.h \
	$(XSD2CC_HEADER_DIR)/xsd2cc/split.h \
	$(XSD2CC_HEADER_DIR)/xsd2cc/type_base.h \
	$(XSD2CC_HEADER_DIR)/xsd2cc/xsd.h


all $(Static_lib): static_lib cc_bin

test: $(Static_lib) ../tools/xsd2cc_run.sh
	sh ../tools/xsd2cc_run.sh ../tests

check: test

sample: $(Static_lib) ../tools/xsd2cc_run.sh
	sh ../tools/xsd2cc_run.sh ../samples

.PHONY: install uninstall clean

install: $(Static_lib) $(CC_bin) $(XSD2CC_SLIB_HEADERS)
	mkdir -p $(DESTDIR)/usr/local/include/xsd2cc
	mkdir -p $(DESTDIR)/usr/local/lib
	mkdir -p $(DESTDIR)/usr/local/bin
	install -p $(INSTALL_FLAGS) -m 0644 -o root $(Static_lib) $(DESTDIR)/usr/local/lib
	install -c -p $(INSTALL_FLAGS) -m 0644 -o root $(XSD2CC_SLIB_HEADERS) $(DESTDIR)/usr/local/include/xsd2cc
	install -p $(INSTALL_FLAGS) -m 755 -o root $(CC_bin) $(DESTDIR)/usr/local/bin/$(CC_bin)

uninstall:
	rm -rf $(DESTDIR)/usr/local/include/xsd2cc
	rm -rf $(DESTDIR)/usr/local/lib/$(Static_lib)
	rm -rf $(DESTDIR)/usr/local/bin/$(CC_bin)

include ./Makefile.inc

