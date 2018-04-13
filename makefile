#
# Version $Revision: 1.68 $
#
# The makefile for building all versions of iozone for all supported
# platforms
#
# Supports:	hpux, hpux_no_ansi, hpux-10.1, hpux_no_ansi-10.1,
#		sppux, sppux-10.1, ghpux, sppux, 
#		convex, FreeBSD, OpenBSD, OSFV3, OSFV4, OSFV5, SCO
#		SCO_Unixware_gcc,NetBSD,TRU64, Mac OS X


all:  
	@echo ""
	@echo "You must specify the target.        "
	@echo "        ->   AIX                  (32bit)   <-"
	@echo "        ->   AIX-LF               (32bit)   <-"
	@echo "        ->   bsdi                 (32bit)   <-" 
	@echo "        ->   convex               (32bit)   <-" 
	@echo "        ->   freebsd              (32bit)   <-"
	@echo "        ->   generic              (32bit)   <-"
	@echo "        ->   ghpux                (32bit)   <-"
	@echo "        ->   hpuxs-11.0 (simple)  (32bit)   <-"
	@echo "        ->   hpux-11.0w           (64bit)   <-"
	@echo "        ->   hpuxs-11.0w          (64bit)   <-"
	@echo "        ->   hpux-11.0            (32bit)   <-"
	@echo "        ->   hpux-10.1            (32bit)   <-"
	@echo "        ->   hpux-10.20           (32bit)   <-"
	@echo "        ->   hpux                 (32bit)   <-" 
	@echo "        ->   hpux_no_ansi         (32bit)   <-"
	@echo "        ->   hpux_no_ansi-10.1    (32bit)   <-"
	@echo "        ->   IRIX                 (32bit)   <-"
	@echo "        ->   IRIX64               (64bit)   <-"
	@echo "        ->   linux                (32bit)   <-"
	@echo "        ->   linux-ia64           (32bit)   <-"
	@echo "        ->   linux-powerpc        (32bit)   <-"
	@echo "        ->   linux-sparc          (32bit)   <-"
	@echo "        ->   macosx               (32bit)   <-"
	@echo "        ->   netbsd               (32bit)   <-"
	@echo "        ->   openbsd              (32bit)   <-"
	@echo "        ->   openbsd-threads      (32bit)   <-"
	@echo "        ->   OSFV3                (64bit)   <-"
	@echo "        ->   OSFV4                (64bit)   <-"
	@echo "        ->   OSFV5                (64bit)   <-"
	@echo "        ->   SCO                  (32bit)   <-"
	@echo "        ->   SCO_Unixware_gcc     (32bit)   <-"
	@echo "        ->   Solaris              (32bit)   <-"
	@echo "        ->   Solaris-2.6          (32bit)   <-"
	@echo "        ->   Solaris7gcc          (32bit)   <-"
	@echo "        ->   Solaris8-64          (64bit)   <-"
	@echo "        ->   sppux                (32bit)   <-"
	@echo "        ->   sppux-10.1           (32bit)   <-"
	@echo "        ->   sppux_no_ansi-10.1   (32bit)   <-"
	@echo "        ->   TRU64                (64bit)   <-"
	@echo "        ->   UWIN                 (32bit)   <-"
	@echo "        ->   Windows (95/98/NT)   (32bit)   <-"
	@echo ""

clean:
	rm -f *.o iozone

#
# Turn on the optimizer, largefiles, Posix async I/O and threads.
#
hpux-11.0:	iozone_hpux-11.0.o libasync.o libbif.o
	 cc +O3 +Oparallel -D_LARGEFILE64_SOURCE -Dunix -D_HPUX_SOURCE -DASYNC_IO \
	-DHAVE_ANSIC_C -DVXFS iozone_hpux-11.0.o libasync.o \
	libbif.o -lpthread -lrt -o iozone

#
# Turn on wide-mode, the optimizer, largefiles, Posix async I/O and threads.
#
hpux-11.0w:	iozone_hpux-11.0w.o libasyncw.o libbif.o
	 cc +DD64 +O3 -D_LARGEFILE64_SOURCE -Dunix -D_HPUX_SOURCE -DASYNC_IO \
	-DHAVE_ANSIC_C -DVXFS iozone_hpux-11.0w.o libasyncw.o \
	libbif.o -lpthread -lrt -o iozone


#
# Simple build with largefiles, Posix threads and Posix async I/O
#
hpuxs-11.0:	iozone_hpuxs-11.0.o libasync.o libbif.o
	 cc -O -D_LARGEFILE64_SOURCE -Dunix -D_HPUX_SOURCE -DASYNC_IO \
	-DHAVE_ANSIC_C -DVXFS iozone_hpuxs-11.0.o \
	libasync.o libbif.o -lpthread -lrt -o iozone

#
# Simple build with wide-mode, largefiles, Posix threads and Posix async I/O
#
hpuxs-11.0w:	iozone_hpuxs-11.0w.o libasyncw.o libbif.o
	 cc -O +DD64 -D_LARGEFILE64_SOURCE -Dunix -D_HPUX_SOURCE -DASYNC_IO \
	-DHAVE_ANSIC_C -DVXFS \
	iozone_hpuxs-11.0w.o libasyncw.o libbif.o -lpthread -lrt -o iozone

#
# Simple 10.1 build with no threads, no largefiles, no async I/O 
#
hpux-10.1:	iozone_hpux-10.1.o  libbif.o
	 c89 +e -O -Dunix -D_HPUX_SOURCE \
	-DNO_THREADS -DHAVE_ANSIC_C iozone_hpux-10.1.o \
	libbif.o -o iozone

hpux-10.20:	iozone_hpux-10.20.o  libbif.o
	 c89 +e -O -Dunix -D_HPUX_SOURCE \
	-DNO_THREADS -DHAVE_ANSIC_C iozone_hpux-10.20.o \
	libbif.o -o iozone

#
# Simple generic HP build with no threads, no largefiles, no async I/O 
#
hpux:	iozone_hpux.o 
	c89 +e -O -Dunix -D_HPUX_SOURCE \
	-DNO_THREADS -DHAVE_ANSIC_C iozone_hpux.o \
	libbif.o -o iozone

#
# GNU HP build with no threads, no largefiles, no async I/O 
#
ghpux:	iozone_ghpux.o  libbif.o
	gcc  -O -Dunix -D_HPUX_SOURCE -DHAVE_ANSIC_C iozone_ghpux.o \
	libbif.o -DNO_THREADS -static -o iozone

#
# GNU Generic build with no threads, no largefiles, no async I/O 
#
generic:	iozone_generic.o  libbif.o
	$(CC)  -O -Dgeneric -Dunix -DHAVE_ANSIC_C iozone_generic.o \
		libbif.o -DNO_THREADS -o iozone

#
# No ansii 'C' compiler HP build with no threads, no largefiles, no async I/O 
#
hpux_no_ansi-10.1:	iozone_hpux_no-10.1.o  libbif.o 
	/opt/ansic/bin/cc  -O -Dunix -D_HPUX_SOURCE iozone_hpux_no-10.1.o \
	libbif.o -DNO_THREADS -o iozone

#
# No ansii 'C' compiler HP build with no threads, no largefiles, no async I/O 
#
hpux_no_ansi:	iozone_hpux_no.o  libbif.o
	c89  -O -Dunix -D_HPUX_SOURCE iozone_hpux_no.o \
		-DNO_THREADS libbif.o -o iozone

#
# GNU 'C' compiler Linux build with threads, no largefiles, no async I/O 
#
linux:	iozone_linux.o  libbif.o libasync.o
	cc  -O3 -Dunix -DHAVE_ANSIC_C -DSHARED_MEM -DASYNC_IO \
		-D_LARGEFILE64_SOURCE -Dlinux \
		iozone_linux.o libasync.o libbif.o -lpthread \
		-lrt -o iozone

#
# GNU 'C' compiler Linux build for powerpc chip with threads, no largefiles, no async I/O 
#
linux-powerpc: iozone_linux-powerpc.o  libbif.o libasync.o
	cc  -O3 -Dunix -DHAVE_ANSIC_C -DSHARED_MEM -DASYNC_IO \
		-D_LARGEFILE64_SOURCE -Dlinux \
		iozone_linux-powerpc.o libasync.o libbif.o -lpthread \
		-lrt -o iozone
#
# GNU 'C' compiler Linux build for sparc chip with threads, no largefiles, no async I/O 
#
linux-sparc: iozone_linux-sparc.o  libbif.o libasync.o
	cc  -O3 -Dunix -DHAVE_ANSIC_C -DSHARED_MEM -DASYNC_IO \
		-D_LARGEFILE64_SOURCE -Dlinux \
		iozone_linux-sparc.o libasync.o libbif.o -lpthread \
		-lrt -o iozone

#
# GNU 'C' compiler Linux build with no threads, no largefiles, no async I/O 
#
linux-ia64:	iozone_linux-ia64.o  libbif.o
	cc  -O3 -Dunix -DHAVE_ANSIC_C -DSHARED_MEM \
		-DNO_THREADS -D_LARGEFILE64_SOURCE -Dlinux \
		iozone_linux-ia64.o libbif.o \
		-o iozone
# 
# AIX
# I would have built with ASYNC_IO but the AIX machine does not have 
# POSIX 1003.1b compliant async I/O header files.  Has threads, no
# largefile support.
# 
AIX:	iozone_AIX.o  libbif.o  
	cc  -O -D__AIX__ -D_NO_PROTO -Dunix -DHAVE_ANSIC_C \
		-DSHARED_MEM iozone_AIX.o libbif.o \
		-lpthreads -o iozone

# 
# AIX-LF
# I would have built with ASYNC_IO but the AIX machine does not have 
# POSIX 1003.1b compliant async I/O header files.  Has threads, and
# largefile support.
# 
AIX-LF:	iozone_AIX-LF.o  libbif.o  
	cc  -O -D__AIX__ -D_NO_PROTO -Dunix -DHAVE_ANSIC_C \
		-DSHARED_MEM  -D_LARGEFILE64_SOURCE -D_LARGE_FILES \
		iozone_AIX-LF.o libbif.o \
		-lpthreads -o iozone

#
# IRIX 32 bit build with threads, largefiles, async I/O 
# This would like to be in 64 bit mode but it hangs whenever in 64 bit mode.
# This version uses the 64 bit interfaces but is compiled as 32 bit code
#
IRIX64:	iozone_IRIX64.o libasyncw.o libbif.o 
	cc   -32 -O -Dunix -DHAVE_ANSIC_C -D_LARGEFILE64_SOURCE -DSHARED_MEM \
		-DASYNC_IO -DIRIX64 iozone_IRIX64.o libbif.o \
		-lpthread libasyncw.o -o iozone

#
# IRIX 32 bit build with threads, No largefiles, and async I/O 
# This version uses the 32 bit interfaces and is compiled as 32 bit code
#
IRIX:	iozone_IRIX.o libasync.o libbif.o
	cc  -O  -32  -Dunix -DHAVE_ANSIC_C -DSHARED_MEM -DASYNC_IO \
		-DIRIX iozone_IRIX.o libbif.o -lpthread \
		libasync.o -o iozone

#
# SPP-UX 32 bit build with threads, No largefiles, and No async I/O, pread extensions
# For older SPP-UX machines with 9.05 compatibility
#
sppux:	iozone_sppux.o  libbif.o
	/opt/ansic/bin/cc  -O -Dunix -D_HPUX_SOURCE -D__convex_spp \
	-DHAVE_ANSIC_C -DHAVE_PREAD -DHAVE_PREADV iozone_sppux.o  libbif.o \
	-Wl,+parallel -lcnx_syscall -lpthread -lail -o iozone

#
# SPP-UX 32 bit build with threads, No largefiles, and No async I/O, pread extensions
# For Newer SPP-UX machines with 10.01 compatibility
#
sppux-10.1:	iozone_sppux-10.1.o libbif.o
	/opt/ansic/bin/cc -O -Dunix -D_HPUX_SOURCE -D__convex_spp \
	-DHAVE_ANSIC_C -DHAVE_PREAD -DHAVE_PREADV iozone_sppux-10.1.o libbif.o \
	 -lcnx_syscall  -Wl,+parallel -lpthread -lail -o iozone

#
# SPP-UX 32 bit build with threads, No largefiles, and No async I/O, pread extensions
# For Newer SPP-UX machines with 10.01 compatibility, and no ansi 'C' compiler.
#
sppux_no_ansi-10.1:	iozone_sppux_no-10.1.o libbif.o
	/usr/ccs/bin/cc  -O -Dunix -D_HPUX_SOURCE -DHAVE_PREAD -DHAVE_PREADV -D__convex_spp \
		iozone_sppux_no-10.1.o libbif.o \
		-Wl,+parallel -lcnx_syscall  \
		-lpthread -lail -o iozone

#
# Convex 'C' series 32 bit build with No threads, No largefiles, and No async I/O
#
convex:	iozone_convex.o libbif.o
	cc -O -Dunix -Dbsd4_2 -DNO_THREADS iozone_convex.o \
		libbif.o -o iozone

#
# Solaris 32 bit build with threads, largefiles, and async I/O
#
Solaris:	iozone_solaris.o libasync.o libbif.o 
	cc  -O -Dunix -DHAVE_ANSIC_C -DASYNC_IO \
		-Dsolaris iozone_solaris.o libasync.o libbif.o \
		-D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -lthread \
                -lpthread -lposix4 -lnsl -laio -lsocket \
		-o iozone

#
# Solaris 32 bit build with threads, largefiles, and async I/O
#
Solaris7gcc:	iozone_solaris7gcc.o libasync7.o libbif7.o 
	gcc  -O -Dunix -DHAVE_ANSIC_C -DASYNC_IO \
		-Dsolaris iozone_solaris7gcc.o libasync7.o libbif7.o \
		-D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -lthread \
		-lpthread -lposix4 -lnsl -laio \
		-lsocket -o iozone

#
# Solaris 2.6 (32 bit) build with no threads, no largefiles, and no async I/O
#
Solaris-2.6:	iozone_solaris-2.6.o libbif.o 
	cc  -O -Dunix -DHAVE_ANSIC_C -Dsolaris iozone_solaris-2.6.o libbif.o \
		-lnsl -laio -l socket -o iozone

#
# Solaris 64 bit build with threads, largefiles, and async I/O
#
Solaris8-64: iozone_solaris8-64.o libasync.o libbif.o
	cc -fast -xtarget=generic64 -v -Dunix -DHAVE_ANSIC_C -DASYNC_IO \
		-Dsolaris iozone_solaris8-64.o libasync.o libbif.o \
		-D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -lthread \
		-lpthread -lposix4 -lnsl -laio \
		-lsocket -o iozone

#
# Windows build requires Cygnus development environment. You
# can get this from www.cygnus.com
# No threads, No largefiles, No async I/O
#
Windows:	iozone_windows.o libbif.o
	gcc  -O -Dunix -DHAVE_ANSIC_C -DNO_THREADS \
		-DWindows iozone_windows.o \
		libbif.o -o iozone

#
# Uwin build requires UWIN development environment. 
# No threads, No largefiles, No async I/O
#
UWIN:	iozone_uwin.o libbif.o
	gcc  -O -DUWIN -Dunix -DHAVE_ANSIC_C -DNO_THREADS \
		-DSHARED_MEM -DWindows iozone_uwin.o \
		libbif.o -o iozone

#
# GNU C compiler BSD/OS build with threads, largefiles, no async I/O
#

bsdi:	iozone_bsdi.o libbif.o
	cc -O -Dunix -Dbsd4_4 -DHAVE_ANSIC_C  \
		iozone_bsdi.o libbif.o -o iozone

#
# GNU C compiler FreeBSD build with no threads, no largefiles, no async I/O
#

freebsd:	iozone_freebsd.o libbif.o
	cc ${CFLAGS} -Dunix -DHAVE_ANSIC_C -DNO_THREADS -DSHARED_MEM \
		iozone_freebsd.o libbif.o -o iozone

#
# GNU C compiler MacosX build with no threads, no largefiles, no async I/O
#

macosx:	iozone_macosx.o libbif.o
	cc -O -Dunix -DHAVE_ANSIC_C -DNO_THREADS -DSHARED_MEM \
		iozone_macosx.o libbif.o -o iozone
#
#
# GNU C compiler OpenBSD build with no threads, no largefiles, no async I/O
#

openbsd:	iozone_openbsd.o libbif.o
	cc -O -Dunix -Dbsd4_4 -DHAVE_ANSIC_C -DNO_THREADS -DSHARED_MEM \
		iozone_openbsd.o libbif.o -o iozone

#
# GNU C compiler OpenBSD build with threads, no largefiles, no async I/O
#

openbsd-threads:	iozone_openbsd-threads.o libbif.o
	cc -O -pthread -Dunix -Dbsd4_4 -DHAVE_ANSIC_C \
		iozone_openbsd-threads.o \
		libbif.o -o iozone

#
# GNU C compiler OSFV3 build 
# Has threads and async I/O but no largefiles.
#

OSFV3:	iozone_OSFV3.o libbif.o libasync.o
	cc -O -Dunix -DHAVE_ANSIC_C -DASYNC_IO \
		-DNO_PRINT_LLD -DOSF_64 -DOSFV3 iozone_OSFV3.o libbif.o \
		-lpthreads libasync.o -laio -o iozone

#
# GNU C compiler OSFV4 build 
# Has threads and async I/O but no largefiles.
#

OSFV4:	iozone_OSFV4.o libbif.o libasync.o
	cc -O -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV4 \
		-DNO_PRINT_LLD -DOSF_64 iozone_OSFV4.o libbif.o -lpthread \
		libasync.o -laio -o iozone

#
# GNU C compiler OSFV5 build 
# Has threads and async I/O but no largefiles.
#

OSFV5:	iozone_OSFV5.o libbif.o libasync.o
	cc -O -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV5 \
		-DNO_PRINT_LLD -DOSF_64 iozone_OSFV5.o libbif.o -lpthread \
		libasync.o -laio -o iozone

#
# GNU C compiler TRU64 build 
# Has threads and async I/O but no largefiles.
#

TRU64:	iozone_TRU64.o libbif.o libasync.o
	cc -O -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV5 -DTRU64 \
		-DNO_PRINT_LLD -DOSF_64 iozone_TRU64.o libbif.o -lpthread \
		libasync.o -laio -o iozone

#
# GNU Generic build with no threads, no largefiles, no async I/O
# for SCO
# Note:	Be sure you have the latest patches for SCO's Openserver
# or you will get warnings about timer problems.
#

SCO:	iozone_SCO.o  libbif.o
	gcc -O -DSCO -Dunix -DHAVE_ANSIC_C iozone_SCO.o \
		libbif.o -DNO_THREADS -o iozone


#
# GNU build with threads, largefiles, async I/O
# for SCO Unixware 5 7.1.1 i386 x86at SCO UNIX SVR5
# Note: Be sure you have the latest patches for SCO's Openserver
# or you will get warnings about timer problems.
#

SCO_Unixware_gcc:	iozone_SCO_Unixware_gcc.o  libbif.o libasync.o
	/usr/local/bin/gcc -O -DSCO_Unixware_gcc -Dunix -DHAVE_ANSIC_C \
		-DASYNC_IO -D_LARGEFILE64_SOURCE iozone_SCO_Unixware_gcc.o \
		libbif.o libasync.o -lthread -o iozone

#
# GNU C compiler NetBSD build with no threads, no largefiles, no async I/O
#

netbsd:	iozone_netbsd.o  libbif.o
	cc -O -Dunix -Dbsd4_4 -DHAVE_ANSIC_C -DNO_THREADS -DSHARED_MEM \
		iozone_netbsd.o libbif.o -o iozone

#
#
# Now for the machine specific stuff
#

iozone_hpux.o:	iozone.c libbif.c
	@echo ""
	@echo "Building iozone for HP-UX (9.05)"
	@echo ""
	c89 +e -c -O -Dunix -D_HPUX_SOURCE -DHAVE_ANSIC_C -DNO_THREADS \
		-DNAME='"hpux"' iozone.c  -o iozone_hpux.o
	c89 +e -c -O -Dunix -D_HPUX_SOURCE -DHAVE_ANSIC_C -DNO_THREADS \
		-DBIG_ENDIAN libbif.c  -o libbif.o

iozone_hpux-11.0.o:	iozone.c libasync.c libbif.c
	@echo ""
	@echo "Building iozone for HP-UX (11.0)"
	@echo ""
	cc -c  +O3 +Oparallel -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE \
		-DNAME='"hpux-11.0"' -DHAVE_ANSIC_C -DASYNC_IO -DVXFS iozone.c  \
		-o iozone_hpux-11.0.o
	cc -c  +O3 +Oparallel -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE \
		-DHAVE_ANSIC_C -DASYNC_IO -DVXFS libasync.c  -o libasync.o
	cc -c  +O3 +Oparallel -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE \
		-DHAVE_ANSIC_C -DASYNC_IO -DVXFS -DBIG_ENDIAN libbif.c  -o libbif.o

iozone_hpux-11.0w.o:	iozone.c libasync.c libbif.c
	@echo ""
	@echo "Building iozone for HP-UX (11.0w)"
	@echo ""
	cc -c +DD64 +O3 -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE \
	    -DNAME='"hpux-11.0w"' -DHAVE_ANSIC_C -DASYNC_IO -DVXFS iozone.c \
	     -o iozone_hpux-11.0w.o
	cc -c +DD64 +O3 -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE \
		-DHAVE_ANSIC_C -DASYNC_IO -DVXFS libasync.c  -o libasyncw.o
	cc -c +DD64 +O3 -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE \
		-DHAVE_ANSIC_C -DASYNC_IO -DVXFS -DBIG_ENDIAN libbif.c  -o libbif.o

iozone_hpuxs-11.0.o:	iozone.c libasync.c libbif.c
	@echo ""
	@echo "Building simple iozone for HP-UX (11.0)"
	@echo ""
	cc -c  -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE -DHAVE_ANSIC_C \
		-DNAME='"hpuxs-11.0"' -DASYNC_IO -DVXFS -DHAVE_PREAD iozone.c  \
		-o iozone_hpuxs-11.0.o
	cc -c  -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE -DHAVE_ANSIC_C \
		-DASYNC_IO  -DVXFS libasync.c  -o libasync.o 
	cc -c  -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE -DHAVE_ANSIC_C \
		-DASYNC_IO -DVXFS -DBIG_ENDIAN libbif.c  -o libbif.o 

iozone_hpuxs-11.0w.o:	iozone.c libasync.c libbif.c
	@echo ""
	@echo "Building simple iozone for HP-UX (11.0w)"
	@echo ""
	cc -c +DD64 -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE \
		-DNAME='"hpuxs-11.0w"' -DHAVE_ANSIC_C -DASYNC_IO -DVXFS \
		-DHAVE_PREAD iozone.c  -o iozone_hpuxs-11.0w.o
	cc -c +DD64 -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE \
		-DHAVE_ANSIC_C -DASYNC_IO -DVXFS libasync.c  -o libasyncw.o 
	cc -c +DD64 -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE \
		-DHAVE_ANSIC_C -DASYNC_IO -DVXFS -DBIG_ENDIAN libbif.c  -o libbif.o 

iozone_hpux-10.1.o:	iozone.c libbif.c
	@echo ""
	@echo "Building iozone for HP-UX (10.1)"
	@echo ""
	c89 +e -c -O -Dunix -D_HPUX_SOURCE -DHAVE_ANSIC_C -DNO_THREADS \
		-DNAME='"hpux-10.1"' iozone.c  -o iozone_hpux-10.1.o
	c89 +e -c -O -Dunix -D_HPUX_SOURCE -DHAVE_ANSIC_C -DNO_THREADS \
		-DBIG_ENDIAN libbif.c  -o libbif.o

iozone_hpux-10.20.o:	iozone.c libbif.c
	@echo ""
	@echo "Building iozone for HP-UX (10.20)"
	@echo ""
	c89 +e -c -O -Dunix -D_HPUX_SOURCE -DHAVE_ANSIC_C -DNO_THREADS \
		-DNAME='"hpux-10.20"' iozone.c  -o iozone_hpux-10.20.o
	c89 +e -c -O -Dunix -D_HPUX_SOURCE -DHAVE_ANSIC_C -DNO_THREADS \
		-DBIG_ENDIAN libbif.c  -o libbif.o

iozone_ghpux.o:	iozone.c libbif.c
	@echo ""
	@echo "Building iozone for GCC HP-UX (9.05) "
	@echo ""
	gcc -c -O -Dunix -D_HPUX_SOURCE -DHAVE_ANSIC_C -DNO_THREADS iozone.c \
		-DNAME='"h=ghpux"' -o iozone_ghpux.o
	gcc -c -O -Dunix -D_HPUX_SOURCE -DHAVE_ANSIC_C -DNO_THREADS -DBIG_ENDIAN \
		libbif.c -o libbif.o

iozone_generic.o:	iozone.c libbif.c
	@echo ""
	@echo "Building iozone Generic "
	@echo ""
	$(CC) -c -O -Dgeneric -Dunix -DHAVE_ANSIC_C -DNO_THREADS \
		-DNAME='"Generic"' iozone.c -o iozone_generic.o
	$(CC) -c -O -Dgeneric -Dunix -DHAVE_ANSIC_C -DNO_THREADS \
		-DBIG_ENDIAN libbif.c -o libbif.o

iozone_hpux_no.o:	iozone.c libbif.c
	@echo ""
	@echo "Building iozone for HP-UX (9.05) without ansi compiler"
	@echo ""
	/opt/ansic/bin/cc -c -O -Dunix -D_HPUX_SOURCE -DNO_THREADS iozone.c \
		-DNAME='"hpux_no_ansi"' -o iozone_hpux_no.o
	/opt/ansic/bin/cc -c -O -Dunix -D_HPUX_SOURCE -DNO_THREADS \
		-DBIG_ENDIAN libbif.c -o libbif.o

iozone_hpux_no-10.1.o:	iozone.c
	@echo ""
	@echo "Building iozone for HP-UX (10.1) without ansi compiler"
	@echo ""
	/opt/ansic/bin/cc -c -O -Dunix -D_HPUX_SOURCE -DNO_THREADS iozone.c \
		-DNAME='"hpux_no_ansi_10.1"' -o iozone_hpux_no-10.1.o
	/opt/ansic/bin/cc -c -O -Dunix -D_HPUX_SOURCE -DNO_THREADS \
		-DBIG_ENDIAN libbif.c -o libbif.o

iozone_linux-powerpc.o:	iozone.c libbif.c libasync.c
	@echo ""
	@echo "Building iozone for Linux PowerPC"
	@echo ""
	cc -c -O3 -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DDONT_HAVE_O_DIRECT \
		-DSHARED_MEM -Dlinux -D_LARGEFILE64_SOURCE iozone.c \
		-DNAME='"linux-powerpc"' -o iozone_linux-powerpc.o
	cc -c -O3 -Dunix -DHAVE_ANSIC_C -DASYNC_IO -D_LARGEFILE64_SOURCE \
		-DSHARED_MEM -Dlinux libbif.c -o libbif.o
	cc -c -O3 -Dunix -Dlinux -DHAVE_ANSIC_C -DASYNC_IO \
		-D_LARGEFILE64_SOURCE libasync.c  -o libasync.o 

iozone_linux-sparc.o:	iozone.c libbif.c libasync.c
	@echo ""
	@echo "Building iozone for Linux Sparc"
	@echo ""
	cc -c -O3 -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DDONT_HAVE_O_DIRECT \
		-DSHARED_MEM -Dlinux -D_LARGEFILE64_SOURCE iozone.c \
		-DNAME='"linux-sparc"' -o iozone_linux-sparc.o
	cc -c -O3 -Dunix -DHAVE_ANSIC_C -DASYNC_IO -D_LARGEFILE64_SOURCE \
		-DSHARED_MEM -Dlinux libbif.c -o libbif.o
	cc -c -O3 -Dunix -Dlinux -DHAVE_ANSIC_C -DASYNC_IO \
		-D_LARGEFILE64_SOURCE libasync.c  -o libasync.o 

iozone_linux.o:	iozone.c libbif.c libasync.c
	@echo ""
	@echo "Building iozone for Linux"
	@echo ""
	cc -c -O3 -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DHAVE_PREAD \
		-DSHARED_MEM -Dlinux -D_LARGEFILE64_SOURCE iozone.c \
		-DNAME='"linux"' -o iozone_linux.o
	cc -c -O3 -Dunix -DHAVE_ANSIC_C -DASYNC_IO -D_LARGEFILE64_SOURCE \
		-DSHARED_MEM -Dlinux libbif.c -o libbif.o
	cc -c -O3 -Dunix -Dlinux -DHAVE_ANSIC_C -DASYNC_IO \
		-D_LARGEFILE64_SOURCE libasync.c  -o libasync.o 

iozone_linux-ia64.o:	iozone.c libbif.c
	@echo ""
	@echo "Building iozone for Linux-ia64"
	@echo ""
	cc -c -O3 -Dunix -DHAVE_ANSIC_C -DNO_THREADS -DNAME='"linux-ia64"' \
		-DSHARED_MEM -Dlinux -D_LARGEFILE64_SOURCE iozone.c \
		-o iozone_linux-ia64.o
	cc -c -O3 -Dunix -DHAVE_ANSIC_C -D_LARGEFILE64_SOURCE \
		-DSHARED_MEM -Dlinux libbif.c -o libbif.o

iozone_AIX.o:	iozone.c libbif.c 
	@echo ""
	@echo "Building iozone for AIX"
	@echo ""
	cc -c -O -D__AIX__ -D_NO_PROTO -Dunix -DHAVE_ANSIC_C  \
		-DNAME='"AIX"' -DSHARED_MEM  iozone.c -o iozone_AIX.o
	cc -c -O -D__AIX__ -D_NO_PROTO -Dunix -DHAVE_ANSIC_C  \
		-DSHARED_MEM libbif.c -o libbif.o

iozone_AIX-LF.o:	iozone.c libbif.c 
	@echo ""
	@echo "Building iozone for AIX with Large files"
	@echo ""
	cc -c -O -D__AIX__ -D_NO_PROTO -Dunix -DHAVE_ANSIC_C  \
		-DSHARED_MEM  -D_LARGEFILE64_SOURCE -D_LARGE_FILES \
		-DNAME='"AIX-LF"' iozone.c -o iozone_AIX-LF.o
	cc -c -O -D__AIX__ -D_NO_PROTO -Dunix -DHAVE_ANSIC_C  \
		-DSHARED_MEM -D_LARGEFILE64_SOURCE -D_LARGE_FILES \
		libbif.c -o libbif.o

iozone_solaris.o:	iozone.c libasync.c libbif.c
	@echo ""
	@echo "Building iozone for Solaris"
	@echo ""
	cc -c -O -Dunix -DHAVE_ANSIC_C -DASYNC_IO \
		-D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -Dsolaris \
		-DNAME='"Solaris"' iozone.c -o iozone_solaris.o
	cc -O -c  -Dunix -DHAVE_ANSIC_C -DASYNC_IO -D__LP64__ \
		-D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -Dsolaris \
		libasync.c -o libasync.o
	cc -O -c  -Dunix -DHAVE_ANSIC_C -DASYNC_IO -D__LP64__ \
		-D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -Dsolaris \
		-DBIG_ENDIAN libbif.c -o libbif.o

iozone_solaris7gcc.o:	iozone.c libasync.c libbif.c
	@echo ""
	@echo "Building iozone for Solaris7gcc"
	@echo ""
	gcc -O -c  -Dunix -DHAVE_ANSIC_C -DASYNC_IO -D__LP64__ \
		-D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -Dsolaris \
		-DNAME='"Solaris7gcc"' libasync.c -o libasync7.o
	gcc -O -c  -Dunix -DHAVE_ANSIC_C -DASYNC_IO -D__LP64__ \
		-D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -Dsolaris \
		-DBIG_ENDIAN libbif.c -o libbif7.o
	gcc -c -O -Dunix -DHAVE_ANSIC_C -DASYNC_IO \
		-D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -Dsolaris \
		iozone.c -o iozone_solaris7gcc.o

#
#		-DSHARED_MEM -Dsolaris libasync.c -o libasync.o
#		-DSHARED_MEM -Dsolaris iozone.c -o iozone_solaris.o
#

iozone_solaris-2.6.o:	iozone.c libbif.c
	@echo ""
	@echo "Building iozone for Solaris-2.6"
	@echo ""
	cc -c -O -Dunix -DHAVE_ANSIC_C -DNO_THREADS \
		-DNAME='"Solaris-2.6"' -Dsolaris  iozone.c -o iozone_solaris-2.6.o
	cc -O -c  -Dunix -DHAVE_ANSIC_C \
		-Dsolaris -DBIG_ENDIAN libbif.c -o libbif.o

iozone_solaris8-64.o: iozone.c libasync.c libbif.c
	@echo ""
	@echo "Building iozone for Solaris8-64"
	@echo ""
	cc -fast -xtarget=generic64 -v -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO \
		-D__LP64__ -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 \
		-DNAME='"Solaris8-64"' -Dsolaris iozone.c -o iozone_solaris8-64.o
	cc -fast -xtarget=generic64 -v -c  -Dunix -DHAVE_ANSIC_C -DASYNC_IO \
		-D__LP64__ -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 \
		-Dsolaris libasync.c -o libasync.o
	cc -fast -xtarget=generic64 -v -c  -Dunix -DHAVE_ANSIC_C -DASYNC_IO \
		-D__LP64__ -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 \
		-Dsolaris -DBIG_ENDIAN libbif.c -o libbif.o

iozone_windows.o:	iozone.c libasync.c libbif.c
	@echo ""
	@echo "Building iozone for Windows (No threads, No async I/O)"
	@echo ""
	gcc -c -O -Dunix -DHAVE_ANSIC_C -DNO_THREADS  \
		-DNAME='"Windows"' -DWindows iozone.c -o iozone_windows.o
	gcc -c -O -Dunix -DHAVE_ANSIC_C -DNO_THREADS  \
		-DWindows libbif.c -o libbif.o

iozone_uwin.o:	iozone.c libbif.c
	@echo ""
	@echo "Building iozone for UWIN (No threads, No async I/O)"
	@echo ""
	gcc -c -O -DUWIN -Dunix -DHAVE_ANSIC_C -DNO_THREADS  \
		-DNAME='"UWIN"' -DSHARED_MEM -DWindows iozone.c -o iozone_uwin.o
	gcc -c -O -DUWIN -Dunix -DHAVE_ANSIC_C -DNO_THREADS  \
		-DSHARED_MEM -DWindows libbif.c -o libbif.o

iozone_IRIX64.o:	iozone.c libasync.c libbif.c
	@echo ""
	@echo "Building iozone for IRIX64"
	@echo ""
	cc -32 -O -c  -Dunix -DHAVE_ANSIC_C -D_LARGEFILE64_SOURCE -DASYNC_IO \
		-DNAME='"IRIX64"' -DIRIX64 -DSHARED_MEM iozone.c -o iozone_IRIX64.o
	cc -32 -O -c  -Dunix -DHAVE_ANSIC_C -D_LARGEFILE64_SOURCE -DASYNC_IO \
		-DIRIX64 -DSHARED_MEM libasync.c -o libasyncw.o
	cc -32 -O -c  -Dunix -DHAVE_ANSIC_C -D_LARGEFILE64_SOURCE -DASYNC_IO \
		-DIRIX64 -DSHARED_MEM -DBIG_ENDIAN libbif.c -o libbif.o

iozone_IRIX.o:	iozone.c libasync.c libbif.c
	@echo ""
	@echo "Building iozone for IRIX"
	@echo ""
	cc  -O -32 -c  -Dunix -DHAVE_ANSIC_C -DASYNC_IO \
		-DNAME='"IRIX"' -DIRIX -DSHARED_MEM iozone.c -o iozone_IRIX.o
	cc  -O -32 -c  -Dunix -DHAVE_ANSIC_C -DASYNC_IO \
		-DIRIX -DSHARED_MEM libasync.c -o libasync.o
	cc  -O -32 -c  -Dunix -DHAVE_ANSIC_C -DASYNC_IO \
		-DIRIX -DSHARED_MEM -DBIG_ENDIAN libbif.c -o libbif.o

iozone_sppux.o:	iozone.c libbif.c
	@echo ""
	@echo "Building iozone for SPP-UX using Convex compiler"
	@echo ""
	/opt/ansic/bin/cc -c  -O -Dunix -D_HPUX_SOURCE -D__convex_spp \
		-DNAME='"sppux"' -Wl,+parallel -DHAVE_ANSIC_C -DHAVE_PREAD \
		-DHAVE_PREADV iozone.c -o iozone_sppux.o
	/opt/ansic/bin/cc -c  -O -Dunix -D_HPUX_SOURCE -D__convex_spp \
		-Wl,+parallel -DHAVE_ANSIC_C -DHAVE_PREAD -DHAVE_PREADV -DBIG_ENDIAN libbif.c -o libbif.o

iozone_sppux-10.1.o:	iozone.c libbif.c
	@echo ""
	@echo "Building iozone for SPP-UX using HP ansic compiler"
	@echo ""
	/opt/ansic/bin/cc -c -O -Dunix -D_HPUX_SOURCE -D__convex_spp \
		-DHAVE_ANSIC_C -DHAVE_PREAD -DHAVE_PREADV iozone.c \
		 -DNAME='"sppux-10.1"' -Wl,+parallel -o iozone_sppux-10.1.o
	/opt/ansic/bin/cc -c -O -Dunix -D_HPUX_SOURCE -D__convex_spp \
		-DHAVE_ANSIC_C -DHAVE_PREAD -DHAVE_PREADV -DBIG_ENDIAN libbif.c \
		 -Wl,+parallel -o libbif.o

iozone_sppux_no-10.1.o:	iozone.c libbif.c
	@echo ""
	@echo "Building iozone for SPP-UX no ANSI c compiler"
	@echo ""
	/usr/ccs/bin/cc -c -O -Dunix -D_HPUX_SOURCE -D__convex_spp \
		-DNAME='"sppux_no_ansi_10.1"' -Wl,+parallel -DHAVE_PREAD \
		-DHAVE_PREADV iozone.c -o iozone_sppux_no-10.1.o
	/usr/ccs/bin/cc -c -O -Dunix -D_HPUX_SOURCE -D__convex_spp \
		-Wl,+parallel -DHAVE_PREAD -DHAVE_PREADV -DBIG_ENDIAN libbif.c -o libbif.o

iozone_convex.o:	iozone.c libbif.c
	@echo ""
	@echo "Building iozone for Convex 'C' series"
	@echo ""
		cc -c -O -Dunix -DNO_THREADS -Dbsd4_2 iozone.c \
			-DNAME='"Convex"' -o iozone_convex.o 
		cc -c -O -Dunix -DNO_THREADS -Dbsd4_2 -DBIG_ENDIAN libbif.c \
			-o libbif.o 

iozone_bsdi.o:	iozone.c libbif.c
	@echo ""
	@echo "Build iozone for BSD/OS"
	@echo ""
	cc -c -O -Dunix -Dbsd4_4 -DHAVE_ANSIC_C \
		-DNAME='"bsdi"' iozone.c -o iozone_bsdi.o
	cc -c -O -Dunix -Dbsd4_4 -DHAVE_ANSIC_C \
		libbif.c -o libbif.o

iozone_freebsd.o:	iozone.c libbif.c
	@echo ""
	@echo "Build iozone for FreeBSD"
	@echo ""
	cc -c ${CFLAGS}  -Dunix -Dbsd4_2 -DHAVE_ANSIC_C -DNO_THREADS \
		-DNAME='"freebsd"' -DSHARED_MEM iozone.c -o iozone_freebsd.o
	cc -c ${CFLAGS} -Dunix -Dbsd4_2 -DHAVE_ANSIC_C -DNO_THREADS \
		-DSHARED_MEM libbif.c -o libbif.o

iozone_macosx.o:	iozone.c libbif.c
	@echo ""
	@echo "Build iozone for MacOSX"
	@echo ""
	cc -c -O -Dunix -Dbsd4_2 -DHAVE_ANSIC_C -DNO_THREADS \
		-DNAME='"macosx"' -DSHARED_MEM iozone.c -o iozone_macosx.o
	cc -c -O -Dunix -Dbsd4_2 -DHAVE_ANSIC_C -DNO_THREADS \
		-DSHARED_MEM libbif.c -o libbif.o

iozone_openbsd.o:	iozone.c libbif.c
	@echo ""
	@echo "Build iozone for OpenBSD"
	@echo ""
	cc -c -O -Dunix -Dbsd4_4 -DHAVE_ANSIC_C -DNO_THREADS \
		-DNAME='"openbsd"' -DSHARED_MEM iozone.c -o iozone_openbsd.o
	cc -c -O -Dunix -Dbsd4_4 -DHAVE_ANSIC_C -DNO_THREADS \
		-DSHARED_MEM libbif.c -o libbif.o

iozone_openbsd-threads.o:	iozone.c libbif.c
	@echo ""
	@echo "Build iozone for OpenBSD with threads"
	@echo ""
	cc -c -O -pthread -Dunix -Dbsd4_4 -DHAVE_ANSIC_C \
		-DNAME='"openbsd-threads"' iozone.c -o iozone_openbsd-threads.o
	cc -c -O -pthread -Dunix -Dbsd4_4 -DHAVE_ANSIC_C \
		libbif.c -o libbif.o

iozone_OSFV3.o:	iozone.c libbif.c
	@echo ""
	@echo "Build iozone for OSFV3"
	@echo ""
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV3 \
		-DNAME='"OSFV3"' -DNO_PRINT_LLD -DOSF_64 iozone.c -o iozone_OSFV3.o
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV3 \
		-DNO_PRINT_LLD  -DOSF_64 libbif.c -o libbif.o
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV3 \
		-DNO_PRINT_LLD -DOSF_64 libasync.c -o libasync.o

iozone_OSFV4.o:	iozone.c libbif.c
	@echo ""
	@echo "Build iozone for OSFV4"
	@echo ""
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV4 \
		-DNAME='"OSFV4"' -DNO_PRINT_LLD -DOSF_64 iozone.c -o iozone_OSFV4.o
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV4 \
		-DNO_PRINT_LLD  -DOSF_64 libbif.c -o libbif.o
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV4 \
		-DNO_PRINT_LLD -DOSF_64 libasync.c -o libasync.o

iozone_OSFV5.o:	iozone.c libbif.c
	@echo ""
	@echo "Build iozone for OSFV5"
	@echo ""
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV5 \
		-DNAME='"OSFV5"' -DNO_PRINT_LLD -DOSF_64 iozone.c -o iozone_OSFV5.o
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV5 \
		-DNO_PRINT_LLD  -DOSF_64 libbif.c -o libbif.o
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV5 \
		-DNO_PRINT_LLD -DOSF_64 libasync.c -o libasync.o

iozone_TRU64.o:	iozone.c libbif.c
	@echo ""
	@echo "Build iozone for TRU64"
	@echo ""
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV5 -DTRU64 \
		-DNAME='"TRU64"' -DNO_PRINT_LLD -DOSF_64 -pthread iozone.c \
		-o iozone_TRU64.o
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV5 \
		-DNO_PRINT_LLD  -DOSF_64 libbif.c -o libbif.o
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV5 \
		-DNO_PRINT_LLD -DOSF_64 libasync.c -o libasync.o

iozone_SCO.o:	iozone.c libbif.c
	@echo ""
	@echo "Building iozone SCO "
	@echo ""
	gcc -c -O -DSCO -Dunix -DHAVE_ANSIC_C -DNO_THREADS \
		-DNAME='"SCO"' iozone.c -o iozone_SCO.o
	gcc -c -O -DSCO -Dunix -DHAVE_ANSIC_C -DNO_THREADS \
		-DBIG_ENDIAN libbif.c -o libbif.o

iozone_SCO_Unixware_gcc.o:	iozone.c libbif.c libasync.c
	@echo ""
	@echo "Building iozone SCO_Unixware_gcc "
	@echo ""
	/usr/local/bin/gcc -c -O -DSCO_Unixware_gcc -Dunix -DHAVE_ANSIC_C  \
		-DASYNC_IO -D_LARGEFILE64_SOURCE iozone.c  \
		-DNAME='"SCO_Unixware_gcc"' -o iozone_SCO_Unixware_gcc.o
	/usr/loca/bin/gcc -c -O -DSCO_Unixware_gcc -Dunix -DHAVE_ANSIC_C  \
		-DASYNC_IO -D_LARGEFILE64_SOURCE libbif.c -o libbif.o
	/usr/loca/bin/gcc -c -O -DSCO_Unixware_gcc -Dunix -DHAVE_ANSIC_C  \
		-DASYNC_IO -D_LARGEFILE64_SOURCE libasync.c -o libasync.o

iozone_netbsd.o:	iozone.c libbif.c
	@echo ""
	@echo "Building iozone NetBSD "
	@echo ""
	cc -c -O -Dunix -Dbsd4_4 -DHAVE_ANSIC_C -DNO_THREADS \
		-DNAME='"netbsd"' -DSHARED_MEM iozone.c -o iozone_netbsd.o
	cc -c -O -Dunix -Dbsd4_4 -DHAVE_ANSIC_C -DNO_THREADS \
		-DSHARED_MEM libbif.c -o libbif.o
