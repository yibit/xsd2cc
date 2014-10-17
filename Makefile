# Copyright (c) 2014 The xsd2cc Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file. See the AUTHORS file for names of contributors.
###############################################################################

all test check sample clean install uninstall: platform-deps
	cd src && $(MAKE) $@

platform-deps:
	sh ./tools/configure.sh build_config.mk .

distclean:
	rm -f src/*.o src/*.a src/*.so src/*.sl src/*.a_* src/*.so_* src/*.sl_*
	rm -f samples/*.o samples/*_sample samples/*_sample_*
	rm -f tests/*.o tests/*_test tests/*_test_* tests/*_test.h tests/*_test.cc
	rm -f src/xsd2cc src/xsd2cc_*
	find . -name \*~ -type f |xargs rm -f
	find . -name \*.bak -type f |xargs rm -f
	find . -name .prog*_config* -type f |xargs rm -f

