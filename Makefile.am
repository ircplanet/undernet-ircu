## AutoMake top-level Makefile for the Undernet IRC Daemon
##
## Copyright (C) 2005 by Kevin L. Mitchell <klmitch@mit.edu>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
##
## @(#)$Id$

SUBDIRS = ircd

EXTRA_DIST = doc patches tools include Doxyfile INSTALL_FR ircd-patch \
	ChangeLog.11 ChangeLog.12 LICENSE RELEASE.NOTES

# Install man page and example.conf
man_MANS = doc/ircd.8
sysconf_DATA = doc/example.conf

# Make sure to filter out those CVS directories
dist-hook:
	find $(distdir) -name CVS -print -prune | xargs rm -rf
