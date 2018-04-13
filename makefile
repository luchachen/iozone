#
# Version $Revision: 1.46 $
#
# The makefile for building all versions of Iozone for all supported
# platforms
#
# Supports:	hpux, hpux_no_ansi, hpux-10.1, hpux_no_ansi-10.1,
#		sppux, sppux-10.1, ghpux, sppux, 
#		convex, FreeBSD, OpenBSD, OSFV3, OSFV4, OSFV5, SCO
#		NetBSD


all:  
	@echo ""
	@echo "You must specify the target.        "
	@echo "        ->   AIX                  (32bit)   <-"
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
	@echo "        ->   netbsd               (32bit)   <-"
	@echo "        ->   openbsd              (32bit)   <-"
	@echo "        ->   OSFV3                (64bit)   <-"
	@echo "        ->   OSFV4                (64bit)   <-"
	@echo "        ->   OSFV5                (64bit)   <-"
	@echo "        ->   SCO                  (32bit)   <-"
	@echo "        ->   Solaris              (32bit)   <-"
	@echo "        ->   Solaris-2.6          (32bit)   <-"
	@echo "        ->   sppux                (32bit)   <-"
	@echo "        ->   sppux-10.1           (32bit)   <-"
	@echo "        ->   sppux_no_ansi-10.1   (32bit)   <-"
	@echo "        ->   Windows (95/98/NT)   (32bit)   <-"
	@echo ""

clean:
	rm -f *.o iozone

#
# Turn on the optimizer, largefiles, Posix async I/O and threads.
#
hpux-11.0: iozone_hpux-11.0.o libasync.o libbif.o
	 cc +O3 +Oparallel -D_LARGEFILE64_SOURCE -Dunix -D_HPUX_SOURCE -DASYNC_IO \
	-DHAVE_ANSIC_C -DVXFS iozone_hpux-11.0.o libasync.o libbif.o -lpthread -lrt -o iozone

#
# Turn on wide-mode, the optimizer, largefiles, Posix async I/O and threads.
#
hpux-11.0w: iozone_hpux-11.0w.o libasyncw.o libbif.o
	 cc +DA2.0w +O3 -D_LARGEFILE64_SOURCE -Dunix -D_HPUX_SOURCE -DASYNC_IO \
	-DHAVE_ANSIC_C -DVXFS iozone_hpux-11.0w.o libasyncw.o libbif.o -lpthread -lrt -o iozone


#
# Simple build with largefiles, Posix threads and Posix async I/O
#
hpuxs-11.0: iozone_hpuxs-11.0.o libasync.o libbif.o
	 cc -O -D_LARGEFILE64_SOURCE -Dunix -D_HPUX_SOURCE -DASYNC_IO \
	-DHAVE_ANSIC_C -DVXFS iozone_hpuxs-11.0.o libasync.o libbif.o -lpthread -lrt -o iozone

#
# Simple build with wide-mode, largefiles, Posix threads and Posix async I/O
#
hpuxs-11.0w: iozone_hpuxs-11.0w.o libasyncw.o libbif.o
	 cc -O +DA2.0w -D_LARGEFILE64_SOURCE -Dunix -D_HPUX_SOURCE -DASYNC_IO \
	-DHAVE_ANSIC_C -DVXFS iozone_hpuxs-11.0w.o libasyncw.o libbif.o -lpthread -lrt -o iozone

#
# Simple 10.1 build with no threads, no largefiles, no async I/O 
#
hpux-10.1: iozone_hpux-10.1.o  libbif.o
	 c89 +e -O -Dunix -D_HPUX_SOURCE \
	-DNO_THREADS -DHAVE_ANSIC_C iozone_hpux-10.1.o libbif.o -o iozone

hpux-10.20: iozone_hpux-10.20.o  libbif.o
	 c89 +e -O -Dunix -D_HPUX_SOURCE \
	-DNO_THREADS -DHAVE_ANSIC_C iozone_hpux-10.20.o libbif.o -o iozone

#
# Simple generic HP build with no threads, no largefiles, no async I/O 
#
hpux: iozone_hpux.o 
	c89 +e -O -Dunix -D_HPUX_SOURCE \
	-DNO_THREADS -DHAVE_ANSIC_C iozone_hpux.o libbif.o -o iozone

#
# GNU HP build with no threads, no largefiles, no async I/O 
#
ghpux: iozone_ghpux.o  libbif.o
	gcc  -O -Dunix -D_HPUX_SOURCE -DHAVE_ANSIC_C iozone_ghpux.o \
	libbif.o -DNO_THREADS -static -o iozone

#
# GNU Generic build with no threads, no largefiles, no async I/O 
#
generic: iozone_generic.o  libbif.o
	$(CC)  -O -Dgeneric -Dunix -DHAVE_ANSIC_C iozone_generic.o \
		libbif.o -DNO_THREADS -o iozone

#
# No ansii 'C' compiler HP build with no threads, no largefiles, no async I/O 
#
hpux_no_ansi-10.1: iozone_hpux_no-10.1.o  libbif.o 
	/opt/ansic/bin/cc  -O -Dunix -D_HPUX_SOURCE iozone_hpux_no-10.1.o \
	libbif.o -DNO_THREADS -o iozone

#
# No ansii 'C' compiler HP build with no threads, no largefiles, no async I/O 
#
hpux_no_ansi: iozone_hpux_no.o  libbif.o
	c89  -O -Dunix -D_HPUX_SOURCE iozone_hpux_no.o \
		-DNO_THREADS libbif.o -o iozone

#
# GNU 'C' compiler Linux build with threads, no largefiles, no async I/O 
#
linux: iozone_linux.o  libbif.o libasync.o
	cc  -O3 -Dunix -DHAVE_ANSIC_C -DSHARED_MEM -DASYNC_IO \
		-D_LARGEFILE64_SOURCE -Dlinux \
		iozone_linux.o libasync.o libbif.o -lpthread \
		-lrt -o iozone

#
# GNU 'C' compiler Linux build with no threads, no largefiles, no async I/O 
#
linux-ia64: iozone_linux-ia64.o  libbif.o
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
AIX: iozone_AIX.o  libbif.o  
	cc  -O -D__AIX__ -D_NO_PROTO -Dunix -DHAVE_ANSIC_C \
		-DSHARED_MEM  \
		iozone_AIX.o libbif.o -lpthreads -o iozone

#
# IRIX 32 bit build with threads, largefiles, async I/O 
# This would like to be in 64 bit mode but it hangs whenever in 64 bit mode.
# This version uses the 64 bit interfaces but is compiled as 32 bit code
#
IRIX64: iozone_IRIX64.o libasyncw.o libbif.o 
	cc   -32 -O -Dunix -DHAVE_ANSIC_C -D_LARGEFILE64_SOURCE -DSHARED_MEM -DASYNC_IO \
		-DIRIX64 iozone_IRIX64.o libbif.o -lpthread libasyncw.o -o iozone

#
# IRIX 32 bit build with threads, No largefiles, and async I/O 
# This version uses the 32 bit interfaces and is compiled as 32 bit code
#
IRIX: iozone_IRIX.o libasync.o libbif.o
	cc  -O  -32  -Dunix -DHAVE_ANSIC_C -DSHARED_MEM -DASYNC_IO \
		-DIRIX iozone_IRIX.o libbif.o -lpthread libasync.o -o iozone

#
# SPP-UX 32 bit build with threads, No largefiles, and No async I/O, pread extensions
# For older SPP-UX machines with 9.05 compatibility
#
sppux: iozone_sppux.o  libbif.o
	/opt/ansic/bin/cc  -O -Dunix -D_HPUX_SOURCE -D__convex_spp \
	-DHAVE_ANSIC_C -DHAVE_PREAD iozone_sppux.o  libbif.o \
	-Wl,+parallel -lcnx_syscall -lpthread -lail -o iozone

#
# SPP-UX 32 bit build with threads, No largefiles, and No async I/O, pread extensions
# For Newer SPP-UX machines with 10.01 compatibility
#
sppux-10.1: iozone_sppux-10.1.o libbif.o
	/opt/ansic/bin/cc -O -Dunix -D_HPUX_SOURCE -D__convex_spp \
	-DHAVE_ANSIC_C -DHAVE_PREAD iozone_sppux-10.1.o libbif.o \
	-lcnx_syscall  -Wl,+parallel -lpthread -lail -o iozone

#
# SPP-UX 32 bit build with threads, No largefiles, and No async I/O, pread extensions
# For Newer SPP-UX machines with 10.01 compatibility, and no ansi 'C' compiler.
#
sppux_no_ansi-10.1: iozone_sppux_no-10.1.o libbif.o
	/usr/ccs/bin/cc  -O -Dunix -D_HPUX_SOURCE -DHAVE_PREAD -D__convex_spp \
		iozone_sppux_no-10.1.o libbif.o \
		-Wl,+parallel -lcnx_syscall  \
		-lpthread -lail -o iozone

#
# Convex 'C' series 32 bit build with No threads, No largefiles, and No async I/O
#
convex: iozone_convex.o libbif.o
	cc -O -Dunix -Dbsd4_2 -DNO_THREADS iozone_convex.o libbif.o -o iozone

#
# Solaris 32 bit build with threads, largefiles, and async I/O
#
Solaris: iozone_solaris.o libasync.o libbif.o 
	cc  -O -Dunix -DHAVE_ANSIC_C -DASYNC_IO \
		-Dsolaris iozone_solaris.o libasync.o libbif.o \
		-D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -lthread -lpthread -lposix4 -o iozone

#
# Solaris 2.6 (32 bit) build with no threads, no largefiles, and no async I/O
#
Solaris-2.6: iozone_solaris-2.6.o libbif.o 
	cc  -O -Dunix -DHAVE_ANSIC_C -Dsolaris iozone_solaris-2.6.o libbif.o \
		-o iozone

#
# Windows build requires Cygnus development environment. You
# can get this from www.cygnus.com
# No threads, No largefiles, No async I/O
#
Windows: iozone_windows.o libbif.o
	gcc  -O -Dunix -DHAVE_ANSIC_C -DNO_THREADS \
		-DWindows iozone_windows.o libbif.o -o iozone

#
# GNU C compiler BSDI build with threads, largefiles, no async I/O
#

bsdi: iozone_bsdi.o libbif.o
	cc -O -Dunix -Dbsd4_4 -DHAVE_ANSIC_C  \
		iozone_bsdi.o libbif.o -o iozone

#
# GNU C compiler FreeBSD build with no threads, no largefiles, no async I/O
#

freebsd: iozone_freebsd.o libbif.o
	cc -O -Dunix -DHAVE_ANSIC_C -DNO_THREADS -DSHARED_MEM \
		iozone_freebsd.o libbif.o -o iozone
#
# GNU C compiler OpenBSD build with no threads, no largefiles, no async I/O
#

openbsd: iozone_openbsd.o libbif.o
	cc -O -Dunix -DHAVE_ANSIC_C -DNO_THREADS -DSHARED_MEM \
		iozone_openbsd.o libbif.o -o iozone

#
# GNU C compiler OSFV3 build 
# Has threads and async I/O but no largefiles.
#

OSFV3: iozone_OSFV3.o libbif.o libasync.o
	cc -O -Dunix -DHAVE_ANSIC_C -DASYNC_IO \
		-DNO_PRINT_LLD -DOSF_64 -DOSFV3 iozone_OSFV3.o libbif.o \
		-lpthreads libasync.o -laio -o iozone

#
# GNU C compiler OSFV4 build 
# Has threads and async I/O but no largefiles.
#

OSFV4: iozone_OSFV4.o libbif.o libasync.o
	cc -O -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV4 \
		-DNO_PRINT_LLD -DOSF_64 iozone_OSFV4.o libbif.o -lpthread \
		libasync.o -laio -o iozone

#
# GNU C compiler OSFV5 build 
# Has threads and async I/O but no largefiles.
#

OSFV5: iozone_OSFV5.o libbif.o libasync.o
	cc -O -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV5 \
		-DNO_PRINT_LLD -DOSF_64 iozone_OSFV5.o libbif.o -lpthread \
		libasync.o -laio -o iozone

#
# GNU Generic build with no threads, no largefiles, no async I/O
# for SCO
# Note: Be sure you have the latest patches for SCO's Openserver
# or you will get warnings about timer problems.
#

SCO: iozone_SCO.o  libbif.o
	gcc -O -DSCO -Dunix -DHAVE_ANSIC_C iozone_SCO.o \
		libbif.o -DNO_THREADS -o iozone

#
# GNU Generic build with no threads, no largefiles, no async I/O
# for NetBSD
#

netbsd: iozone_netbsd.o  libbif.o
	gcc -O -Dnetbsd -Dunix -DHAVE_ANSIC_C iozone_netbsd.o \
		libbif.o -DNO_THREADS -o iozone

#
#
# Now for the machine specific stuff
#

iozone_hpux.o: iozone.c libbif.c
	@echo ""
	@echo "Building iozone for HP-UX (9.05)"
	@echo ""
	c89 +e -c -O -Dunix -D_HPUX_SOURCE -DHAVE_ANSIC_C -DNO_THREADS \
		iozone.c  -o iozone_hpux.o
	c89 +e -c -O -Dunix -D_HPUX_SOURCE -DHAVE_ANSIC_C -DNO_THREADS \
		-DBIG_ENDIAN libbif.c  -o libbif.o

iozone_hpux-11.0.o: iozone.c libasync.c libbif.c
	@echo ""
	@echo "Building iozone for HP-UX (11.0)"
	@echo ""
	cc -c  +O3 +Oparallel -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE \
		-DHAVE_ANSIC_C -DASYNC_IO -DVXFS iozone.c  \
		-o iozone_hpux-11.0.o
	cc -c  +O3 +Oparallel -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE \
		-DHAVE_ANSIC_C -DASYNC_IO -DVXFS libasync.c  -o libasync.o
	cc -c  +O3 +Oparallel -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE \
		-DHAVE_ANSIC_C -DASYNC_IO -DVXFS -DBIG_ENDIAN libbif.c  -o libbif.o

iozone_hpux-11.0w.o: iozone.c libasync.c libbif.c
	@echo ""
	@echo "Building iozone for HP-UX (11.0w)"
	@echo ""
	cc -c +DA2.0w +O3 -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE \
	    -DHAVE_ANSIC_C -DASYNC_IO -DVXFS iozone.c  -o iozone_hpux-11.0w.o
	cc -c +DA2.0w +O3 -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE \
		-DHAVE_ANSIC_C -DASYNC_IO -DVXFS libasync.c  -o libasyncw.o
	cc -c +DA2.0w +O3 -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE \
		-DHAVE_ANSIC_C -DASYNC_IO -DVXFS -DBIG_ENDIAN libbif.c  -o libbif.o

iozone_hpuxs-11.0.o: iozone.c libasync.c libbif.c
	@echo ""
	@echo "Building simple iozone for HP-UX (11.0)"
	@echo ""
	cc -c  -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE -DHAVE_ANSIC_C \
		-DASYNC_IO -DVXFS iozone.c  -o iozone_hpuxs-11.0.o
	cc -c  -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE -DHAVE_ANSIC_C \
		-DASYNC_IO  -DVXFS libasync.c  -o libasync.o 
	cc -c  -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE -DHAVE_ANSIC_C \
		-DASYNC_IO -DVXFS -DBIG_ENDIAN libbif.c  -o libbif.o 

iozone_hpuxs-11.0w.o: iozone.c libasync.c libbif.c
	@echo ""
	@echo "Building simple iozone for HP-UX (11.0w)"
	@echo ""
	cc -c +DA2.0w -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE \
		-DHAVE_ANSIC_C -DASYNC_IO -DVXFS iozone.c  \
		-o iozone_hpuxs-11.0w.o
	cc -c +DA2.0w -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE \
		-DHAVE_ANSIC_C -DASYNC_IO -DVXFS libasync.c  -o libasyncw.o 
	cc -c +DA2.0w -Dunix -D_LARGEFILE64_SOURCE  -D_HPUX_SOURCE \
		-DHAVE_ANSIC_C -DASYNC_IO -DVXFS -DBIG_ENDIAN libbif.c  -o libbif.o 

iozone_hpux-10.1.o: iozone.c libbif.c
	@echo ""
	@echo "Building iozone for HP-UX (10.1)"
	@echo ""
	c89 +e -c -O -Dunix -D_HPUX_SOURCE -DHAVE_ANSIC_C -DNO_THREADS \
		iozone.c  -o iozone_hpux-10.1.o
	c89 +e -c -O -Dunix -D_HPUX_SOURCE -DHAVE_ANSIC_C -DNO_THREADS \
		-DBIG_ENDIAN libbif.c  -o libbif.o

iozone_hpux-10.20.o: iozone.c libbif.c
	@echo ""
	@echo "Building iozone for HP-UX (10.20)"
	@echo ""
	c89 +e -c -O -Dunix -D_HPUX_SOURCE -DHAVE_ANSIC_C -DNO_THREADS \
		iozone.c  -o iozone_hpux-10.20.o
	c89 +e -c -O -Dunix -D_HPUX_SOURCE -DHAVE_ANSIC_C -DNO_THREADS \
		-DBIG_ENDIAN libbif.c  -o libbif.o

iozone_ghpux.o: iozone.c libbif.c
	@echo ""
	@echo "Building iozone for GCC HP-UX (9.05) "
	@echo ""
	gcc -c -O -Dunix -D_HPUX_SOURCE -DHAVE_ANSIC_C -DNO_THREADS iozone.c \
		-o iozone_ghpux.o
	gcc -c -O -Dunix -D_HPUX_SOURCE -DHAVE_ANSIC_C -DNO_THREADS -DBIG_ENDIAN \
		libbif.c -o libbif.o

iozone_generic.o: iozone.c libbif.c
	@echo ""
	@echo "Building iozone Generic "
	@echo ""
	$(CC) -c -O -Dgeneric -Dunix -DHAVE_ANSIC_C -DNO_THREADS \
		iozone.c -o iozone_generic.o
	$(CC) -c -O -Dgeneric -Dunix -DHAVE_ANSIC_C -DNO_THREADS \
		-DBIG_ENDIAN libbif.c -o libbif.o

iozone_hpux_no.o: iozone.c libbif.c
	@echo ""
	@echo "Building iozone for HP-UX (9.05) without ansi compiler"
	@echo ""
	/opt/ansic/bin/cc -c -O -Dunix -D_HPUX_SOURCE -DNO_THREADS iozone.c -o \
		iozone_hpux_no.o
	/opt/ansic/bin/cc -c -O -Dunix -D_HPUX_SOURCE -DNO_THREADS \
		-DBIG_ENDIAN libbif.c -o libbif.o

iozone_hpux_no-10.1.o: iozone.c
	@echo ""
	@echo "Building iozone for HP-UX (10.1) without ansi compiler"
	@echo ""
	/opt/ansic/bin/cc -c -O -Dunix -D_HPUX_SOURCE -DNO_THREADS iozone.c -o \
		iozone_hpux_no-10.1.o
	/opt/ansic/bin/cc -c -O -Dunix -D_HPUX_SOURCE -DNO_THREADS \
		-DBIG_ENDIAN libbif.c -o libbif.o

iozone_linux.o: iozone.c libbif.c libasync.c
	@echo ""
	@echo "Building iozone for Linux"
	@echo ""
	cc -c -O3 -Dunix -DHAVE_ANSIC_C -DASYNC_IO \
		-DSHARED_MEM -Dlinux -D_LARGEFILE64_SOURCE iozone.c \
		-o iozone_linux.o
	cc -c -O3 -Dunix -DHAVE_ANSIC_C -DASYNC_IO -D_LARGEFILE64_SOURCE \
		-DSHARED_MEM -Dlinux libbif.c -o libbif.o
	cc -c -O3 -Dunix -Dlinux -DHAVE_ANSIC_C -DASYNC_IO \
		-D_LARGEFILE64_SOURCE libasync.c  -o libasync.o 
iozone_linux-ia64.o: iozone.c libbif.c
	@echo ""
	@echo "Building iozone for Linux-ia64"
	@echo ""
	cc -c -O3 -Dunix -DHAVE_ANSIC_C -DNO_THREADS \
		-DSHARED_MEM -Dlinux -D_LARGEFILE64_SOURCE iozone.c \
		-o iozone_linux-ia64.o
	cc -c -O3 -Dunix -DHAVE_ANSIC_C -D_LARGEFILE64_SOURCE \
		-DSHARED_MEM -Dlinux libbif.c -o libbif.o

iozone_AIX.o: iozone.c libbif.c 
	@echo ""
	@echo "Building iozone for AIX"
	@echo ""
	cc -c -O -D__AIX__ -D_NO_PROTO -Dunix -DHAVE_ANSIC_C  \
		-DSHARED_MEM  iozone.c -o iozone_AIX.o
	cc -c -O -D__AIX__ -D_NO_PROTO -Dunix -DHAVE_ANSIC_C  \
		-DSHARED_MEM  libbif.c -o libbif.o

iozone_solaris.o: iozone.c libasync.c libbif.c
	@echo ""
	@echo "Building iozone for Solaris"
	@echo ""
	cc -c -O -Dunix -DHAVE_ANSIC_C -DNNO_THREADS -DASYNC_IO \
		-D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -Dsolaris \
		iozone.c -o iozone_solaris.o
	cc -O -c  -Dunix -DHAVE_ANSIC_C -DASYNC_IO -D__LP64__ \
		-D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -Dsolaris \
		libasync.c -o libasync.o
	cc -O -c  -Dunix -DHAVE_ANSIC_C -DASYNC_IO -D__LP64__ \
		-D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -Dsolaris \
		-DBIG_ENDIAN libbif.c -o libbif.o
#
#		-DSHARED_MEM -Dsolaris libasync.c -o libasync.o
#		-DSHARED_MEM -Dsolaris iozone.c -o iozone_solaris.o
#

iozone_solaris-2.6.o: iozone.c libbif.c
	@echo ""
	@echo "Building iozone for Solaris-2.6"
	@echo ""
	cc -c -O -Dunix -DHAVE_ANSIC_C -DNO_THREADS \
		-Dsolaris  iozone.c -o iozone_solaris-2.6.o
	cc -O -c  -Dunix -DHAVE_ANSIC_C \
		-Dsolaris -DBIG_ENDIAN libbif.c -o libbif.o

iozone_windows.o: iozone.c libasync.c libbif.c
	@echo ""
	@echo "Building iozone for Windows (No threads, No async I/O)"
	@echo ""
	gcc -c -O -Dunix -DHAVE_ANSIC_C -DNO_THREADS  \
		-DWindows iozone.c -o iozone_windows.o
	gcc -c -O -Dunix -DHAVE_ANSIC_C -DNO_THREADS  \
		-DWindows libbif.c -o libbif.o
	
iozone_IRIX64.o: iozone.c libasync.c libbif.c
	@echo ""
	@echo "Building iozone for IRIX64"
	@echo ""
	cc -32 -O -c  -Dunix -DHAVE_ANSIC_C -D_LARGEFILE64_SOURCE -DASYNC_IO \
		-DIRIX64 -DSHARED_MEM iozone.c -o iozone_IRIX64.o
	cc -32 -O -c  -Dunix -DHAVE_ANSIC_C -D_LARGEFILE64_SOURCE -DASYNC_IO \
		-DIRIX64 -DSHARED_MEM libasync.c -o libasyncw.o
	cc -32 -O -c  -Dunix -DHAVE_ANSIC_C -D_LARGEFILE64_SOURCE -DASYNC_IO \
		-DIRIX64 -DSHARED_MEM -DBIG_ENDIAN libbif.c -o libbif.o

iozone_IRIX.o: iozone.c libasync.c libbif.c
	@echo ""
	@echo "Building iozone for IRIX"
	@echo ""
	cc  -O -32 -c  -Dunix -DHAVE_ANSIC_C -DASYNC_IO \
		-DIRIX -DSHARED_MEM iozone.c -o iozone_IRIX.o
	cc  -O -32 -c  -Dunix -DHAVE_ANSIC_C -DASYNC_IO \
		-DIRIX -DSHARED_MEM libasync.c -o libasync.o
	cc  -O -32 -c  -Dunix -DHAVE_ANSIC_C -DASYNC_IO \
		-DIRIX -DSHARED_MEM -DBIG_ENDIAN libbif.c -o libbif.o

iozone_sppux.o: iozone.c libbif.c
	@echo ""
	@echo "Building iozone for SPP-UX using Convex compiler"
	@echo ""
	/opt/ansic/bin/cc -c  -O -Dunix -D_HPUX_SOURCE -D__convex_spp \
		-Wl,+parallel -DHAVE_ANSIC_C -DHAVE_PREAD iozone.c -o iozone_sppux.o
	/opt/ansic/bin/cc -c  -O -Dunix -D_HPUX_SOURCE -D__convex_spp \
		-Wl,+parallel -DHAVE_ANSIC_C -DHAVE_PREAD -DBIG_ENDIAN libbif.c -o libbif.o

iozone_sppux-10.1.o: iozone.c libbif.c
	@echo ""
	@echo "Building iozone for SPP-UX using HP ansic compiler"
	@echo ""
	/opt/ansic/bin/cc -c -O -Dunix -D_HPUX_SOURCE -D__convex_spp \
		-DHAVE_ANSIC_C -DHAVE_PREAD iozone.c \
		 -Wl,+parallel -o iozone_sppux-10.1.o
	/opt/ansic/bin/cc -c -O -Dunix -D_HPUX_SOURCE -D__convex_spp \
		-DHAVE_ANSIC_C -DHAVE_PREAD -DBIG_ENDIAN libbif.c \
		 -Wl,+parallel -o libbif.o

iozone_sppux_no-10.1.o: iozone.c libbif.c
	@echo ""
	@echo "Building iozone for SPP-UX no ANSI c compiler"
	@echo ""
	/usr/ccs/bin/cc -c -O -Dunix -D_HPUX_SOURCE -D__convex_spp \
		-Wl,+parallel -DHAVE_PREAD iozone.c -o iozone_sppux_no-10.1.o
	/usr/ccs/bin/cc -c -O -Dunix -D_HPUX_SOURCE -D__convex_spp \
		-Wl,+parallel -DHAVE_PREAD -DBIG_ENDIAN libbif.c -o libbif.o

iozone_convex.o: iozone.c libbif.c
	@echo ""
	@echo "Building iozone for Convex 'C' series"
	@echo ""
		cc -c -O -Dunix -DNO_THREADS -Dbsd4_2 iozone.c \
			-o iozone_convex.o 
		cc -c -O -Dunix -DNO_THREADS -Dbsd4_2 -DBIG_ENDIAN libbif.c \
			-o libbif.o 

iozone_bsdi.o: iozone.c libbif.c
	@echo ""
	@echo "Build Iozone for BSDI"
	@echo ""
	cc -c -O -Dunix -Dbsd4_4 -DHAVE_ANSIC_C \
		iozone.c -o iozone_bsdi.o
	cc -c -O -Dunix -Dbsd4_4 -DHAVE_ANSIC_C \
		libbif.c -o libbif.o

iozone_freebsd.o: iozone.c libbif.c
	@echo ""
	@echo "Build Iozone for FreeBSD"
	@echo ""
	cc -c -O -Dunix -Dbsd4_2 -DHAVE_ANSIC_C -DNO_THREADS \
		-DSHARED_MEM iozone.c -o iozone_freebsd.o
	cc -c -O -Dunix -Dbsd4_2 -DHAVE_ANSIC_C -DNO_THREADS \
		-DSHARED_MEM libbif.c -o libbif.o

iozone_openbsd.o: iozone.c libbif.c
	@echo ""
	@echo "Build Iozone for OpenBSD"
	@echo ""
	cc -c -O -Dunix -Dbsd4_4 -DHAVE_ANSIC_C -DNO_THREADS \
		-DSHARED_MEM iozone.c -o iozone_openbsd.o
	cc -c -O -Dunix -Dbsd4_4 -DHAVE_ANSIC_C -DNO_THREADS \
		-DSHARED_MEM libbif.c -o libbif.o

iozone_OSFV3.o: iozone.c libbif.c
	@echo ""
	@echo "Build Iozone for OSFV3"
	@echo ""
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV3 \
		-DNO_PRINT_LLD -DOSF_64 iozone.c -o iozone_OSFV3.o
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV3 \
		-DNO_PRINT_LLD  -DOSF_64 libbif.c -o libbif.o
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV3 \
		-DNO_PRINT_LLD -DOSF_64 libasync.c -o libasync.o

iozone_OSFV4.o: iozone.c libbif.c
	@echo ""
	@echo "Build Iozone for OSFV4"
	@echo ""
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV4 \
		-DNO_PRINT_LLD -DOSF_64 iozone.c -o iozone_OSFV4.o
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV4 \
		-DNO_PRINT_LLD  -DOSF_64 libbif.c -o libbif.o
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV4 \
		-DNO_PRINT_LLD -DOSF_64 libasync.c -o libasync.o

iozone_OSFV5.o: iozone.c libbif.c
	@echo ""
	@echo "Build Iozone for OSFV5"
	@echo ""
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV5 \
		-DNO_PRINT_LLD -DOSF_64 iozone.c -o iozone_OSFV5.o
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV5 \
		-DNO_PRINT_LLD  -DOSF_64 libbif.c -o libbif.o
	cc -O -c -Dunix -DHAVE_ANSIC_C -DASYNC_IO -DOSFV5 \
		-DNO_PRINT_LLD -DOSF_64 libasync.c -o libasync.o

iozone_SCO.o: iozone.c libbif.c
	@echo ""
	@echo "Building iozone SCO "
	@echo ""
	gcc -c -O -DSCO -Dunix -DHAVE_ANSIC_C -DNO_THREADS \
		iozone.c -o iozone_SCO.o
	gcc -c -O -DSCO -Dunix -DHAVE_ANSIC_C -DNO_THREADS \
		-DBIG_ENDIAN libbif.c -o libbif.o

iozone_netbsd.o: iozone.c libbif.c
	@echo ""
	@echo "Building iozone NetBSD "
	@echo ""
	gcc -c -O -Dnetbsd -Dunix -DHAVE_ANSIC_C -DNO_THREADS \
		iozone.c -o iozone_netbsd.o
	gcc -c -O -Dnetbsd -Dunix -DHAVE_ANSIC_C -DNO_THREADS \
		libbif.c -o libbif.o

