#!/bin/sh

# Print the gcc cpu specific options appropriate for the current CPU

# Author:
#    http://www.pixelbeat.org/
# Notes:
#    This script currently supports Linux,FreeBSD
#    This script is x86 (32 bit) specific
#    It should work on any gcc >= 2.95 at least
#    It only returns CPU specific options. You probably also want -03 etc.
# Changes:
#    V0.1, 12 Mar 2003, Initial release
#    V0.2, 01 Jun 2005, Added support for 3.2>=gcc<=4.0
#    V0.3, 03 Jun 2005, Added support for pentium-m
#    V0.4, 03 Jun 2005, Fix silly bugs
#    V0.5, 07 Jun 2005, Clarify/Simplify confusing floating point expr usage
#                       Print warning when CPU only supported on a newer gcc
#    V0.6, 15 Dec 2006, Added support for Intel Core and Core 2 processors
#                       Added support for 4.1>=gcc<=4.3
#                       Added support for gcc -msse3 option
#                       Added support for new gcc -march=native option
#    V0.7, 18 Dec 2006, Changes from Conor McDermottroe
#                       Added support for FreeBSD
#                       Remove bash specific constructs
#                       Better error handling
#    V0.8, 19 Dec 2006, Give warnings and 32 bit -march on 64 bit platforms.
#                       Previously it just gave an invalid blank -march.
#                       Reported and tested by Ewan Oughton.

if [ "$1" = "--version" ]; then
    echo "0.7" && exit
fi

# This table shows when -march options were introduced into _official_ gcc releases.
# Note there are vendor deviations that complicate this.
# For e.g. redhat introduced the prescott option in 3.3-13.
#   gcc-2.95   = i386, i486, i586,pentium, i686,pentiumpro, k6
#   gcc-3.0   += athlon
#   gcc-3.1   += pentium-mmx, pentium2, pentium3, pentium4, k6-2, k6-3, athlon-{tbird, 4,xp,mp}
#   gcc-3.3   += winchip-c6, winchip2, c3
#   gcc-3.4.0 += k8,opteron,athlon64,athlon-fx, c3-2
#   gcc-3.4.1 += pentium-m, pentium3m, pentium4m, prescott, nocona
#   gcc-4.3   += core2

[ -z "$CC" ] && CC=gcc

try_gcc_options() {
    $CC $* -S -o /dev/null -xc /dev/null >/dev/null 2>&1
}

if ! try_gcc_options -march=i386; then
    if ! try_gcc_options -m32 -march=i386; then
        echo "Error: This script only supports 32 bit x86 architectures" >&2
        exit 1
    else
        echo "Warning: The optimum *32 bit* architecture is reported" >&2
        m32="-m32 "
    fi
fi

try_line() {
    skip=0
    for arch in $1; do
        if try_gcc_options $m32 -march=$arch; then
            echo $arch
            return
        elif [ "$skip" = "0" ] && [ "$arch" != "native" ]; then
            skip=1
            echo "Warning: Newer versions of GCC better support your CPU with -march=$arch" >&2
        fi
    done
    return 1
}

read_cpu_data_linux() {
    IFS=":"
    while read name value; do
        unset IFS
        name=`echo $name`
        value=`echo $value`
        IFS=":"
        if [ "$name" = "vendor_id" ]; then
            vendor_id="$value"
        elif [ "$name" = "cpu family" ]; then
            cpu_family="$value"
        elif [ "$name" = "model" ]; then
            cpu_model="$value"
        elif [ "$name" = "flags" ]; then
            flags="$value"
            break #flags last so break early
        fi
    done < /proc/cpuinfo
    unset IFS
}

read_cpu_data_freebsd() {
    local _line

    if [ ! -x /usr/local/bin/cpuid ]; then
        echo "Please install misc/cpuid and run again"
        exit 1;
    fi
    if [ ! -r /var/run/dmesg.boot ]; then
        echo "/var/run/dmesg.boot does not exist!"
        exit 1;
    fi

    # Get the vendor_id, cpu_family and cpu_model from cpuid
    IFS="
"
    for _line in `/usr/local/bin/cpuid`; do
        if [ -n "`echo $_line | grep '^Vendor ID: '`" ]; then
            vendor_id="`echo $_line | cut -d'"' -f2`"
        fi
        if [ -n "`echo $_line | grep '^Family: '`" ]; then
            cpu_family="`echo $_line | sed -e 's/Family: //' -e 's/ .*//'`"
        fi
        if [ -n "`echo $_line | grep '^Family: '`" ]; then
            cpu_model="`echo $_line | sed -e 's/Family:.*Model: //' -e 's/ .*//'`"
        fi
    done
    unset IFS

    # Get the flags from /var/run/dmesg.boot
    flags="`grep -A2 '^CPU:' /var/run/dmesg.boot | grep '^  Features' | sed -e 's/^.*<//' -e 's/>.*//' -e 's/,/ /g' | tr 'A-Z' 'a-z'`"
}

read_cpu_data() {
    # Default values
    vendor_id="Unset"
    cpu_family="-1"
    cpu_model="-1"
    flags=""
    if [ "`uname`" = "Linux" ]; then
        read_cpu_data_linux
    elif [ "`uname`" = "FreeBSD" ]; then
        read_cpu_data_freebsd
    else
        echo "Error: `uname` is not a supported operating system"
        exit 1
    fi
}

read_cpu_data

if [ "$vendor_id" = "AuthenticAMD" ]; then
    if [ $cpu_family -eq 4 ]; then
        _CFLAGS="-march=i486"
    elif [ $cpu_family -eq 5 ]; then
        if [ $cpu_model -lt 4 ]; then
            _CFLAGS="-march=pentium"
        elif [ \( $cpu_model -eq 6 \) -o \( $cpu_model -eq 7 \) ]; then
            _CFLAGS="-march=k6"
        elif [ \( $cpu_model -eq 8 \) -o \( $cpu_model -eq 12 \) ]; then
            line="k6-2 k6"
        elif [ \( $cpu_model -eq 9 \) -o \( $cpu_model -eq 13 \) ]; then
            line="k6-3 k6-2 k6"
        fi
    elif [ $cpu_family -eq 6 ]; then
        if [ $cpu_model -le 3 ]; then
            line="athlon k6-3 k6-2 k6"
        elif [ $cpu_model -eq 4 ]; then
            line="athlon-tbird athlon k6-3 k6-2 k6"
        elif [ $cpu_model -ge 6 ]; then #athlon-{4,xp,mp}
            line="athlon-4 athlon k6-3 k6-2 k6"
        fi
    elif [ $cpu_family -eq 15 ]; then #k8,opteron,athlon64,athlon-fx
        line="k8 athlon-4 athlon k6-3 k6-2 k6"
    fi
elif [ "$vendor_id" = "CentaurHauls" ]; then
    if [ $cpu_family -eq 5 ]; then
        if [ $cpu_model -eq 4 ]; then
            line="winchip-c6 pentium"
        elif [ $cpu_model -eq 8 ]; then
            line="winchip2 winchip-c6 pentium"
        elif [ $cpu_model -ge 9 ]; then
            line="winchip2 winchip-c6 pentium" #actually winchip3 but gcc doesn't support this currently
        fi
    elif [ $cpu_family -eq 6 ]; then
        if echo "$flags" | grep -q cmov; then
            fallback=pentiumpro
        else
            fallback=pentium #gcc incorrectly assumes i686 always has cmov
        fi
        if [ $cpu_model -eq 6 ]; then
            _CFLAGS="-march=pentium" # ? Cyrix 3 (samuel)
        elif [ $cpu_model -eq 7 ] || [ $cpu_model -eq 8 ]; then
            line="c3 winchip2 winchip-c6 $fallback"
        elif [ $cpu_model -ge 9 ]; then
            line="c3-2 c3 winchip2 winchip-c6 $fallback"
        fi
    fi
elif [ "$vendor_id" = "GenuineIntel" ]; then
    if [ $cpu_family -eq 3 ]; then
        _CFLAGS="-march=i386"
    elif [ $cpu_family -eq 4 ]; then
        _CFLAGS="-march=i486"
    elif [ $cpu_family -eq 5 ]; then
        if [ $cpu_model -ne 4 ]; then
            _CFLAGS="-march=pentium"
        else
            line="pentium-mmx pentium" #No overlap with other vendors
        fi
    elif [ $cpu_family -eq 6 ]; then
        if [ \( $cpu_model -eq 0 \) -o \( $cpu_model -eq 1 \) ]; then
            _CFLAGS="-march=pentiumpro"
        elif [ \( $cpu_model -ge 3 \) -a \( $cpu_model -le 6 \) ]; then #4=TM5600 at least
            line="pentium2 pentiumpro pentium-mmx pentium i486 i386"
        elif [ \( $cpu_model -eq 9 \) -o \( $cpu_model -eq 13 \) ]; then #centrino
            line="pentium-m pentium4 pentium3 pentium2 pentiumpro pentium-mmx pentium i486 i386"
        elif [ $cpu_model -eq 14 ]; then #Core
            line="prescott pentium-m pentium4 pentium3 pentium2 pentiumpro pentium-mmx pentium i486 i386"
        elif [ $cpu_model -eq 15 ]; then #Core 2
            line="core2 pentium-m pentium4 pentium3 pentium2 pentiumpro pentium-mmx pentium i486 i386"
        elif [ \( $cpu_model -ge 7 \) -a \( $cpu_model -le 11 \) ]; then
            line="pentium3 pentium2 pentiumpro pentium-mmx pentium i486 i386"
        fi
    elif [ $cpu_family -eq 15 ]; then
        if [ $cpu_model -eq 3 ]; then
            line="prescott pentium4 pentium3 pentium2 pentiumpro pentium-mmx pentium i486 i386"
        else
            line="pentium4 pentium3 pentium2 pentiumpro pentium-mmx pentium i486 i386"
        fi
    fi
else
    echo "Unknown CPU Vendor: $vendor_id"
    exit 1
fi

[ -z "$_CFLAGS" ] && _CFLAGS="-march=`try_line "native $line"`"

if echo "$flags" | grep -q "sse"; then
    if try_gcc_options "-mfpmath=sse"; then #gcc >= 3.1
        _CFLAGS="$_CFLAGS -mfpmath=sse"
    fi
fi
if echo "$flags" | grep -E -q "(sse3|pni)"; then
    if ! echo "$_CFLAGS" | grep -E -q "(native|core2)"; then
        if try_gcc_options "-msse3"; then #gcc >= 3.3.3
            _CFLAGS="$_CFLAGS -msse3"
        fi
    fi
fi

echo "$m32$_CFLAGS"
