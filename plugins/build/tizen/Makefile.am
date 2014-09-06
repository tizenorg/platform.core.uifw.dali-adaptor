# Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved
#
# This file is part of Dali Adaptor
#
# PROPRIETARY/CONFIDENTIAL
#
# This software is the confidential and proprietary information of
# SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
# disclose such Confidential Information and shall use it only in
# accordance with the terms of the license agreement you entered
# into with SAMSUNG ELECTRONICS.
#
# SAMSUNG make no representations or warranties about the suitability
# of the software, either express or implied, including but not limited
# to the implied warranties of merchantability, fitness for a particular
# purpose, or non-infringement. SAMSUNG shall not be liable for any
# damages suffered by licensee as a result of using, modifying or
# distributing this software or its derivatives.

SUBDIRS = common dali-adaptor dali-application dali-evas dali-native-buffer plugins

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = dali.pc \
                 dali-application.pc \
                 dali-evas.pc \
                 dali-native-buffer.pc

MAINTAINERCLEANFILES = \
    aclocal.m4 \
    autom4te.cache \
    config.guess \
    config.sub \
    configure \
    depcomp \
    install-sh \
    ltmain.sh \
    missing \
    `find "$(srcdir)" -type f -name Makefile.in -print` \
    `find . \( -name "*.gcov" -o -name "*.gcno" -o -name "*.gcda" \) -print`

CLEANFILES = \
	`find . \( -name "*.gcov" -o -name "*.gcno" -o -name "*.gcda" \) -print`

COVERAGE_DIR=.cov
COVERAGE_OUTPUT_DIR=doc/coverage

cov_data:
	@test -z $(COVERAGE_DIR) || mkdir -p $(COVERAGE_DIR)
	@rm -f $(COVERAGE_DIR)/*
	@cp common/.libs/*.gcda common/.libs/*.gcno  $(COVERAGE_DIR)
	@for i in `find $(COVERAGE_DIR) -name "libdali_adaptor_common_la-*.gcda" -o -name "libdali_adaptor_common_la-*.gcno"` ;\
		do mv $$i `echo $$i | sed s/libdali_adaptor_common_la-//` ; echo $$i ; done
	@cd $(COVERAGE_DIR) ; lcov --base-directory . --directory . -c -o dali.info
	@cd $(COVERAGE_DIR) ; lcov --remove dali.info "*boost*" "dali-env/*" "/usr/include/*" -o dali.info
	@test -z $(COVERAGE_OUTPUT_DIR) || mkdir -p $(COVERAGE_OUTPUT_DIR)

coverage: cov_data
	@genhtml -o $(COVERAGE_OUTPUT_DIR) $(COVERAGE_DIR)/dali.info

reset_coverage:
	@lcov -z --directory `pwd`
