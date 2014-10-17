# Copyright (c) 2014 The xsd2cc Authors. All rights reserved.
# Use of this source code is governed by GPL license that can be
# found in the LICENSE file. See the AUTHORS file for names of contributors.
###############################################################################

include ../build_config.mk

CC_bin= resource_test

CC_bin_SRC_cc= \
	resource_test.cc \
	main.cc

XSD2CC_HEADER_DIR= ../include
LIBXML_HEADER_DIR= /usr/include/libxml2

MY_COMMON_FLAGS= -I $(XSD2CC_HEADER_DIR) -I$(LIBXML_HEADER_DIR)

MY_LIBS= -L../src -lxsd2cc -lxml2

all: xsd2cc cc_bin

resource_test.cc: xsd2cc

xsd2cc: 
	../src/xsd2cc resource_test.xsd

include ../src/Makefile.inc

