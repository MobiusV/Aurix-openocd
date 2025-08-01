# SPDX-License-Identifier: GPL-2.0-or-later

noinst_LTLIBRARIES += %D%/libocdtasclient.la
%C%_libocdtasclient_la_CPPFLAGS = \
	-D_WINSOCK_DEPRECATED_NO_WARNINGS \
	-D_WIN32_WINNT=0x0600 \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/jimtcl \
	-I$(top_builddir)/jimtcl
%C%_libocdtasclient_la_SOURCES = $(TAS_CLIENT_SRC)

TAS_CLIENT_SRC = %D%/tas_protocol.c %D%/tas_client.c %D%/tas_sock.c
