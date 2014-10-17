# Copyright (c) 2013-2014 The xsd2cc Authors. All rights reserved.
# Use of this source code is governed by GPL license that can be
# found in the LICENSE file. See the AUTHORS file for names of contributors.
###############################################################################
#!/bin/sh
#
# Detects OS we're compiling on and outputs a file specified by the first
# argument, which in turn gets read while processing Makefile.
#
# The output will set the following variables:
#   AR                          Archiver
#   CC                          C Compiler path
#   CXX                         C++ Compiler path
#   CC_LD                       C Linker
#   CXX_LD                      C++ Linker
#   PLATFORM_LDFLAGS            Linker flags
#   PLATFORM_LIBS               Libraries flags
#   PLATFORM_SHARED_EXT         Extension for shared libraries
#   PLATFORM_SHARED_LDFLAGS     Flags for building shared library
#                               This flag is embedded just before the name
#                               of the shared library without intervening spaces
#   PLATFORM_SHARED_CFLAGS      Flags for compiling objects for shared library
#   PLATFORM_CCFLAGS            C compiler flags
#   PLATFORM_CXXFLAGS           C++ compiler flags.  Will contain:
#   PLATFORM_SHARED_VERSIONED   Set to 'true' if platform supports versioned
#                               shared libraries, empty otherwise.
#
# The PLATFORM_CCFLAGS and PLATFORM_CXXFLAGS might include the following:
#

OUTPUT=$1
PREFIX=$2
FORCE=$3
if test -z "$OUTPUT" || test -z "$PREFIX"; then
    echo "usage: $0 <output-filename> <directory_prefix> [-f]" >&2
    exit 1
fi

# Delete existing output, if it exists
OUTPUT=$PREFIX/$OUTPUT

if test -f $OUTPUT -a -z "$FORCE"; then
    exit 0    
fi

rm -f $OUTPUT
touch $OUTPUT

if test -z "$AR"; then
    AR=ar
fi

if test -z "$CC"; then
    CC=cc
fi

if test -z "$CXX"; then
    CXX=g++
fi

if test -z "$LD"; then
    LD=ld
fi

CC_LD=$LD
CXX_LD=$LD

# Detect OS
if test -z "$TARGET_OS"; then
    TARGET_OS=`uname -s`
fi

# Detect machine hardware name
if test -z "$TARGET_M"; then
    TARGET_M=`uname -m`
fi

ARFLAGS=rcu
COMMON_FLAGS=
CROSS_COMPILE=
PLATFORM_CCFLAGS=
PLATFORM_CXXFLAGS=
PLATFORM_LDFLAGS=
PLATFORM_LIBS=
PLATFORM_SHARED_EXT="so"
PLATFORM_STATIC_EXT="a"
PLATFORM_SHARED_LDFLAGS="-shared -Wl,-soname -Wl,"
PLATFORM_SHARED_CFLAGS=
PLATFORM_SHARED_VERSIONED=true
ECHO_OPT=

case "$TARGET_OS" in
    AIX)
        CC=xlc
        CXX=xlC
        ARFLAGS= -X64 rcu
        PLATFORM=OS_IBMAIX
        COMMON_FLAGS="-DOS_AIX"
        PLATFORM_CCFLAGS="-q64 -D_THREAD_SAFE -D_H_LOCALEDEF"
        PLATFORM_CXXFLAGS="-qrtti -q64 -D_THREAD_SAFE"
        PLATFORM_LDFLAGS="-brtl -bexpall"
        PLATFORM_SHARED_LDFLAGS="-b64 -G -bnoentry -brtl -bexpall"
        PLATFORM_LIBS="-lpthread -lodm -lcfg -lrt -lm"
        ;;
    Darwin)
        PLATFORM=OS_MACOSX
        COMMON_FLAGS="-DOS_MACOSX"
        PLATFORM_SHARED_EXT=dylib
        [ -z "$INSTALL_PATH" ] && INSTALL_PATH=`pwd`
        PLATFORM_SHARED_LDFLAGS="-dynamiclib -install_name $INSTALL_PATH/"
        ;;
    Linux)
        PLATFORM=OS_LINUX
        CXX_LD=$CXX
        CC_LD=$CC
        COMMON_FLAGS="-O2 -pthread -DOS_LINUX -fPIC"        
        PLATFORM_LDFLAGS="-lpthread -Wl,-E -ldl"
        PLATFORM_SHARED_CFLAGS="-fPIC"
        PLATFORM_LIBS="-ldl -lm"
        ECHO_OPT="-e"
        ;;
    SunOS)
        PLATFORM=OS_SOLARIS
        COMMON_FLAGS="-D_REENTRANT -DOS_SOLARIS"
        PLATFORM_LIBS="-lpthread -lsocket -lnsl -lrt"
        ;;
    FreeBSD)
        PLATFORM=OS_FREEBSD
        COMMON_FLAGS="-D_REENTRANT -DOS_FREEBSD"
        PLATFORM_LIBS="-lpthread"
        ;;
    NetBSD)
        PLATFORM=OS_NETBSD
        CC_LD=$CC
        CXX_LD=$CXX
        COMMON_FLAGS="-D_REENTRANT -DOS_NETBSD"
        PLATFORM_LIBS="-lpthread -lgcc_s"
        ;;
    OpenBSD)
        PLATFORM=OS_OPENBSD
        COMMON_FLAGS="-D_REENTRANT -DOS_OPENBSD"
        PLATFORM_LDFLAGS="-pthread"
        ;;
    DragonFly)
        PLATFORM=OS_DRAGONFLYBSD
        COMMON_FLAGS="-D_REENTRANT -DOS_DRAGONFLYBSD"
        PLATFORM_LIBS="-lpthread"
        ;;
    OS_ANDROID_CROSSCOMPILE)
        PLATFORM=OS_ANDROID
        COMMON_FLAGS="-D_REENTRANT -DOS_ANDROID -DLEVELDB_PLATFORM_POSIX"
        PLATFORM_LDFLAGS=""  # All pthread features are in the Android C library
        CROSS_COMPILE=true
        ;;
    HP-UX)
        PLATFORM=OS_HPUX
        CXX=aCC        
        PLATFORM_SHARED_EXT="sl"
        COMMON_FLAGS="-DOS_HPUX"
        if test "$TARGET_M" = "ia64"; then
            PLATFORM_CXXFLAGS=" +DD64 \
                                -AA \
                                -Wl,-E \
                                -D_RWSTD_MULTI_THREAD -D_REENTRANT \
                                +W829,749 \
                                -D__HP_NO_MATH_OVERLOADS \
                                -D__HPACC_STRICTER_ANSI__ \
                                -D__HPACC_USING_MULTIPLIES_IN_FUNCTIONAL\
                                +inst_implicit_include"
                                
            PLATFORM_SHARED_LDFLAGS="-b"
            PLATFORM_LIBS="-lpthread -lxti -lm -lrt"
            
        else
            PLATFORM_CXXFLAGS=" -AA +z -lstd_v2 -lCsup_v2 \
                                -Wl,-E \
                                -D_RWSTD_MULTI_THREAD -D_REENTRANT \
                                +W829,749 \
                                -D__HP_NO_MATH_OVERLOADS \
                                -D__HPACC_STRICTER_ANSI__ \
                                -D__HPACC_USING_MULTIPLIES_IN_FUNCTIONAL \
                                +inst_implicit_include"
                                
            PLATFORM_SHARED_LDFLAGS="-b -lc"
            PLATFORM_LIBS="-lpthread -lxti -lm -lc -lrt"
        fi
        ;;
    OSF1)
        PLATFORM=OS_COMPAQ
        CXX=cxx        
        COMMON_FLAGS="-D_REENTRANT -DOS_COMPAQ"
        PLATFORM_CXXFLAGS="-D__USE_STD_IOSTREAM -D_POSIX_PII_SOCKET -D_SOCKADDR_LEN"
        PLATFORM_LDFLAGS="-pthread"
        PLATFORM_SHARED_LDFLAGS="-shared -no_archive -lc"
        PLATFORM_LIBS="-lm"
        ;;
    *)
        echo "Unknown platform!" >&2
        exit 1
esac

MEMCMP_FLAG=
if test "$CXX" = "g++"; then
    # Use libc's memcmp instead of GCC's memcmp.  This results in ~40%
    # performance improvement on readrandom under gcc 4.4.3 on Linux/x86.
    MEMCMP_FLAG="-fno-builtin-memcmp"
fi

COMMON_FLAGS="$COMMON_FLAGS $MEMCMP_FLAG"
PLATFORM_CCFLAGS="$PLATFORM_CCFLAGS $COMMON_FLAGS"
PLATFORM_CXXFLAGS="$PLATFORM_CXXFLAGS $COMMON_FLAGS"

echo "CC=$CC" >> $OUTPUT
echo "CXX=$CXX" >> $OUTPUT
echo "CC_LD=$CC_LD" >> $OUTPUT
echo "CXX_LD=$CXX_LD" >> $OUTPUT
echo "AR=$AR" >> $OUTPUT
echo "ARFLAGS=$ARFLAGS" >> $OUTPUT
echo "PLATFORM=$PLATFORM" >> $OUTPUT
echo "PLATFORM_LDFLAGS=$PLATFORM_LDFLAGS" >> $OUTPUT
echo "PLATFORM_LIBS=$PLATFORM_LIBS" >> $OUTPUT
echo "PLATFORM_CCFLAGS=$PLATFORM_CCFLAGS" >> $OUTPUT
echo "PLATFORM_CXXFLAGS=$PLATFORM_CXXFLAGS" >> $OUTPUT
echo "PLATFORM_SHARED_CFLAGS=$PLATFORM_SHARED_CFLAGS" >> $OUTPUT
echo "PLATFORM_SHARED_EXT=$PLATFORM_SHARED_EXT" >> $OUTPUT
echo "PLATFORM_STATIC_EXT=$PLATFORM_STATIC_EXT" >> $OUTPUT
echo "PLATFORM_SHARED_LDFLAGS=$PLATFORM_SHARED_LDFLAGS" >> $OUTPUT
echo "PLATFORM_SHARED_VERSIONED=$PLATFORM_SHARED_VERSIONED" >> $OUTPUT
echo "ECHO_OPT=$ECHO_OPT" >> $OUTPUT
echo "" >> $OUTPUT

cat $OUTPUT
