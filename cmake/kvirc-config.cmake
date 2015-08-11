#!/bin/sh
#
#   A script for retrieving the latest kvirc build configuration
#   Mainly used for building plugins out of the source tree
#   The idea is "stolen" from the gtk-config and xmms-config scripts :)
#
#
#   09-04-2000 Szymon Stefanek (Initial release)
#   31-01-2002 Szymon Stefanek (Revised for KVIrc 3.0.0)
#   27-07-2002 Szymon Stefanek (Revised again for KVIrc 3.0.0)
#   01-06-2007 Szymon Stefanek (Revised for SST)
#   ??-10-2007 Szymon Stefanek (Revised for Humor)
#   14-04-2008 CtrlAltCa (adapted to cmake branch)
#   24-06-2015 Namikaze (echo to printf, cleanup)
#
#   This program is FREE software. You can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   as published by the Free Software Foundation; either version 2
#   of the License, or (at your opinion) any later version.
#
#   This program is distributed in the HOPE that it will be USEFUL,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#   See the GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program. If not, write to the Free Software Foundation,
#   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#

set -e

print_syntax()
{
	printf "%s\n" "@PACKAGE@@VERSION_MAJOR@-config (KVIrc @VERSION_RELEASE@)" \
		"	A script for retrieving the latest KVIrc build configuration" \
		"" \
		"Syntax : @PACKAGE@@VERSION_MAJOR@-config [OPTIONS]" \
		"  options:" \
		"    --version         : Version of the KVIrc toolkit." \
		"    --prefix          : The prefix where KVIrc was installed." \
		"    --lib-suffix      : The suffix (if any) for the library directory where KVIrc was installed." \
		"    --revision        : The KVIrc revision number." \
		"    --date            : The KVIrc build date." \
		"    --type            : The KVIrc build type (Debug, Release, ..)."
	exit 0
}

if test $# -eq 0; then
	print_syntax 1 1>&2
fi

KVIRC_STUFF_TO_ECHO=""

while test $# -gt 0; do
	case "$1" in
		-*=*) optarg=`printf "$1" | sed 's/[-_a-zA-Z0-9]*=//'` ;;
		*) optarg= ;;
	esac

	case $1 in
		--version)
		KVIRC_STUFF_TO_ECHO="$KVIRC_STUFF_TO_ECHO @VERSION_RELEASE@\n"
		;;
		--prefix)
		KVIRC_STUFF_TO_ECHO="$KVIRC_STUFF_TO_ECHO @CMAKE_INSTALL_PREFIX@\n"
		;;
		--lib-suffix)
		KVIRC_STUFF_TO_ECHO="$KVIRC_STUFF_TO_ECHO @LIB_SUFFIX@\n"
		;;
		--revision)
		KVIRC_STUFF_TO_ECHO="$KVIRC_STUFF_TO_ECHO @CMAKE_KVIRC_BUILD_REVISION@\n"
		;;
		--date)
		KVIRC_STUFF_TO_ECHO="$KVIRC_STUFF_TO_ECHO @CMAKE_KVIRC_BUILD_DATE@\n"
		;;
		--type)
		KVIRC_STUFF_TO_ECHO="$KVIRC_STUFF_TO_ECHO @CMAKE_BUILD_TYPE@\n"
		;;
		*)
		print_syntax 1 1>&2
		;;
	esac
	shift
done

if test -n "$KVIRC_STUFF_TO_ECHO"; then
	printf $KVIRC_STUFF_TO_ECHO
fi
