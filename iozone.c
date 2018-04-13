/************************************************************************/
/*	Original Author: 						*/
/*		William Norcott (wnorcott@us.oracle.com)		*/
/*              4 Dunlap Drive						*/
/*              Nashua, NH 03060					*/
/*  									*/
/************************************************************************/
/*      Enhancements by:						*/
/*	 	Don Capps (capps@rsn.hp.com)				*/
/* 		7417 Crenshaw						*/
/* 		Plano, TX 75025						*/
/*  									*/
/************************************************************************/
/*  Copyright 1991, 1992, 1994, 1998, 2000, 2001  William D. Norcott	*/
/************************************************************************/
/*									*/
/* Iozone is based on the original work done by William Norrcot. It has	*/
/* been enhanced so that it provides a more complete filesystem   	*/
/* characterization.							*/
/* Its purpose is to provide automated filesystem characterization.	*/
/* Enhancements have been made by:					*/
/*									*/
/* Don Capps	   (HP/Convex)	capps@rsn.hp.com			*/ 
/* Isom Crawford   (HP/Convex)	isom@rsn.hp.com				*/
/* Kirby Collins   (HP/Convex)	kcollins@rsn.hp.com			*/
/* Al Slater	   (HP)		aslater@jocko.bri.hp.com		*/
/* Mark Kelly	   (HP)		mkelly@rsn.hp.com			*/
/*									*/
/* Iozone can perform single stream and multi stream I/O		*/
/* also it now performs read, write, re-read, re-write, 		*/
/* read backwards, read/write random, re-read record, 			*/
/* pread, re-pread, re-pwrite, preadv, re-preadv, pwritev,		*/
/* stride read, and re-pwritev						*/
/*									*/
/* The frontend now uses getopt() and the user can control many more	*/
/* of the actions.							*/
/*									*/
/* Iozone supports multi process throughput tests.			*/
/*									*/
/* Has support for processor cache issues.				*/
/*									*/
/************************************************************************/

/************************************************************************/
/* For the more casual user... 						*/
/* 									*/
/* 1. make sppux  (linux, hpux, convex, hpux_no_ansi)			*/
/* 2. type ./iozone -Ra							*/
/* 									*/
/*  Hint: Type make    (it will give you a list of valid targets)	*/
/* 									*/
/************************************************************************/


/* The version number */
#define THISVERSION "        Version $Revision: 3.88 $"

/* Include for Cygnus development environment for Windows */
#ifdef Windows
#include <Windows.h>
int errno;
#else
extern  int errno;
extern  int h_errno;
#endif


#if defined (__LP64__) || defined(OSF_64) || defined(__alpha__) || defined(__arch64__)
#define MODE "\tCompiled for 64 bit mode."
#define _64BIT_ARCH_
#else
#define MODE "\tCompiled for 32 bit mode."
#endif

#ifdef HAVE_PROTO
#include "proto.h"
#else
int atoi();
int close();
int unlink();
int main();
void record_command_line();
long wait();
int fsync();
void srand48();
long lrand48();
void create_list();
void Poll();
void print_header();
void Kill();
#ifndef Windows
long long l_min();
long long l_max();
#endif
long long mythread_create();
#endif


char *help[] = {
"    Usage: iozone [-s filesize_Kb] [-r record_size_Kb] [-f [path]filename] [-h]",
"                  [-i test] [-E] [-p] [-a] [-A] [-z] [-Z] [-m] [-M] [-t children]",
"                  [-l min_number_procs] [-u max_number_procs] [-v] [-R] [-x] [-o]",
"                  [-d microseconds] [-F path1 path2...] [-V pattern] [-j stride]",
"                  [-T] [-C] [-B] [-D] [-G] [-I] [-H depth] [-k depth] [-U mount_point]",
"                  [-S cache_size] [-O] [-L cacheline_size] [-K] [-g maxfilesize_Kb]",
"                  [-n minfilesize_Kb] [-N] [-Q] [-P start_cpu] [-e] [-c] [-b Excel.xls]",
"                  [-J milliseconds] [-X write_telemetry_filename] [-w] [-W]",
"                  [-Y read_telemetry_filename] [-y minrecsize_Kb] [-q maxrecsize_Kb]",
"                  [-+u] [-+m cluster_filename]",
" ",
"           -a  Auto mode",
"           -A  Auto2 mode",
"           -b Filename  Create Excel worksheet file",
"           -B  Use mmap() files",
"           -c  Include close in the timing calculations",
"           -C  Show bytes transferred by each child in throughput testing",
"           -d #  Microsecond delay out of barrier",
"           -D  Use msync(MS_ASYNC) on mmap files",
"           -e  Include flush (fsync,fflush) in the timing calculations",
"           -E  Run extension tests",
"           -f filename  to use",
"           -F filenames  for each process/thread in throughput test",
"           -g #  Set maximum file size (in Kbytes) for auto mode (or #m or #g)",
"           -G  Use msync(MS_SYNC) on mmap files",
"           -h  help",
"           -H #  Use POSIX async I/O with # async operations",
"           -i #  Test to run (0=write/rewrite, 1=read/re-read, 2=random-read/write",
"                 3=Read-backwards, 4=Re-write-record, 5=stride-read, 6=fwrite/re-fwrite",
"                 7=fread/Re-fread, 8=pwrite/Re-pwrite, 9=pread/Re-pread",
"                 10=pwritev/Re-pwritev, 11=preadv/Re-preadv)",
"           -I  Use VxFS VX_DIRECT or O_DIRECT for all file operations",
"           -j #  Set stride of file accesses to (# * record size)",
"           -J #  milliseconds of compute cycle before each I/O operation",
"           -k #  Use POSIX async I/O (no bcopy) with # async operations",
"           -K  Create jitter in the access pattern for readers",
"           -l #  Lower limit on number of processes to run",
"           -L #  Set processor cache line size to value (in bytes)",
"           -m  Use multiple buffers",
"           -M  Report uname -a output",
"           -n #  Set minimum file size (in Kbytes) for auto mode (or #m or #g)",
"           -N  Report results in microseconds per operation",
"           -o  Writes are synch (O_SYNC)",
"           -O  Give results in ops/sec.",
"           -p  Purge on",
"           -P #  Bind processes/threads to processors, starting with this cpu",
"           -q #  Set maximum record size (in Kbytes) for auto mode (or #m or #g)",
"           -Q  Create offset/latency files",
"           -r #  record size in Kb",
"              or -r #k .. size in Kb",
"              or -r #m .. size in Mb",
"              or -r #g .. size in Gb",
"           -R  Generate Excel report",
"           -s #  file size in Kb",
"              or -s #k .. size in Kb",
"              or -s #m .. size in Mb",
"              or -s #g .. size in Gb",
"           -S #  Set processor cache size to value (in Kbytes)",
"           -t #  Number of threads or processes to use in throughput test",
"           -T  Use POSIX pthreads for throughput tests",
"           -u #  Upper limit on number of processes to run",
"           -U  Mount point to remount between tests",
"           -v  version information",
"           -V  Verify data pattern write/read",
"           -w  Do not unlink temporary file",
"           -W  Lock file when reading or writing",
"           -x  Turn off stone-walling",
"           -X filename  Write telemetry file. Contains lines with (offset reclen compute_time) in ascii",
"           -y #  Set minimum record size (in Kbytes) for auto mode (or #m or #g)",
"           -Y filename  Read  telemetry file. Contains lines with (offset reclen compute_time) in ascii",
"           -z  Used in conjunction with -a to test all possible record sizes",
"           -Z  Enable mixing of mmap I/O and file I/O",
"           -+m  Cluster_filename   Enable Cluster testing",
"           -+u  Enable CPU utilization output (Experimental)",
"" };

char *head1[] = {
  "       'Iozone' Filesystem Benchmark Program",
  " ",
THISVERSION,
  MODE,
  " ",
  "       Original Author: William Norcott (wnorcott@us.oracle.com)",
  "               4 Dunlap Drive",
  "               Nashua, NH 03060",
  " ",
  "       Enhancements: Don Capps (capps@rsn.hp.com)",
  "        	  7417 Crenshaw",
  "        	  Plano, TX 75025",
  " ",
  "  Copyright 1991, 1992, 1994, 1998, 1999   William D. Norcott",
  " ",
  "  License to freely use and distribute this software is hereby granted ",
  "  by the author, subject to the condition that this copyright notice ",
  "  remains intact.  The author retains the exclusive right to publish ",
  "  derivative works based on this work, including, but not limited to, ",
  "  revised versions of this work",
  " ",
  "  Other contributors:",
  " ",
  "  Don Capps       (Hewlett Packard)	capps@rsn.hp.com",
  "  Isom Crawford   (Hewlett Packard)	isom@rsn.hp.com",
  "  Kirby Collins   (Hewlett Packard)	kcollins@rsn.hp.com",
  "  Mark Kelly      (Hewlett Packard)	mkelly@rsn.hp.com",
  " ",
  ""};

/******************************************************************

    INCLUDE FILES (system-dependent)

******************************************************************/
#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#if defined(linux)
  #define __USE_GNU
#endif
#include <fcntl.h>
#if !defined(__FreeBSD__) && !defined(__OpenBSD__)
#include <malloc.h>
#endif

#if defined (__FreeBSD__) || defined(__OpenBSD__) || defined(__bsdi__)
#ifndef O_SYNC
#define O_SYNC O_FSYNC
#endif
#endif

#if defined(OSFV5)
#include <string.h>
#endif

#ifndef MAP_FAILED
#define MAP_FAILED -1
#endif

#ifdef generic
typedef long long off64_t;
#endif

#ifndef solaris
#ifndef off64_t
#ifndef _OFF64_T
#ifndef __AIX__
#ifndef __off64_t_defined
#ifndef SCO_Unixware_gcc
typedef long long off64_t;
#endif
#endif
#endif
#endif
#endif
#endif

#ifdef __AIX__
#include <fcntl.h>
#endif

#ifdef VXFS
#include <sys/fs/vx_ioctl.h>
#endif

#ifdef unix
#include <sys/times.h>
#include <sys/file.h>
#include <sys/resource.h>
#ifndef NULL
#define NULL 0
#endif

#ifndef nolimits
#include <limits.h>
#endif
#endif

#ifdef HAVE_ANSIC_C
#define VOLATILE volatile
#else 
#define VOLATILE 
#endif

#include <sys/time.h>

#ifdef SHARED_MEM
#include <sys/shm.h>
#endif

#ifdef bsd4_2
#define MS_SYNC 0
#define MS_ASYNC 0
#endif

#ifdef bsd4_4
#define MAP_ANONYMOUS MAP_ANON
#endif

#ifdef SCO
#define AMAP_FILE (0)
#endif

#if defined(SCO_Unixware_gcc) || defined(solaris)
#define MAP_FILE (0)
#endif

#if defined(IRIX) || defined(IRIX64) || defined(Windows) || defined(bsd4_2) || defined(bsd4_4) || defined(SCO) || defined(Solaris) || defined(SCO_Unixware_gcc)
long long page_size = 4096;
#define GOT_PAGESIZE 1
#elif defined(NBPG)
long long page_size = NBPG;
#define GOT_PAGESIZE 1
#elif defined(linux)
#include <asm/page.h>
long long page_size = PAGE_SIZE;
#define GOT_PAGESIZE 1
#elif !defined(GOT_PAGESIZE)
long long page_size = 4096; /* Used when all else fails */
#endif

#ifdef HAVE_PREAD
#include <sys/puio.h>
#define PVECMAX 16
struct piovec piov[PVECMAX];
#endif

struct child_stats {
	long long flag;		/* control space */
	long long flag1;	/* pad */
	double walltime;	/* child elapsed time */
	double cputime;		/* child CPU time */
	double throughput; 	/* Throughput in either kb/sec or ops/sec */
	double actual;	   	/* Either actual kb read or # of ops performed */
	double start_time;	/* Actual start time */
	double stop_time;	/* Actual end time */
	double fini_time;	/* always set, while stop_time is not always set for a child */
} VOLATILE *child_stat;

struct runtime {
	double	walltime;
	double	cputime;
	double	cpuutil;
};

#ifdef __convex_spp
#include <sys/cnx_ail.h>
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


/* 
 * Messages the controlling process sends to children.
 */
struct client_command {
	char c_host_name[128];
	char c_client_name[128];
	char c_working_dir[256];
	char c_path_dir[256];
	char c_execute_name[256];
	char c_oflag;
	char c_jflag;
	char c_async_flag;
	char c_mmapflag;
	char c_verify;
	char c_Q_flag;
	char c_include_flush;
	char c_OPS_flag;
	char c_mmapnsflag;
	char c_mmapssflag;
	char c_no_copy_flag;
	char c_include_close;
	char c_disrupt_flag;
	char c_compute_flag;
	char c_xflag;
	int c_direct_flag;
	int c_client_number;
	int c_command;
	int c_testnum;
	int c_no_unlink;
	int  c_file_lock;
	long long c_delay;
	long long c_purge;
	long long c_fetchon;
	long long c_numrecs64;
	long long c_reclen;
	long long c_child_flag;
	double c_stop_flag;
};	

/* 
 * Messages the clients will send to the master.
 */
struct master_command {
	char m_host_name[128];
	char m_client_name[128];
	int m_client_number;
	int m_child_port;
	int m_child_async_port;
	int m_command;
	int m_testnum;
	double m_throughput;
	double m_stop_time;
	double m_start_time;
	double m_fini_time;
	double m_stop_flag;
	double m_actual;
	long long m_child_flag;
};	


/*
 * Possible values for the commands sent to the master
 */
#define R_CHILD_JOIN        1
#define R_STAT_DATA         2
#define R_FLAG_DATA         3

/*
 * Possible values for the master's commands sent to a client
 *
 * The R_FLAG_DATA is also used by the master to tell the 
 * client to update its flags.
 */
#define R_JOIN_ACK        4
#define R_STOP_FLAG       5
#define R_TERMINATE       6


/* These are the defaults for the processor. They can be 
 * over written by the command line options.
 */
#define CACHE_LINE_SIZE 32
#define CACHE_SIZE ( 1024 * 1024 )
#define MEG (1024 * 1024)

#define STRIDE 17
#ifndef NO_THREADS
#include <pthread.h>
#endif


/************************************************************************/
/*									*/
/*    DEFINED CONSTANTS							*/
/*									*/
/************************************************************************/

#define IBUFSIZE 100
#define DISRUPT 100
#define LARGE_REC 65536
#define KILOBYTES 512 			/* number of kilobytes in file */
#define RECLEN 1024			/* number of bytes in a record */
#define FILESIZE (KILOBYTES*1024)	/*size of file in bytes*/
#define NUMRECS FILESIZE/RECLEN		/* number of records */

#ifdef __bsdi__
#define CROSSOVER (8*1024)		/* At 8 Meg switch to large records */
#define MAXBUFFERSIZE (8*1024*1024)		/*maximum buffer size*/
#else
#define CROSSOVER (16*1024)		/* At 16 Meg switch to large records */
#define MAXBUFFERSIZE (16*1024*1024)		/*maximum buffer size*/
#endif

#define	MAXSTREAMS	48		/* maximum number of children */
#define MINBUFFERSIZE 128
#define TOOFAST 10
#define MAXTESTS 10
#define PATTERN 0xA5
#define MAX_X 100			/* Used for Excel internal tables */
#define MAX_Y 200			/* Used for Excel internal tables */
#define USAGE  "\tUsage: For usage information type iozone -h \n\n"


#define MAXNAMESIZE 1000                /* max # of characters in filename */

#ifdef NO_PRINT_LLD
#ifdef HAVE_PREAD
#include <sys/times.h>
#define CONTROL_STRING1 "%16ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%9ld%9ld%8ld%10ld%9ld%10ld%9ld%10ld%10ld%9ld\n"
#define CONTROL_STRING2 "%16s%8s%8s%8s%8s%10s%8s%8s%8s%8s%8s%9s%9s%8s%9s%8s%9s%7s%10s%10s%10s%9s%9s\n"
#define CONTROL_STRING3 "%16s%8s%8s%8s%8s%10s%8s%8s%8s%8s%8s%9s%9s%8s%9s\n"
#define CONTROL_STRING4 "%16s%8s%8s%8s%8s%10s\n"
#else
#define CONTROL_STRING1 "%16ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%9ld%9ld\n"
#define CONTROL_STRING2 "%16s%8s%8s%8s%8s%10s%8s%8s%8s%8s%8s%9s%9s%8s%9s\n"
#define CONTROL_STRING3 "%16s%8s%8s%8s%8s%10s%8s%8s%8s%8s%8s%9s%9s%8s%9s\n"
#define CONTROL_STRING4 "%16s%8s%8s%8s%8s%10s\n"
#endif
#endif

#ifndef NO_PRINT_LLD
#ifdef HAVE_PREAD
#include <sys/times.h>
#define CONTROL_STRING1 "%16lld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%9ld%9ld%8ld%10ld%9ld%10ld%9ld%10ld%10ld%9ld\n"
#define CONTROL_STRING2 "%16s%8s%8s%8s%8s%10s%8s%8s%8s%8s%8s%9s%9s%8s%9s%8s%9s%7s%10s%10s%10s%9s%9s\n"
#define CONTROL_STRING3 "%16s%8s%8s%8s%8s%10s%8s%8s%8s%8s%8s%9s%9s%8s%9s\n"
#define CONTROL_STRING4 "%16s%8s%8s%8s%8s%10s\n"
#else
#define CONTROL_STRING1 "%16lld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%8ld%9ld%9ld\n"
#define CONTROL_STRING2 "%16s%8s%8s%8s%8s%10s%8s%8s%8s%8s%8s%9s%9s%8s%9s\n"
#define CONTROL_STRING3 "%16s%8s%8s%8s%8s%10s%8s%8s%8s%8s%8s%9s%9s%8s%9s\n"
#define CONTROL_STRING4 "%16s%8s%8s%8s%8s%10s\n"
#endif
#endif

/* 
    For 'auto mode', these defines determine the number of iterations
    to perform for both the file size and the record length.
*/ 

#define KILOBYTES_START       64
#define KILOBYTES_END (1024*512)
#define RECLEN_START  4096
#define RECLEN_END    (MAXBUFFERSIZE)
#define MULTIPLIER    2

#define WRITER_TEST		0
#define READER_TEST		1
#define RANDOM_RW_TEST		2
#define REVERSE_TEST		3
#define REWRITE_REC_TEST	4
#define STRIDE_READ_TEST	5
#define FWRITER_TEST		6
#define FREADER_TEST		7
#ifdef HAVE_PREAD
#define PWRITER_TEST		8
#define PREADER_TEST		9
#define PWRITEV_TEST		10
#define PREADV_TEST		11
#endif /* HAVE_PREAD */

#define WRITER_MASK		(1 << WRITER_TEST)
#define READER_MASK		(1 << READER_TEST)
#define RANDOM_RW_MASK		(1 << RANDOM_RW_TEST)
#define REVERSE_MASK		(1 << REVERSE_TEST)
#define REWRITE_REC_MASK	(1 << REWRITE_REC_TEST)
#define STRIDE_READ_MASK	(1 << STRIDE_READ_TEST)
#define FWRITER_MASK		(1 << FWRITER_TEST)
#define FREADER_MASK		(1 << FREADER_TEST)
#ifdef HAVE_PREAD
#define PWRITER_MASK		(1 << PWRITER_TEST)
#define PREADER_MASK		(1 << PREADER_TEST)
#define PWRITEV_MASK		(1 << PWRITEV_TEST)
#define PREADV_MASK		(1 << PREADV_TEST)
#endif /* HAVE_PREAD */

/*
 * child_stat->flag values and transitions
 */
#define CHILD_STATE_HOLD	0	/* parent initializes children to HOLD */
#define CHILD_STATE_READY	1	/* child says when it's READY */
#define CHILD_STATE_BEGIN	2	/* parent tells child to BEGIN */
#define CHILD_STATE_DONE	3	/* child tells parent that it's DONE */
			/* children sometimes use HOLD instead of DONE when finished */


/******************************************************************/
/*								  */
/*    FUNCTION DECLARATIONS					  */
/*								  */
/******************************************************************/
char *initfile();
void mmap_end();
void auto_test();		/* perform automatic test series  */
void show_help();		/* show development help          */
static double time_so_far();	/* time since start of program    */
#ifdef unix
static double utime_so_far();	/* user time 			  */
static double stime_so_far();	/* system time   		  */
static double clk_tck();	/* Get clocks/tick		  */
static double cputime_so_far();
#else
#define cputime_so_far()	time_so_far()
#endif
static double time_so_far1();	/* time since start of program    */
void get_resolution();
void get_rusage_resolution();
void signal_handler();		/* clean up if user interrupts us */
void begin();			/* The main worker in the app     */
void fetchit();			/* Prime on chip cache		  */
void purgeit();			/* Purge on chip cache		  */
void throughput_test();		/* Multi process throughput 	  */
void multi_throughput_test();	/* Multi process throughput 	  */
void prepage();			/* Pre-fault user buffer	  */
double do_compute();		/* compute cycle simulation       */

void write_perf_test();		/* write/rewrite test		  */
void fwrite_perf_test();	/* fwrite/refwrite test		  */
void fread_perf_test();		/* fread/refread test		  */
void read_perf_test();		/* read/reread test		  */
void random_perf_test();	/* random read/write test	  */
void reverse_perf_test();	/* reverse read test		  */
void rewriterec_perf_test();	/* rewrite record test		  */
void read_stride_perf_test();	/* read with stride test	  */
#ifdef HAVE_PREAD
void pread_perf_test();		/* pread/re-pread test		  */
void preadv_perf_test();	/* preadv/re-preadv test	  */
void pwrite_perf_test();	/* pwrite/re-pwrite test	  */
void pwritev_perf_test();	/* pwritev/re-pwritev test	  */
#endif /* HAVE_PREAD */
void store_dvalue();		/* Store doubles array 		  */
void dump_excel();
void dump_throughput();
#ifdef HAVE_ANSIC_C
void end_async();
void takeoff_cache();
void del_cache();
void async_init();
void fill_area(long long *, long long *, long long);
void fill_buffer(char *,long long ,long long ,char );
void store_value(off64_t);
void store_times(double, double);
static double cpu_util(double, double);
void dump_cputimes(void);
void purge_buffer_cache(void);
char *alloc_mem(long long);
void *(thread_rwrite_test)(void *);
void *(thread_write_test)(void *);
void *(thread_read_test)(void*);
void *(thread_cleanup_test)(void*);
void *(thread_cleanup_quick)(void*);
void *(thread_ranread_test)(void *);
void *(thread_ranwrite_test)(void *);
void *(thread_rread_test)(void *);
void *(thread_reverse_read_test)(void *);
void *(thread_stride_read_test)(void *);
void *(thread_set_base)(void *);
void *(thread_join)(long long, void *);
void disrupt(int);
long long get_traj(FILE *, long long *, double *, long);
void create_temp(off64_t, long long );
FILE *open_w_traj(void);
FILE *open_r_traj(void);
void traj_vers(void);
long long r_traj_size(void);
long long w_traj_size(void);
void init_file_sizes();
off64_t get_next_file_size(off64_t);
void add_file_size(off64_t);
void init_file_sizes( off64_t,  off64_t);
off64_t get_next_record_size(off64_t);
void add_record_size(off64_t);
void init_record_sizes( off64_t,  off64_t);
void del_record_sizes( void );
#else
void traj_vers();
long long r_traj_size();
long long w_traj_size();
FILE *open_w_traj();
FILE *open_r_traj();
void create_temp();
void fill_buffer();
char *alloc_mem();
void *(thread_rwrite_test)();
void *(thread_write_test)();
void *(thread_read_test)();
void *(thread_cleanup_test)();
void *(thread_ranread_test)();
void *(thread_ranwrite_test)();
void *(thread_rread_test)();
void *(thread_reverse_read_test)();
void *(thread_stride_read_test)();
void *(thread_set_base)();
void *(thread_join)();
void disrupt();
long long get_traj();
void init_file_sizes();
off64_t get_next_file_size();
void add_file_size();
void init_record_sizes();
off64_t get_next_record_size();
void add_record_size();
#endif

/************************************************************************/
/* The list of tests to be called.					*/
/************************************************************************/
void (*func[])() = { 
			write_perf_test, 
			read_perf_test,
			random_perf_test,
			reverse_perf_test,
			rewriterec_perf_test,
			read_stride_perf_test,
			fwrite_perf_test,
			fread_perf_test 
#ifdef HAVE_PREAD
			,
			pwrite_perf_test,
			pread_perf_test,
			pwritev_perf_test,
			preadv_perf_test
#endif /* HAVE_PREAD */
	};

char *test_output[] = {"                ",
		      "                  ",
		      "                ",
		      "        ",
		      "        ",
		      "        ",
		      "                  ",
		      "                ",
		      "                 ",
		      "                 ",
		      "                   ",
		      "                   \n" };
long long test_soutput[] = {2,2,2,1,1,1,2,2,2,2,2,2};


/******************************************************************/
/*								  */
/*    GLOBAL VARIABLES					          */
/*								  */
/*******************************************************************/

#define SHMSIZE  ((( sizeof(struct child_stats) * MAXSTREAMS) )+4096 )
VOLATILE struct child_stats *shmaddr;
double totaltime,total_time, temp_time ,total_kilos;
off64_t report_array[MAX_X][MAX_Y];
double report_darray[MAX_X][MAXSTREAMS];
double time_res,cputime_res;
long long throughput_array[MAX_X];	/* Filesize & record size are constants */
short current_x, current_y;
long long max_x, max_y;
unsigned long long goodkilos;
off64_t kilobytes64 = (off64_t)KILOBYTES;
long long reclen = RECLEN, goodrecl;
off64_t  numrecs64 = (off64_t)NUMRECS;
off64_t offset = 0;               /*offset for random I/O */
off64_t offset64 = 0;               /*offset for random I/O */
off64_t filebytes64;
char *barray[MAXSTREAMS];
char *haveshm;
extern int optind;
long long onetime, auto_mode,purge,fetchon, sfd, multi_buffer;
int fd;
int begin_proc,num_processors,ioz_processor_bind;
long long stride = STRIDE;
long long res_prob,rec_prob;
char Q_flag,silent;
char master_iozone, client_iozone,distributed;
int bif_fd;
int bif_row,bif_column;
char aflag, mflag, pflag, Eflag, hflag, oflag, Rflag, rflag, sflag;
char no_copy_flag,h_flag,k_flag,include_close,include_flush,bif_flag;
char stride_flag,gflag,nflag;
char yflag,qflag;
int direct_flag;
char async_flag;
char trflag; 
char cpuutilflag;
long long mint, maxt,depth; 
long long w_traj_ops, r_traj_ops, w_traj_fsize,r_traj_fsize;
long long r_traj_ops_completed,r_traj_bytes_completed;
long long w_traj_ops_completed,w_traj_bytes_completed;
int w_traj_items, r_traj_items;
int current_client_number;
char fflag, Uflag,uflag,lflag,OPS_flag,include_tflag,jflag; 
char disrupt_flag,r_traj_flag,w_traj_flag;
char MS_flag;
char mmapflag,mmapasflag,mmapssflag,mmapnsflag,mmap_mix;
char compute_flag;
double compute_time;
struct runtime runtimes [MAX_X] [MAX_Y];	/* in parallel with report_array[][] */
long long include_test[50];
long long include_mask;
char RWONLYflag, NOCROSSflag;		/*auto mode 2 - kcollins 8-21-96*/
char mfflag;
int file_lock;
long long status, x, y, childids[MAXSTREAMS+1], myid, num_child,delay_start,delay;
#ifndef NO_THREADS
pthread_t p_childids[MAXSTREAMS+1];
#endif
char sverify = 1;
char verify = 1;
off64_t next64;
unsigned int pattern;
char wol_opened, rol_opened;
long long chid;
FILE *wqfd,*rwqfd,*rqfd,*rrqfd;

extern char *optarg;
#ifndef __AIX__
long long ret;
#else
short ret;
#endif
struct size_entry {
	struct size_entry *next;
	off64_t size;
};
struct size_entry *size_list=0;
struct size_entry *rec_size_list=0;
off64_t maximum_file_size;
off64_t minimum_file_size;

char bif_filename [MAXNAMESIZE];           /* name of biff file      */
char filename [MAXNAMESIZE];               /* name of temporary file */
char mountname [MAXNAMESIZE];              /* name of device         */
char dummyfile [MAXSTREAMS][MAXNAMESIZE];  /* name of dummy file     */
char dummyfile1 [MAXNAMESIZE];             /* name of dummy file     */
char write_traj_filename [MAXNAMESIZE];     /* name of write telemetry file */
char read_traj_filename [MAXNAMESIZE];    /* name of read telemetry file  */
char *filearray[MAXSTREAMS];		   /* array of file names    */
char tfile[] = "iozone";
char *buffer, *mbuffer,*mainbuffer;
FILE *pi,*r_traj_fd,*w_traj_fd;
VOLATILE char *pbuffer;
char *default_filename="iozone.tmp"; /*default name of temporary file*/
VOLATILE char *stop_flag;		/* Used to stop all children */
VOLATILE char stoptime;
char xflag,Cflag;
char use_thread = 0;
long long debug1=0;		
long long debug=0;
unsigned long cache_size=CACHE_SIZE;
unsigned long cache_line_size=CACHE_LINE_SIZE;
long long *pstatus;
int no_unlink = 0;
off64_t min_file_size = KILOBYTES_START;
off64_t max_file_size = KILOBYTES_END;
long long min_rec_size = RECLEN_START;
long long max_rec_size = RECLEN_END;
long long orig_min_rec_size = RECLEN_START;
long long orig_max_rec_size = RECLEN_END;
long long xover = CROSSOVER;
char *throughput_tests[] = {"Initial write","Rewrite","Read","Re-read",
	"Reverse Read","Stride read","Random read","Random write"};
char command_line[1024] = "\0";
#ifdef unix
double sc_clk_tck;
#endif

int argcsave;
char **argvsave;
char splash[80][80];
int splash_line;
char client_filename[256];

#ifdef linux
#define REMOTE_SHELL "rsh"
#endif
#ifdef _HPUX_SOURCE
#define REMOTE_SHELL "remsh"
#endif
/* Default to rsh */
#ifndef REMOTE_SHELL
#define REMOTE_SHELL "rsh"
#endif
/* 
 * Host ports used to listen, and handle errors.
 */
#define HOST_LIST_PORT 20000
#define HOST_ESEND_PORT (HOST_LIST_PORT+MAXSTREAMS)

/* 
 * Childs ports used to listen, and handle errors.
 */
#define CHILD_ESEND_PORT (HOST_ESEND_PORT+MAXSTREAMS)
#define CHILD_LIST_PORT (CHILD_ESEND_PORT+MAXSTREAMS)

/* Childs async message port. Used for stop flag and terminate */
#define CHILD_ALIST_PORT (CHILD_LIST_PORT+MAXSTREAMS)


#define THREAD_WRITE_TEST 1
#define THREAD_REWRITE_TEST 2
#define THREAD_READ_TEST 3
#define THREAD_REREAD_TEST 4
#define THREAD_STRIDE_TEST 5
#define THREAD_RANDOM_READ_TEST 6
#define THREAD_RANDOM_WRITE_TEST 7
#define THREAD_REVERSE_READ_TEST 8
#define THREAD_CLEANUP_TEST 9

/*
 * Child states that the master is tracking.
 * The master uses these to determine how to shutdown
 * the clients when some fool hits control-C.
 */
#define C_STATE_ZERO 1
#define C_STATE_WAIT_WHO 2
#define C_STATE_WAIT_BARRIER 3

struct child_ident {
	char child_name[100];
	char workdir[256];
	char execute_path[256];
	int state;
	int child_number;
	int child_port;
	int child_async_port;
	int master_socket_num;
	int master_async_socket_num;
}child_idents[MAXSTREAMS];

int c_port,a_port;
int child_port; /* Virtualized due to fork */
int child_async_port; /* Virtualized due to fork */
int client_listen_pid; /* Virtualized due to fork */
int master_join_count;
int l_sock,s_sock,l_async_sock;
char master_rcv_buf[10240];
int master_listen_pid;
char master_send_buf[10240];
char child_rcv_buf[10240];
char child_async_rcv_buf[10240];
char child_send_buf[10240];
int child_send_socket;
int child_listen_socket;
int child_listen_socket_async;
int master_send_socket; /* Needs to be an array. One for each child*/
int master_send_sockets[MAXSTREAMS]; /* Needs to be an array. One for each child*/
int master_send_async_sockets[MAXSTREAMS]; /* Needs to be an array. One for each child*/
int master_listen_port;
int master_listen_socket;
int clients_found;
char controlling_host_name[256];
FILE *newstdin, *newstdout, *newerrout;
char toutput[20][20];
int toutputindex;
int cdebug = 0;
int mdebug = 0;

/* 
 * Prototypes
 */
void child_send();
int start_child_send();
int start_child_listen();
int start_child_listen_async();
void start_child_listen_loop();
void child_listen();
void child_listen_async();
void stop_child_send();
void stop_child_listen();

void master_send();
int start_master_send();
int start_master_listen();
void master_listen();
void stop_master_send();
void stop_master_listen();
long long start_child_proc();
int parse_client_line();
void wait_dist_join();
void tell_children_begin();
void start_master_listen_loop();
void wait_for_master_go();
void tell_master_ready();
void stop_master_listen_loop();
void tell_master_stats();
void become_client();
int pick_client();
long long start_child_proc();
int start_master_send();
void child_listen();
int start_child_listen();
void stop_master_send();
void stop_master_listen();
void stop_child_send();
void stop_child_listen();
int start_child_send();
void master_send();
void child_send();
void master_listen();
int start_master_listen();
void child_remove_files();
void terminate_child_async();
void distribute_stop();
void send_stop();
void cleanup_children();


/****************************************************************/
/*								*/
/*   MAIN () 							*/
/*								*/
/****************************************************************/

int
main(argc,argv) 
int argc;
char **argv;
{

	long long fileindx,i,tval;
	long long ind;
	int ret;
	FILE *pi;
	char reply[IBUFSIZE];
	unsigned char inp_pat;
	time_t time_run;
	char *port,*pl,*m,*subarg;
	int num_child1;
	int cret,test_fd;
	int anwser,bind_cpu,retx;
#if defined(linux)
	char *tb,*tbs;
#endif
#ifdef VXFS
	int test_foo;
#endif

	setvbuf( stdout, NULL, _IONBF, (size_t) NULL );
	setvbuf( stderr, NULL, _IONBF, (size_t) NULL );
	
	gethostname(controlling_host_name,256);
#if defined (solaris) || defined (_HPUX_SOURCE) || defined (linux) || defined(IRIX) || defined (IRIX64)
#ifndef __convex_spp
	page_size=getpagesize();
#endif
#endif
#ifdef unix
	sc_clk_tck = clk_tck();
#endif
	for(ind=0;ind<MAXSTREAMS;ind++)
		filearray[ind]=(char *)tfile;

	myid=(long long)getpid();
	get_resolution();
	time_run = time(0);
	sprintf(splash[splash_line++],"\tIozone: Performance Test of File I/O\n");
    	sprintf(splash[splash_line++],"\t%s\n\t%s\n\n", THISVERSION,MODE);
    	sprintf(splash[splash_line++],"\tContributors:William Norcott, Don Capps, Isom Crawford, Kirby Collins\n");
	sprintf(splash[splash_line++],"\t             Al Slater, Scott Rhine, Mike Wisner, Ken Goss\n");
    	sprintf(splash[splash_line++],"\t             Steve Landherr, Brad Smith, Mark Kelly, Dr. Alain CYR,\n");
    	sprintf(splash[splash_line++],"\t             Randy Dunlap.\n\n");
	sprintf(splash[splash_line++],"\tRun began: %s\n",ctime(&time_run));
	argcsave=argc;
	argvsave=argv;

    	signal(SIGINT, signal_handler);	 /* handle user interrupt */
    	signal(SIGTERM, signal_handler);	 /* handle kill from shell */

        /********************************************************/
        /* Allocate and align buffer with beginning of the 	*/
        /* on chip data cache. 					*/
        /********************************************************/

     	buffer = (char *) alloc_mem((long long)(MAXBUFFERSIZE + (2 * cache_size)));
	if(buffer == 0) {
        	perror("Memory allocation failed:");
        	exit(1);
        }

#ifdef _64BIT_ARCH_
     	buffer = (char *) ((long long )(buffer + cache_size ) & 
		~(cache_size-1));
#else
     	buffer = (char *) ((long)(buffer + cache_size ) & 
		~((long)cache_size-1));
#endif
	mainbuffer = buffer;
	fetchon++;
  	strcpy(filename,default_filename);
  	sprintf(dummyfile[0],"%s.DUMMY",default_filename);
	if(argc <=1){
		printf(USAGE);
		exit(0);
	}
	auto_mode = 0;
	inp_pat = PATTERN;
	pattern = ((inp_pat << 24) | (inp_pat << 16) | (inp_pat << 8) | inp_pat);
	while((cret = getopt(argc,argv,"ZQNIBDGCTOMREWovAxamwphcezKJ:j:k:V:r:t:s:f:F:d:l:u:U:S:L:H:+:P:i:b:X:Y:g:n:y:q: ")) != EOF){
		switch(cret){
		case 'k':	/* Async I/O with no bcopys */
			depth = (long long)(atoi(optarg));
			if(depth <0)
				depth=0;
			/*
			if(depth > 60)
				depth=60;
			*/
#ifdef NO_PRINT_LLD
			sprintf(splash[splash_line++],"\tPOSIX Async I/O (no bcopy). Depth %ld \n",depth);
#else
			sprintf(splash[splash_line++],"\tPOSIX Async I/O (no bcopy). Depth %lld \n",depth);
#endif
			no_copy_flag=1;
			async_flag++;
			k_flag++;
			break;
		case 'T':	/* switch to thread based */
#ifndef NO_THREADS
			use_thread++;
#else
			printf("\tThreads not supported in this version\n");
			exit(2);
#endif
			break;
		case 'H':	/* Use async_io */
			h_flag++;
			depth = (long long)(atoi(optarg));
			if(depth <0)
				depth=0;
			/*
			if(depth > 60)
				depth=60;
			*/
#ifdef NO_PRINT_LLD
			sprintf(splash[splash_line++],"\tPOSIX async I/O (with bcopy). Depth %ld\n",depth);
#else
			sprintf(splash[splash_line++],"\tPOSIX async I/O (with bcopy). Depth %lld\n",depth);
#endif
			async_flag++;
			break;
		case 'I':	/* Use vxfs direct advisory or O_DIRECT from Linux */
#ifdef VXFS
			direct_flag++;
			test_fd=open("vxfstest",O_CREAT|O_RDWR,0660);
			ioctl(test_fd,VX_SETCACHE,VX_DIRECT);
			ioctl(test_fd,VX_GETCACHE,&test_foo);
			unlink("vxfstest");
			if(test_foo == 0)
			{
				printf("\tVxFS advanced setcache feature not available\n");
				exit(3);
			}
			sprintf(splash[splash_line++],"\tVxFS advanced feature SET_CACHE, VX_DIRECT enabled\n");
			break;
#endif
#if defined(linux)
			tbs=tb=(char *)malloc(2*page_size);
#ifdef _64BIT_ARCH_
	     	        tb = (char *) 
			(((unsigned long long)tb + page_size ) 
				& ~(page_size-1));
#else
	     		tb = (char *) 
			(((long)tb + (long)page_size ) 
				& ~((long)page_size-1));
#endif
			test_fd=open("dxfstest",O_CREAT|O_RDWR|O_DIRECT,0660);
			if(test_fd <= 0)
			{
				printf("\tO_DIRECT feature not available\n");
				exit(3);
			}
			retx=write(test_fd,tb,4096);
			if(retx == 4096)
			{
				direct_flag++;
			}
			else
				direct_flag=0;
			free(tbs);
			unlink("dxfstest");
			if(direct_flag)
				sprintf(splash[splash_line++],"\tO_DIRECT feature enabled\n");
			else
			{
				sprintf(splash[splash_line++],"\tO_DIRECT feature not available\n");
				exit(3);
			}
				
			break;
#endif
		case 'B':	/* Use mmap file for test file */
			sprintf(splash[splash_line++],"\tUsing mmap files\n");
			mmapflag++;
			mmapnsflag++;
			break;
		case 'D':	/* Use async msync mmap file */
			sprintf(splash[splash_line++],"\tUsing msync(MS_ASYNC) on mmap files\n");
			mmapflag++;
			mmapasflag++;
			mmapnsflag=0;
			break;
		case 'G':	/* Use msync sync for mmap file */
			sprintf(splash[splash_line++],"\tUsing msync(MS_SYNC) on mmap files\n");
			mmapssflag++;
			mmapnsflag=0;
			break;
		case 'C':	/* show children xfer counts */
			Cflag++;
			break;
		case 'Q':	/* Output offset/latency files */
	    		sprintf(splash[splash_line++],"\tOffset/latency files enabled.\n");
			Q_flag++;
			break;
		case 'x':	/* disable stone_wall */
	    		sprintf(splash[splash_line++],"\tStonewall disabled\n");
			xflag++;
			break;

		case 'a':	/* auto mode */
			fetchon=1;
			purge=0;
			multi_buffer=0;
	    		auto_mode = 1;
			aflag++;
	    		sprintf(splash[splash_line++],"\tAuto Mode\n");
			break;
		case 'c':	/* Include close in timing */
			include_close++;
	    		sprintf(splash[splash_line++],"\tInclude close in write timing\n");
			break;
		case 'e':	/* Include fsync in timing */
			include_flush++;
	    		sprintf(splash[splash_line++],"\tInclude fsync in write timing\n");
			break;
		case 'A':	/* auto2 mode - kcollins 8-21-96*/
			fetchon=1;
			purge=0;
			multi_buffer=0;
	    		auto_mode = 1;
			aflag++;
	    		sprintf(splash[splash_line++],"\tAuto Mode 2. This option is obsolete. Use -az -i0 -i1 \n");
			RWONLYflag++;
			NOCROSSflag++;
			include_tflag++;	/* automatically set WRITER_TEST and READER_TEST */
			include_test[WRITER_TEST]++;
			include_test[READER_TEST]++;
			break;
		case 's': 	/* set file size */
#ifdef NO_PRINT_LLD
			sscanf(optarg,"%ld",&kilobytes64);
#else
			sscanf(optarg,"%lld",&kilobytes64);
#endif
			if(optarg[strlen(optarg)-1]=='k' ||
				optarg[strlen(optarg)-1]=='K'){
				;
			}
			if(optarg[strlen(optarg)-1]=='m' ||
				optarg[strlen(optarg)-1]=='M'){
				kilobytes64 = kilobytes64 * 1024;
			}
			if(optarg[strlen(optarg)-1]=='g' ||
				optarg[strlen(optarg)-1]=='G'){
				kilobytes64 = kilobytes64 *1024 * 1024;
			}
			if(kilobytes64 <= 0)
				kilobytes64=512;
			min_file_size = (off64_t)kilobytes64; /* Make visable globally */
			max_file_size = (off64_t)kilobytes64; /* Make visable globally */
#ifdef NO_PRINT_LLD
	    		sprintf(splash[splash_line++],"\tFile size set to %ld KB\n",kilobytes64);
#else
	    		sprintf(splash[splash_line++],"\tFile size set to %lld KB\n",kilobytes64);
#endif
			sflag++;
			break;
		case 'l': 	/* set lower thread limit  */
			mint = (long long)(atoi(optarg));
			if(mint <= 0)
			{
				mint=1;
				num_child=1;
			}else
				num_child=mint;
			if(mint > (unsigned long long)MAXSTREAMS){
			  printf("Invalid options: maximum streams for ");
			  printf("throughput is MAXSTREAMS\n");
			  exit(4);
			}
			lflag++;
			trflag++;
			if(Uflag)
			{
				printf("Can not run throughput tests with unmount & remounts.\n");
				exit(5);
			}
			break;
		case 'u': 	/* set upper thread limit  */
			maxt = (long long)(atoi(optarg));
			if(maxt <= 0)
				maxt=1;
			if(maxt > MAXSTREAMS){
			  printf("Invalid options: maximum streams for ");
			  printf("throughput is MAXSTREAMS\n");
			  exit(6);
			}
			uflag++;
			trflag++;
			if(Uflag)
			{
				printf("Can not run throughput tests with unmount & remounts.\n");
				exit(7);
			}
			break;
		case 'm':	/* use multiple buffers */
			fetchon=0;
			multi_buffer=1;
			mflag++;
     			mbuffer = (char *) alloc_mem((long long)MAXBUFFERSIZE);
			if(mbuffer == 0) {
                        	perror("Memory allocation failed:");
                          	exit(8);
			}
	    		sprintf(splash[splash_line++],"\tMulti_buffer. Work area %d bytes\n",
				MAXBUFFERSIZE);
			break;
                case 'M':       /* Report machine name and OS */
			bzero(reply,sizeof(reply));
                        pi=popen("uname -a", "r");
			if(pi == (FILE *)0)
			{
				sprintf(splash[splash_line++],"\n\tError using popen() on uname\n");
				sprintf(splash[splash_line++],"\t-M option suppressed.\n");
			}
			else
			{
                        	fread(reply,IBUFSIZE-1,1,pi);
                        	pclose(pi);
				m=reply;
                        	while(*m) /* Strip new line */
				{
					if(*m=='\n')
                               		 	*m=0;
					else	
                               		 	m++;
				}
                        	sprintf(splash[splash_line++],"\n\tMachine = %s\n",reply);
			}
                        break;

		case 'P':	/* Set beginning processor for binding. */
#ifndef NO_THREADS
#ifdef _HPUX_SOURCE
			num_processors= pthread_num_processors_np();
			begin_proc = atoi(optarg);
			if(begin_proc < 0)
				begin_proc=0;
			if(begin_proc > num_processors)
				begin_proc=0;
                        sprintf(splash[splash_line++],"\tBinding of processors beginning with %d \n",begin_proc);
			ioz_processor_bind++;
#else
			sprintf(splash[splash_line++],"\tProcessor binding not available in this version\n");
#endif
#endif
                        break;
		case 'p':	/* purge the processor cache */
	    		sprintf(splash[splash_line++],"\tPurge Mode On\n");
			fetchon=0;
			pflag++;
			purge=1;
			break;
		case 'h':	/* show help */
			hflag++;
	    		show_help();
			exit(0);
			break;
		case 'E':	/* Extended testing for pread/pwrite... */
			Eflag++;
			break;
		case 'R':	/* Generate Report */
			Rflag++;
	    		sprintf(splash[splash_line++],"\tExcel chart generation enabled\n");
			break;
		case 'o':	/* Open OSYNC */
	    		sprintf(splash[splash_line++],"\tSYNC Mode. \n");
			oflag++;
			break;
		case 'O':	/* Report in Ops/sec instead of KB/sec */
	    		sprintf(splash[splash_line++],"\tOPS Mode. Output is in operations per second.\n");
			OPS_flag++;
			break;
		case 'N':	/* Report in usec/op  */
	    		sprintf(splash[splash_line++],"\tMicroseconds/op Mode. Output is in microseconds per operation.\n");
			MS_flag++;
			break;
		case 'V':	/* Turn on Verify every byte */
			sverify=0;
			inp_pat = (char)(atoi(optarg));
			if(inp_pat == 0)
				inp_pat = PATTERN;
			pattern = ((inp_pat << 24) | (inp_pat << 16) | (inp_pat << 8) 
				| inp_pat);
			verify++;
	    		sprintf(splash[splash_line++],"\tVerify Mode. Pattern %x\n",pattern);
    			sprintf(splash[splash_line++],"\tPerformance measurements are invalid in this mode.\n");
			break;
		case 'S':	/* Set cache size */
			cache_size = (long)(atoi(optarg)*1024);
			if(cache_size == 0)
				cache_size = CACHE_SIZE;
			break;
		case 'L':	/* Set cache line size */
			cache_line_size = (long)(atoi(optarg));
			if(cache_line_size == 0)
				cache_line_size = CACHE_LINE_SIZE;
			break;
		case 'f':	/* Specify the file name */
			if(mfflag) {
			  printf("invalid options: -f and -F are mutually exclusive\n");
			  exit(10);
			}
			fflag++;
			strcpy(filename,optarg);
	  		sprintf(dummyfile[0],"%s.DUMMY",optarg);
			break;
		case 'b':	/* Specify the biff file name */
			Rflag++;
			bif_flag++;
			strcpy(bif_filename,optarg);
			break;
		case 'F':	/* Specify multiple file names for -t */
	 		mfflag++;
			if(fflag) {
			  printf("invalid options: -f and -F are mutually exclusive\n");
			  exit(11);
			}
			if(!trflag) {
			  printf("invalid options: must specify -t N before -F\n");
			  exit(12);
			}
			optind--;
			for(fileindx=0;fileindx<maxt;fileindx++) {
			  filearray[fileindx]=argv[optind++];
			  if(optind > argc) {
#ifdef NO_PRINT_LLD
			    printf("invalid options: not enough filenames for %ld streams\n",num_child);
#else
			    printf("invalid options: not enough filenames for %lld streams\n",num_child);
#endif
			    exit(13);
			  }
			}
			break;
		case 'r':	/* Specify the record size to use */
			rflag++;
			reclen = ((long long)(atoi(optarg))*1024);
			if(optarg[strlen(optarg)-1]=='k' ||
				optarg[strlen(optarg)-1]=='K'){
				reclen = (long long)(1024 * atoi(optarg));
			}
			if(optarg[strlen(optarg)-1]=='m' ||
				optarg[strlen(optarg)-1]=='M'){
				reclen = (long long)(1024 * 1024 * atoi(optarg));
			}
			if(optarg[strlen(optarg)-1]=='g' ||
				optarg[strlen(optarg)-1]=='G'){
				reclen = (long long)(1024 * 1024 * 1024 *(long long)atoi(optarg));
			}
			if(reclen <= 0)
				reclen=(long long)4096;

			max_rec_size = (off64_t)reclen;   /* Make visable globally */
			min_rec_size = (off64_t)reclen;   /* Make visable globally */
#ifdef NO_PRINT_LLD
	    		sprintf(splash[splash_line++],"\tRecord Size %ld KB\n",reclen/1024);
#else
	    		sprintf(splash[splash_line++],"\tRecord Size %lld KB\n",reclen/1024);
#endif
			break;
		case 'J':	/* Specify the compute time in millisecs */
			compute_time = (double)(atoi(optarg));
			compute_time=compute_time/1000;	
			if(compute_time < (double)0)
				compute_time=(double)0;
			else
				compute_flag=1;
			jflag++;
			break;
		case 'j':	/* Specify the stride in records */
			stride = (long long)(atoi(optarg));
			if(stride < 0)
				stride=0;
			stride_flag=1;
			break;
		case 't':	/* Specify the number of children to run */
			num_child1=(atoi(optarg));
			num_child = (long long)num_child1;
			if(num_child > (long long)MAXSTREAMS) {
			  printf("invalid options: maximum streams for throughput is MAXSTREAMS\n");
#ifdef NO_PRINT_LLD
			  printf("Numchild %ld %s\n",num_child,optarg);
#else
			  printf("Numchild %lld %s\n",num_child,optarg);
#endif
			  exit(14);
			}
			if(num_child <= 0)
				num_child = 8;
			if(num_child == 0)
				num_child=1;
			mint=maxt=num_child;
			trflag++;
			if(Uflag)
			{
				printf("Can not run throughput tests with unmount & remounts.\n");
				exit(15);
			}
			break;
		case 'd':	/* Specify the delay of children to run */
			delay_start = (long long)(atoi(optarg));
			if(delay_start < 0)
				delay_start=0;
			break;
		case 'i':	/* Specify specific tests */
			tval=(long long)(atoi(optarg));
			if(tval < 0) tval=0;
#ifndef HAVE_PREAD
			if(tval > 7)
			{
				printf("\tPread tests not available on this operating system.\n");
				exit(183);
			}
#endif
			if(tval > sizeof(func)/sizeof(char *)) 
			{
				tval=0;
				sprintf(splash[splash_line++],"\tSelected test not available on the version.\n");
			}
			include_test[tval]++;
			include_tflag++;
			break;
		case 'v':	/* Show version information */
    			for(ind=0; strlen(head1[ind]); ind++)
    			{
				printf("%s\n", head1[ind]);
    			}
			exit(0);
			break;
		case 'U':	/* Specify the dev name */
			Uflag++;
			strcpy(mountname,optarg);
			if(trflag)
			{
				printf("Can not run throughput tests with unmount & remounts.\n");
				exit(16);
			}
			break;
		case 'w':	/* Do not unlink files */
			sprintf(splash[splash_line++],"\tSetting no_unlink\n");
			no_unlink = 1;
			break;
		case 'Z':	/* Turn on the mmap and file I/O mixing */
			sprintf(splash[splash_line++],"\tEnable mmap & file I/O mixing.\n");
			mmap_mix = 1;
			break;
		case 'W':	/* Read/Write with file locked */
			file_lock=1;
			sprintf(splash[splash_line++],"\tLock file when reading/writing.\n");
			break;
		case 'K':	/* Cause disrupted read pattern */
			disrupt_flag=1;
			sprintf(splash[splash_line++],"\tDisrupted read patterns selected.\n");
			break;
		case 'X':	/* Open write telemetry file */
			compute_flag=1;
			sverify=2;  /* touch lightly */
			w_traj_flag=1;
			strcpy(write_traj_filename,optarg);
			traj_vers();
			w_traj_size();
			sprintf(splash[splash_line++],"\tUsing write telemetry file \"%s\"\n",
				write_traj_filename);
			w_traj_fd=open_w_traj();
			if(w_traj_fd == (FILE *)0)
				exit(200);
			break;
		case 'Y':	/* Open Read telemetry file */
			compute_flag=1;
			sverify=2;  /* touch lightly */
			r_traj_flag=1;
			strcpy(read_traj_filename,optarg);
			sprintf(splash[splash_line++],"\tUsing read telemetry file \"%s\"\n",
				read_traj_filename);
			traj_vers();
			r_traj_size();
			r_traj_fd=open_r_traj();
			if(r_traj_fd == (FILE*) 0)
				exit(200);
			break;
		case 'n':	/* Set min file size for auto mode */
			nflag=1;
			minimum_file_size = (off64_t)atoi(optarg);
			if(optarg[strlen(optarg)-1]=='k' ||
				optarg[strlen(optarg)-1]=='K'){
				;
			}
			if(optarg[strlen(optarg)-1]=='m' ||
				optarg[strlen(optarg)-1]=='M'){
				minimum_file_size = (long long)(1024 * atoi(optarg));
			}
			if(optarg[strlen(optarg)-1]=='g' ||
				optarg[strlen(optarg)-1]=='G'){
				minimum_file_size = (long long)(1024 * 1024 * (long long)atoi(optarg));
			}
			if(minimum_file_size <= KILOBYTES_START)
				minimum_file_size=(off64_t)KILOBYTES_START;
#ifdef NO_PRINT_LLD
			sprintf(splash[splash_line++],"\tUsing minimum file size of %ld kilobytes.\n",minimum_file_size);
#else
			sprintf(splash[splash_line++],"\tUsing minimum file size of %lld kilobytes.\n",minimum_file_size);
#endif
			break;
		case 'g':	/* Set maximum file size for auto mode */
			gflag=1;
			maximum_file_size = (off64_t)atoi(optarg);
			if(optarg[strlen(optarg)-1]=='k' ||
				optarg[strlen(optarg)-1]=='K'){
				;
			}
			if(optarg[strlen(optarg)-1]=='m' ||
				optarg[strlen(optarg)-1]=='M'){
				maximum_file_size = (long long)(1024 * atoi(optarg));
			}
			if(optarg[strlen(optarg)-1]=='g' ||
				optarg[strlen(optarg)-1]=='G'){
				maximum_file_size = (long long)(1024 * 1024 * (long long)atoi(optarg));
			}
			if(maximum_file_size <= KILOBYTES_START)
				maximum_file_size=(off64_t)KILOBYTES_END;
#ifdef NO_PRINT_LLD
			sprintf(splash[splash_line++],"\tUsing maximum file size of %ld kilobytes.\n",maximum_file_size);
#else
			sprintf(splash[splash_line++],"\tUsing maximum file size of %lld kilobytes.\n",maximum_file_size);
#endif
			break;
		case 'z':	/* Set no cross over */
			sprintf(splash[splash_line++],"\tCross over of record size disabled.\n");
			NOCROSSflag=1;
			break;
		case 'y':		/* set min record size for auto mode */
			yflag=1;
			min_rec_size = ((long long)(atoi(optarg))*1024);
			if(optarg[strlen(optarg)-1]=='k' ||
				optarg[strlen(optarg)-1]=='K'){
				min_rec_size = (long long)(1024 * atoi(optarg));
			}
			if(optarg[strlen(optarg)-1]=='m' ||
				optarg[strlen(optarg)-1]=='M'){
				min_rec_size = (long long)(1024 * 1024 * atoi(optarg));
			}
			if(optarg[strlen(optarg)-1]=='g' ||
				optarg[strlen(optarg)-1]=='G'){
				min_rec_size = (long long)(1024 * 1024 * 1024 *(long long)atoi(optarg));
			}
			if(min_rec_size <= 0)
				min_rec_size=(long long)RECLEN_START;
#ifdef NO_PRINT_LLD
	    		sprintf(splash[splash_line++],"\tUsing Minimum Record Size %ld KB\n", min_rec_size/1024);
#else
	    		sprintf(splash[splash_line++],"\tUsing Minimum Record Size %lld KB\n", min_rec_size/1024);
#endif
			break;
		case 'q':		/* set max record size for auto mode */
			qflag=1;
			max_rec_size = ((long long)(atoi(optarg))*1024);
			if(optarg[strlen(optarg)-1]=='k' ||
				optarg[strlen(optarg)-1]=='K'){
				max_rec_size = (long long)(1024 * atoi(optarg));
			}
			if(optarg[strlen(optarg)-1]=='m' ||
				optarg[strlen(optarg)-1]=='M'){
				max_rec_size = (long long)(1024 * 1024 * atoi(optarg));
			}
			if(optarg[strlen(optarg)-1]=='g' ||
				optarg[strlen(optarg)-1]=='G'){
				max_rec_size = (long long)(1024 * 1024 * 1024 *(long long)atoi(optarg));
			}
			if(max_rec_size <= 0)
				min_rec_size=(long long)RECLEN_END;
			if(max_rec_size > MAXBUFFERSIZE) {
#ifdef NO_PRINT_LLD
				printf("Error: maximum record size %ld KB is greater than maximum buffer size %ld KB\n ",
					max_rec_size/1024, MAXBUFFERSIZE/1024);
#else
				printf("Error: maximum record size %lld KB is greater than maximum buffer size %lld KB\n ",
					(long long)(max_rec_size/1024LL), (long long)MAXBUFFERSIZE/1024LL);
#endif
				exit(23);
			}
#ifdef NO_PRINT_LLD
			sprintf(splash[splash_line++],"\tUsing Maximum Record Size %ld KB\n", max_rec_size/1024);
#else
			sprintf(splash[splash_line++],"\tUsing Maximum Record Size %lld KB\n", max_rec_size/1024);
#endif
			break;
			/* 
			 * The + operator is for the new extended options mechanism 
			 * Syntax is -+ followed by option leter, and if the optino
			 * takes an operand  then it is implemented below. An example
			 * -+a arg    is shown below. This is a sub option with an argument.
			 * -+b  is shown below. This is a sub option with no argument.
			 */
		case '+':
			/* printf("Plus option = >%s<\n",optarg);*/
			switch (*((char *)optarg))
			{
				case 'a':  /* Has argument */
					subarg=argv[optind++];
					/* if(subarg!=(char *)0)   Error checking. */
					/* printf("Plus option argument = >%s<\n",subarg);*/
					break;
				case 'b':  /* Does not have an argument */
					break;
				case 'c':  /* Argument is the controlling host name */
					/* I am a client for distributed Iozone */
					subarg=argv[optind++];
					if(subarg==(char *)0)
					{
					     printf("-+c takes an operand !!\n");
					     exit(200);
					}
					strcpy(controlling_host_name,subarg);
					distributed=1;
					client_iozone=1;
					master_iozone=0;
					break;
				case 'm':  /* Does not have an argument */
					/* I am the controlling process for distributed Iozone */
					subarg=argv[optind++];
					if(subarg==(char *)0)
					{
					     printf("-+m takes an operand. ( filename )\n");
					     exit(201);
					}
					strcpy(client_filename,subarg);
					ret=get_client_info();
					if(ret <=0)
					{
						printf("Error reading client file\n");
						exit(178);
					}
					clients_found=ret;
					distributed=1;
					master_iozone=1;
					client_iozone=0;
					sprintf(splash[splash_line++],"\tNetwork distribution mode enabled.\n");
					break;
				case 'u':	/* set CPU utilization output flag */
					cpuutilflag = 1;	/* only used if R(eport) flag is also set */
					get_rusage_resolution();
    					sprintf(splash[splash_line++],"\tCPU utilization Resolution = %5.3f seconds.\n",cputime_res);
	    				sprintf(splash[splash_line++],"\tCPU utilization Excel chart enabled\n");
					break;
				case 's':  /* Does not have an argument */
					silent=1;
					break;
				default:
					printf("Unsupported Plus option -> %s <-\n",optarg);
					exit(0);
					break;
			}	
			break;
		}
	}
	for(i=0;i<splash_line;i++)
		if(!silent) printf("%s",splash[i]);
	record_command_line(argcsave, argvsave);
	if(pflag) /* Allocate after cache_size is set */
	{
     		pbuffer = (char *) alloc_mem((long long)(3 * cache_size));
		if(pbuffer == 0) {
                       	perror("Memory allocation failed:");
                       	exit(9);
		}
#ifdef _64BIT_ARCH_
	     	pbuffer = (char *) 
			(((unsigned long long)pbuffer + cache_size ) 
				& ~(cache_size-1));
#else
	     	pbuffer = (char *) 
			(((long)pbuffer + (long)cache_size ) 
				& ~((long)cache_size-1));
#endif
	}
	if(distributed && master_iozone)
	{
		if(maxt > clients_found)
		{
			printf("You can not specify more threads/processes than you have in the client file list\n");
			exit(202);
		}
	}
	
	if(distributed && master_iozone && cpuutilflag)
	{
		printf("\n\t>>> WARNING <<< CPU utilization not supported in distributed mode.\n");
		printf("\tRunning with CPU utilization disabled.\n\n");
		cpuutilflag=0;

	}
	if(!OPS_flag && !MS_flag)
	{
		if(!silent) printf("\tOutput is in Kbytes/sec\n");
	}
	if (min_rec_size > max_rec_size) {
#ifdef NO_PRINT_LLD
		printf("Error: minimum record size %ld KB is greater than maximum record size %ld KB\n ",
			min_rec_size/1024, max_rec_size/1024);
#else
		printf("Error: minimum record size %lld KB is greater than maximum record size %lld KB\n ",
			min_rec_size/1024, max_rec_size/1024);
#endif
		exit(23);
        }
	orig_min_rec_size=min_rec_size;
	orig_max_rec_size=max_rec_size;
	/*
 	 * No telemetry files... just option selected 
	 */
	if(compute_flag && jflag  && !(r_traj_flag || w_traj_flag))
		if(!silent) printf("\tCompute time %f seconds for reads and writes.\n",compute_time);
	/*
 	 * Read telemetry file and option selected
	 */
	if(compute_flag && r_traj_flag && !w_traj_flag)
	{
		if(r_traj_items==3)
		{
			if(!silent) printf("\tCompute time from telemetry files for reads.\n");
		}
		else
		{
			if(jflag)
				if(!silent) printf("\tCompute time %f seconds for reads.\n",compute_time);
		}
		if(jflag)
			if(!silent) printf("\tCompute time %f seconds for writes.\n",compute_time);
	}
	/*
 	 * Write telemetry file and option selected
	 */
	if(compute_flag && !r_traj_flag && w_traj_flag)
	{
		if(w_traj_items==3)
			if(!silent) printf("\tCompute time from telemetry files for writes.\n");
		else
		{
			if(jflag)
				if(!silent) printf("\tCompute time %f seconds for writes.\n",compute_time);
		}
		if(jflag)
			if(!silent) printf("\tCompute time %f seconds for reads.\n",compute_time);
	}
	if(compute_flag && r_traj_flag && w_traj_flag && jflag)
	{
		if(r_traj_items==3)
			if(!silent) printf("\tCompute time from telemetry files for reads.\n");
		else
			if(!silent) printf("\tCompute time %f seconds for reads.\n",compute_time);
		if(w_traj_items==3) 
			if(!silent) printf("\tCompute time from telemetry files for writes.\n");
		else
			if(!silent) printf("\tCompute time %f seconds for writes.\n",compute_time);
	}
	if(compute_flag && r_traj_flag && w_traj_flag && !jflag)
	{
		if(r_traj_items==3)
			if(!silent) printf("\tCompute time from telemetry files for reads.\n");
		else
			if(!silent) printf("\tNo compute time for reads.\n");

		if(w_traj_items==3) 
			if(!silent) printf("\tCompute time from telemetry files for writes.\n");
		else
			if(!silent) printf("\tNo compute time for writes.\n");
	}

	/* Enforce only write,rewrite,read,reread */
	if(w_traj_flag || r_traj_flag)
	{
		for(i=2;i<sizeof(func)/sizeof(char *);i++)
			include_test[i] = 0;
	}
	if(r_traj_flag)
	{
		if(include_test[READER_TEST] == 0) 
		{
			include_test[WRITER_TEST]=1;
			include_test[READER_TEST]=1;
			include_tflag=1;
		}
	}
	if(w_traj_flag)
	{
		if(include_test[WRITER_TEST] == 0) 
		{
			include_test[WRITER_TEST]=1;
			include_tflag=1;
		}
	}
	if(w_traj_flag && w_traj_fsize != 0)
		kilobytes64=w_traj_fsize/1024;
	if(r_traj_flag && r_traj_fsize != 0)
		kilobytes64=r_traj_fsize/1024;

	if( sverify==0 && (w_traj_flag || r_traj_flag))
	{
		printf("\n\tFull verification not supported in telemetry mode.\n\n");
		exit(17);
	}
	;
	if(disrupt_flag &&(w_traj_flag || r_traj_flag) )
	{
		printf("\n\tDisrupt not supported in telemetry mode.\n\n");
		exit(17);
	}
	if(aflag &&(w_traj_flag || r_traj_flag) )
	{
		printf("\n\tAuto mode not supported in telemetry mode.\n");
		printf("\tTry:   -i 0 -i 1 \n\n");
		exit(17);
	}
	if(sflag && w_traj_flag )
	{
		printf("\n\tSize of file is determined by telemetry file.\n\n");
		exit(17);
	}
	if(rflag && w_traj_flag )
	{
		printf("\n\tRecord size of file is determined by telemetry file.\n\n");
		exit(17);
	}
	if(stride_flag && (w_traj_flag || r_traj_flag))
	{
		printf("\n\tStride size is determined by telemetry file.\n\n");
		exit(17);
	}
	if(trflag && MS_flag)
	{
		printf("\n\tMicrosecond mode not supported in throughput mode.\n\n");
		exit(17);
	}
	if (trflag	/* throughput mode, don't allow auto-mode options: */
		&& (auto_mode || aflag || yflag || qflag || nflag || gflag))
	{
		printf("\n\tCan not mix throughput mode and auto-mode flags.\n\n");
		exit(17);
	}
	if(async_flag && mmapflag)
	{
		printf("\n\tSorry ... Only mmap or async but not both\n\n");
		exit(18);
	}
#ifndef ASYNC_IO
	if(async_flag)
	{
		printf("\n\tSorry ... This version does not support async I/O\n\n");
		exit(19);
	}
#endif
	if(include_tflag)
	{
		for(i=0;i<sizeof(func)/sizeof(char *);i++)
			if(include_test[i])
				include_mask|=(1<<i);
			/*printf("%x",include_mask); */
	}

	if(h_flag && k_flag)
	{
		printf("\n\tCan not do both -H and -k\n");
		exit(20);
	}
		
	if(!aflag && !rflag)
		max_rec_size=min_rec_size;

	init_record_sizes(min_rec_size,max_rec_size);
    	if(!silent) printf("\tTime Resolution = %1.6f seconds.\n",time_res);
#ifdef NO_PRINT_LLD
    	if(!silent) printf("\tProcessor cache size set to %ld Kbytes.\n",cache_size/1024);
    	if(!silent) printf("\tProcessor cache line size set to %ld bytes.\n",cache_line_size);
	if(!silent) printf("\tFile stride size set to %ld * record size.\n",stride);
#else
    	if(!silent) printf("\tProcessor cache size set to %ld Kbytes.\n",cache_size/1024);
    	if(!silent) printf("\tProcessor cache line size set to %ld bytes.\n",cache_line_size);
	if(!silent) printf("\tFile stride size set to %lld * record size.\n",stride);
#endif
	if(!rflag)
		reclen=(long long)4096;

	if(uflag && !lflag)
		num_child=mint = 1;
	if(lflag && !uflag)
		maxt = 8;
	if(use_thread)
		port="thread";
	else
		port="process";
	if(lflag || uflag){
#ifdef NO_PRINT_LLD
		if(!silent) printf("\tMin %s = %ld \n",port,mint);
		if(!silent) printf("\tMax %s = %ld \n",port,maxt);
#else
		if(!silent) printf("\tMin %s = %lld \n",port,mint);
		if(!silent) printf("\tMax %s = %lld \n",port,maxt);
#endif
	}
	if(trflag)
	{
		if(num_child > 1)
			if(use_thread)
				port="threads";
			else
				port="processes";

#ifdef NO_PRINT_LLD
		if(!silent) printf("\tThroughput test with %ld %s\n", num_child,port);
#else
		if(!silent) printf("\tThroughput test with %lld %s\n", num_child,port);
#endif
	}
        numrecs64 = (long long)(kilobytes64*1024)/reclen;
        if (reclen >  (long long)MAXBUFFERSIZE) {
#ifdef NO_PRINT_LLD
                printf("Error: Maximum record length is %ld bytes\n",
                                MAXBUFFERSIZE);
#else
                printf("Error: Maximum record length is %lld bytes\n",
                                MAXBUFFERSIZE);
#endif
                exit(21);
        }
        if (reclen < (long long)MINBUFFERSIZE) {
#ifdef NO_PRINT_LLD
                printf("Error: Minimum record length is %ld bytes\n",
                                MINBUFFERSIZE);
#else
                printf("Error: Minimum record length is %lld bytes\n",
                                MINBUFFERSIZE);
#endif
                exit(22);
        }
        if (reclen > (long long)(kilobytes64*1024)) {
#ifdef NO_PRINT_LLD
            printf("Error: record length %ld is greater than filesize %ld KB\n ",
                                reclen,kilobytes64);
#else
            printf("Error: record length %lld is greater than filesize %lld KB\n ",
                                reclen,kilobytes64);
#endif
                exit(23);
        }
	/* Only bzero or fill that which you will use. The buffer is very large */
	if(verify )	
	{
		fill_buffer((char *)buffer,l_min(reclen,(long long)cache_size),(long long)pattern,(char)sverify);
		if(pflag)
			fill_buffer((char *)pbuffer,l_min(reclen,(long long)cache_size),(long long)pattern,(char)sverify);
		if(mflag)
			fill_buffer((char *)mbuffer,l_min(reclen,(long long)cache_size),(long long)pattern,(char)sverify);
	}
	else
	{
		bzero(buffer,(size_t)l_min(reclen,(long long)cache_size));
	}

#ifndef NO_THREADS
#ifdef _HPUX_SOURCE
	if(ioz_processor_bind)
	{
		 bind_cpu=begin_proc;
		 pthread_processor_bind_np(PTHREAD_BIND_FORCED_NP,
                         (pthread_spu_t *)&anwser, (pthread_spu_t)bind_cpu, pthread_self());
		my_nap(40);	/* Switch to new cpu */
	}
#endif
#endif
	if(trflag){
	    (void)multi_throughput_test(mint,maxt);
	    goto out;
	}
	if(trflag && (mint == maxt)){
		auto_mode=0;
		throughput_test();
	    	goto out;
	}
    	if (aflag) {
		print_header();
		auto_test();
		goto out;
	} 
	print_header();
	(void) begin(kilobytes64,reclen);
out:
	if(r_traj_flag)
		fclose(r_traj_fd);
	if(w_traj_flag)
		fclose(w_traj_fd);
	if (!no_unlink)
	      	unlink(dummyfile[0]);	/* delete the file */
	if(!silent) printf("\niozone test complete.\n");
	if(res_prob)
	{
		printf("Timer resolution is poor. Some small transfers may have \n");
		printf("reported inaccurate results. Sizes %d Kbytes and below.\n",
			(long)rec_prob/1024);
	}

	if(Rflag && !trflag){
		dump_excel();
	}
	return(0);	
}

#ifdef HAVE_ANSIC_C
void
record_command_line(int argc, char **argv)
#else
void
record_command_line(argc, argv)
int argc;
char **argv;
#endif
{
	int ix, len = 0;

	/* print and save the entire command line */
	if(!silent) printf("\tCommand line used:");
	for (ix=0; ix < argc; ix++) {
		if(!silent) printf(" %s", argv[ix]);
		if ((len + strlen(argv[ix])) < sizeof(command_line)) {
			strcat (command_line, argv[ix]);
			strcat (command_line, " ");
			len += strlen(argv[ix]) + 1;
		}
		else {
			printf ("Command line too long to save completely.\n");
			break;
		}
	}
	if(!silent) printf("\n");
}

/*************************************************************************/
/* BEGIN()								 */
/* This is the main work horse. It is called from main and from 	 */
/* auto_test. The caller provides the size of file and the record length.*/
/*************************************************************************/
#ifdef HAVE_ANSIC_C
void 
begin(off64_t kilos64,long long reclength)
#else
void
begin(kilos64,reclength)
off64_t kilos64;
long long reclength;
#endif
{
	long long num_tests,test_num,i,j;
	long long data1[MAXTESTS], data2[MAXTESTS];
	long long tmp;
	num_tests = sizeof(func)/sizeof(char *);
#ifdef HAVE_PREAD
	if(!Eflag)
	{
		num_tests -= 4;
		if(mmapflag || async_flag)
		{
			num_tests -= 2;
		}
	}
	else
	{
		if(mmapflag || async_flag)
			num_tests -= 6;
	}
#else
	if(mmapflag || async_flag)
	{
		num_tests -= 2;
	}
#endif

	if(RWONLYflag) num_tests = 2;		/* kcollins 8-21-96*/
	sync(); 		/* just in case there were some dirty */
	sync();
	kilobytes64=kilos64;
	reclen=reclength;
	numrecs64 = (kilobytes64*1024)/reclen;
	store_value(kilobytes64);
	if(r_traj_flag || w_traj_flag)
		store_value((off64_t)0);
	else	
		store_value((off64_t)(reclen/1024));

#ifdef NO_PRINT_LLD
	if(!silent) printf("%16ld",kilobytes64);
	if(r_traj_flag || w_traj_flag)
		if(!silent) printf("%8ld",0);
	else
		if(!silent) printf("%8ld",reclen/1024);
#else
	if(!silent) printf("%16lld",kilobytes64);
	if(r_traj_flag || w_traj_flag)
	{
		if(!silent) printf("%8lld",(long long )0);
	}
	else
	{
		if(!silent) printf("%8lld",reclen/1024);
	}
#endif
	if(include_tflag)
	{
		for(i=0;i<num_tests;i++)
		{
			if(include_mask & (1<<i))
			   func[i](kilobytes64,reclen,&data1[i],&data2[i]);
			else
			{
			       	if(!silent) printf("%s",test_output[i]); 
				fflush(stdout);
				for(j=0;j<test_soutput[i];j++)
					store_value((off64_t)0);
			}
		}
	}
	else
	{
		for(test_num=0;test_num < num_tests;test_num++)
		{
			func[test_num](kilobytes64,reclen,&data1[test_num],&data2[test_num]);
		};
	}
	if(!silent) printf("\n");
	if(!OPS_flag && !include_tflag){			/* Report in ops/sec ? */
	   if(data1[1]!=0 && data2[1] != 0)
	   {   
		totaltime = data1[1] + data1[0];
		if (totaltime < TOOFAST) 
		{
		   	goodkilos = (TOOFAST/totaltime)*2*kilobytes64;
		    	printf("\nThe test completed too quickly to give a good result\n");
		    	printf("You will get a more precise measure of this machine's\n");
		    	printf("performance by re-running iozone using the command:\n");
		    	printf("\n\tiozone %ld ", goodkilos);
#ifdef NO_PRINT_LLD
		    	printf("\t(i.e., file size = %ld kilobytes64)\n", goodkilos);
#else
		    	printf("\t(i.e., file size = %lld kilobytes64)\n", goodkilos);
#endif
		}
	   } else {
	    	goodrecl = reclen/2;
	    	printf("\nI/O error during read.  Try again with the command:\n");
#ifdef NO_PRINT_LLD
	    	printf("\n\tiozone %lld %ld ", kilobytes64,  goodrecl);
#else
	    	printf("\n\tiozone %ld %ld ", kilobytes64,  goodrecl);
#endif
	    	printf("\t(i.e. record size = %ld bytes)\n",  goodrecl);
	   }
	}
	if (!no_unlink)
    		unlink(filename);	/* delete the file */
					/*stop timer*/
	return ;
}
/******************************************************************

    SHOW_HELP -- show development help of this program

******************************************************************/
#ifdef HAVE_ANSIC_C
void show_help(void)
#else
void show_help()
#endif
{
    	long long i;
    	if(!silent) printf("iozone: help mode\n\n");
    	for(i=0; strlen(help[i]); i++)
    	{
		if(!silent) printf("%s\n", help[i]);
    	}
}
/******************************************************************

    SIGNAL_HANDLER -- clean up if user interrupts the program

******************************************************************/
#ifdef HAVE_ANSIC_C
void signal_handler(void)
#else
void signal_handler()
#endif
{
	long long i;
	if(distributed)
	{
		if(master_iozone)
			cleanup_children();
	}
	if((long long)getpid()==myid)
	{
    		if(!silent) printf("\niozone: interrupted\n\n");
#ifndef VMS
		if (!no_unlink)
    			unlink(filename);	/* delete the file */
		for(i=1;i<num_child;i++)
    			unlink(dummyfile[i]);	/* delete the file */
		if (!no_unlink)
    			unlink(dummyfile[0]);	/* delete the file */
		
#endif
		if(Rflag && !trflag){
			dump_excel();
		}
		if(Rflag && trflag){
			dump_throughput();
		}

	    	if(!silent) printf("exiting iozone\n\n");
		if(res_prob)
		{
			printf("Timer resolution is poor. Some small transfers may have \n");
			printf("reported inaccurate results. Sizes %d Kbytes and below.\n",
				(long)rec_prob/1024);
		}
		if(trflag && !use_thread)
		   for(i=0;i<num_child;i++)
			kill((pid_t)childids[i],SIGTERM);
		if(r_traj_flag)
			fclose(r_traj_fd);
		if(w_traj_flag)
			fclose(w_traj_fd);
	}
    	exit(0);
}

/****************************************************************/
/*								*/
/*   AUTO_TEST -- perform series of tests and tabulate results  */
/*								*/
/****************************************************************/
#ifdef HAVE_ANSIC_C
void 
auto_test(void)
#else
void auto_test()
#endif
{
    	off64_t kilosi;
	long long recszi,count1;
	long long mult,save_y=0;
	long long xx;

	/****************************************************************/
	/* Start with file size of 1 megabyte and repeat the test 	*/
	/* KILOBYTES_ITER_LIMIT  					*/
	/* times.  Each time we run, the file size is doubled		*/
	/****************************************************************/
/*
        if(sflag) {
          min_file_size = kilobytes64;
          max_file_size = kilobytes64;
        }
        if(rflag) {
          min_rec_size = reclen;
          max_rec_size = reclen;
        }
*/
	if(gflag)
		max_file_size = maximum_file_size;
	if(nflag)
		min_file_size = minimum_file_size;

	if(NOCROSSflag) xover = max_file_size;

	init_file_sizes(min_file_size, max_file_size);
	del_record_sizes();
	orig_min_rec_size=min_rec_size;
	orig_max_rec_size=max_rec_size;
	init_record_sizes(min_rec_size, max_rec_size);




        for(kilosi=get_next_file_size((off64_t)0); kilosi>0; kilosi=get_next_file_size(kilosi))
        {
	/****************************************************************/
	/* Start with record size of min_rec_size bytes and repeat the 	*/
	/* test, multiplying the record size by MULTIPLIER each time, 	*/
	/* until we reach max_rec_size.	 At the CROSSOVER we stop doing */
	/* small buffers as it takes forever and becomes very 		*/
	/* un-interesting.						*/
	/****************************************************************/
             if(!rflag && !sflag )
             	if(kilosi > xover){
                	min_rec_size = LARGE_REC;
			mult = orig_min_rec_size/1024;
			del_record_sizes();
			init_record_sizes(min_rec_size, max_rec_size);
		     	/************************************/
			/* Generate dummy entries in the    */
			/* Excel buffer for skipped         */
			/* record sizes			 */
			/************************************/
			for(count1=min_rec_size;
			     	(count1 != orig_min_rec_size) && (
				     	mult <= (kilosi*1024)) ;
						count1=(count1>>1))
			{
			    	current_x=0;
			     	store_value((off64_t)kilosi);
			     	store_value((off64_t)mult);
			     	for(xx=0;xx<20;xx++)
			     		store_value((off64_t)0);
			     	mult=mult*2;
			     	current_y++;
			     	if(current_y>max_y)
			     		max_y=current_y;
			     	current_x=0;
			}
	     }

             for (recszi=get_next_record_size((off64_t)0);recszi!=0;recszi=get_next_record_size(recszi))
             {
                     if(recszi > (kilosi*1024)) 
			break;
                     begin(kilosi, recszi );
		     current_x=0;
		     current_y++;
             }
	}
}


/****************************************************************/
/*								*/
/* THROUGHPUT_TEST () Multi process throughput test		*/
/*								*/
/* Note: States for share memory barrier are:			*/
/* 0 = Child not running or has finished.			*/
/* 1 = Child is ready to begin.					*/
/* 2 = Child is told to begin.					*/
/****************************************************************/
/* Data in shared memory format is:				*/
/*								*/
/* struct child_stats {						*/
/* 	long long flag; 	Used to barrier			*/
/*	double walltime;	Child's elapsed time		*/
/*	double cputime;		Child's CPU time		*/
/* 	double throughput;	Child's throughput		*/
/* 	double actual;		Child's actual read/written	*/
/*	double start_time;	Actual start time		*/
/*	double stop_time;	Actual end time			*/
/*	double fini_time;	Ending time for a child		*/
/* } 								*/
/*								*/
/* There is an array of child_stat structures layed out in 	*/
/* shared memory.						*/
/*								*/
/****************************************************************/

#ifdef HAVE_ANSIC_C
void 
throughput_test(void)
#else
void 
throughput_test()
#endif
{
	char *unit;
	char *stackp;
	double starttime1 = 0;
	double jstarttime = 0;
	double jtime = 0;
	double walltime = 0;
	double cputime = 0;
	double time_begin, time_fini;
	char *port;
	char getout;
	long long throughsize = KILOBYTES;
	long long xx,xy,i;
	long long flags,xyz;
	double ptotal;
	long long recs_per_buffer;
	long long buffer_size = MAXBUFFERSIZE;
	off64_t written_so_far, read_so_far, re_written_so_far,re_read_so_far;
	long tx;
	long long stopped;
	VOLATILE char *temp;
	double min_throughput = 0;
	double max_throughput = 0;
	double avg_throughput = 0;
	double min_xfer = 0; 


	toutputindex=0;
	strcpy(&toutput[0][0],throughput_tests[0]);
	ptotal=written_so_far=read_so_far=re_written_so_far=re_read_so_far=0 ;

	if(OPS_flag)
		unit="ops";
	else
		unit="KB";

	if(!haveshm)
	{
		shmaddr=(struct child_stats *)alloc_mem((long long)SHMSIZE);
#ifdef _64BIT_ARCH_
		if((long long)shmaddr==(long long)-1)
#else
		if((long )shmaddr==(long)-1)
#endif
		{
			printf("\nShared memory not working\n");
			exit(24);
		}
		haveshm=(char*)shmaddr;
	}
	else
		shmaddr=(struct child_stats *)haveshm;

	if(use_thread)
		stop_flag = &stoptime;
	else
	{
		temp = (char *)&shmaddr[0];
		stop_flag = (char *)&temp[(long long)SHMSIZE]-4;
	}
	for(xyz=0;xyz<num_child;xyz++){ /* all children to state 0 (HOLD) */
		child_stat = (struct child_stats *)&shmaddr[xyz];
		child_stat->flag=CHILD_STATE_HOLD;
		child_stat->actual=0;
		child_stat->throughput=0;
		child_stat->start_time=0;
		child_stat->stop_time=0;
		child_stat->fini_time=0;
		child_stat->cputime=0.0;
		child_stat->walltime=0.0;
	}
	*stop_flag = 0;
	if(!sflag)
		kilobytes64=throughsize;
	if(!rflag)
        	reclen=(long long)4096;
        numrecs64 = (long long)(kilobytes64*1024)/reclen;
	buffer=mainbuffer;
	if(use_thread)
		port="thread";
	else
		port="process";
	if(w_traj_flag)
	{
#ifdef NO_PRINT_LLD
	if(!silent) printf("\tEach %s writes a %ld Kbyte file in telemetry controlled records\n",
		port,kilobytes64);
#else
	if(!silent) printf("\tEach %s writes a %lld Kbyte file in telemetry controlled records\n",
		port,kilobytes64);
#endif
	}
	else
	{
#ifdef NO_PRINT_LLD
	if(!silent) printf("\tEach %s writes a %ld Kbyte file in %ld Kbyte records\n",
		port,kilobytes64,reclen/1024);
#else
	if(!silent) printf("\tEach %s writes a %lld Kbyte file in %lld Kbyte records\n",
		port,kilobytes64,reclen/1024);
#endif
	}

	if(fflag)		/* Each child has a file name to write */
	  for(xx=0;xx<num_child;xx++)
	    filearray[xx] = filename;
	myid = (long long)getpid();

	/* rags: skip writer test */
	if(include_tflag)
		if(!(include_mask & WRITER_MASK))
			goto next0;

	/* Hooks to start the distributed Iozone client/server code */
	if(distributed)
	{
		use_thread=0;  /* Turn of any Posix threads */
		if(master_iozone)
			master_listen_socket = start_master_listen();
		else
			become_client();
	}
	if(!use_thread)
	{
	   for(xx = 0; xx< num_child ; xx++){	/* Create the children */
		chid=xx;
		childids[xx] = start_child_proc(THREAD_WRITE_TEST,numrecs64,reclen);
		if(childids[xx]==-1){
			printf("\nFork failed\n");
			for(xy = 0; xy< xx ; xy++){
				if(!use_thread)
					kill((pid_t)childids[xy],SIGTERM);
			}
			exit(25);
		}
		if(childids[xx]!=0 && debug1)
#ifdef NO_PRINT_LLD
			printf("Parent starting slot %ld\n",xx);	
#else
			printf("Parent starting slot %lld\n",xx);	
#endif
		if( childids[xx] == 0 ){
#ifdef _64BIT_ARCH_
		  thread_write_test((void *)xx);
#else
		  thread_write_test((void *)(long)xx);
#endif
		}else {
#ifdef NO_PRINT_LLD
		  sprintf(dummyfile[xx],"%s.DUMMY.%ld",filearray[xx], xx);
#else
		  sprintf(dummyfile[xx],"%s.DUMMY.%lld",filearray[xx], xx);
#endif
		}
	   }
	}
#ifndef NO_THREADS
	else
	{
	   for(xx = 0; xx< num_child ; xx++){	/* Create the children */
#ifdef NO_PRINT_LLD
		sprintf(dummyfile[xx],"%s.DUMMY.%ld",filearray[xx], xx);
#else
		sprintf(dummyfile[xx],"%s.DUMMY.%lld",filearray[xx], xx);
#endif
		if(!barray[xx])
		{
			barray[xx]=(char *) alloc_mem((long long)(MAXBUFFERSIZE+cache_size));
			if(barray[xx] == 0) {
        		   perror("Memory allocation failed:");
        		   exit(26);
        		}
     			barray[xx] =(char *)(((long)barray[xx] + cache_size ) & 
			~(cache_size-1));
		}

#ifdef _64BIT_ARCH_
		childids[xx] = mythread_create(thread_write_test,(void*)xx);
#else
		childids[xx] = mythread_create(thread_write_test,(void*)(long)xx);
#endif
		if(childids[xx]==-1){
			printf("Thread create failed\n");
			for(xy = 0; xy< xx ; xy++){
				kill((pid_t)myid,SIGTERM);
			}
			exit(27);
		}
	   }
	}
#endif
	if((long long)getpid() == myid)
	{
		prepage(buffer,reclen);		/* Force copy on write */
				/* wait for children to start */
		if(distributed && master_iozone)
		{
			start_master_listen_loop((int) num_child);
		}
		for(i=0;i<num_child; i++){
			child_stat = (struct child_stats *)&shmaddr[i];	
			while(child_stat->flag==CHILD_STATE_HOLD)
				Poll((long long)1);
		}
		for(i=0;i<num_child; i++)	/* Start all children going */
		{
			if(delay_start!=0)
				Poll((long long)delay_start);
						/* State "go" */
			child_stat = (struct child_stats *)&shmaddr[i];	
			child_stat->flag=CHILD_STATE_BEGIN;
			if(distributed && master_iozone)
				tell_children_begin(i);
		}
		starttime1 = time_so_far();	/* Start parents timer */
		goto waitout;
	}

waitout:
	getout=0;
	if((long long)getpid() == myid) {	/* Parent only */
		starttime1 = time_so_far(); /* Wait for all children */
		for( i = 0; i < num_child; i++){
			child_stat = (struct child_stats *) &shmaddr[i];
			if(distributed && master_iozone)
			{
				wait_dist_join();
				break;
			}
			else
			{
			 if(use_thread)
			 {
				thread_join(childids[i],(void *)&pstatus);
			 }
			 else
			 {
		   	 	wait(0);
			 }
			}
			if(!jstarttime)
				jstarttime = time_so_far(); 
		}
		jtime = (time_so_far()-jstarttime)-time_res;
		if(jtime < (double).000001) 
		{
			jtime=time_res; 
		}
	}
	total_time = (time_so_far() - starttime1)-time_res; /* get parents total time */
	if(total_time < (double).000001) 
	{
		total_time=time_res; 
		if(rec_prob < reclen)
			rec_prob = reclen;
		res_prob=1;
	}
#ifdef JTIME
	total_time=total_time-jtime;/* Remove the join time */
	if(!silent) printf("\nJoin time %10.2f\n",jtime);
#endif

	total_kilos=0;
	ptotal=0;
	time_begin = time_fini = 0.0;
	walltime = 0.0;
	cputime = 0.0;
	if(!silent) printf("\n");
	for(xyz=0;xyz<num_child;xyz++){
		child_stat = (struct child_stats *) &shmaddr[xyz];
		total_kilos += child_stat->throughput; /* add up the children */
		ptotal += child_stat->actual;
		if(!min_xfer)
			min_xfer=child_stat->actual;
		if(child_stat->actual < min_xfer)
			min_xfer=child_stat->actual;
		if(!min_throughput)
			min_throughput=child_stat->throughput;
		if(child_stat->throughput < min_throughput)
			min_throughput=child_stat->throughput;
		if(child_stat->throughput > max_throughput)
			max_throughput=child_stat->throughput;
		/* Add up the cpu times of all children */
		cputime += child_stat->cputime;

		/* and find the child with the longest wall time */
		/* Get the earliest start time and latest fini time to calc. elapsed time. */
		if (time_begin == 0.0)
			time_begin = child_stat->start_time;
		else
			if (child_stat->start_time < time_begin)
				time_begin = child_stat->start_time;
		if (child_stat->fini_time > time_fini)
			time_fini = child_stat->fini_time;
	}
	avg_throughput=total_kilos/num_child;
	if(cpuutilflag)
	{
		walltime = time_fini - time_begin;
		if (walltime < cputime_res)
			walltime = 0.0;
		if (cputime < cputime_res)
			cputime = 0.0;
	}
	
	for(xyz=0;xyz<num_child;xyz++){
		child_stat = (struct child_stats *) &shmaddr[xyz];
		child_stat->flag = CHILD_STATE_HOLD; /* Start children at state 0 (HOLD) */
	}
	if(cpuutilflag)
		store_times (walltime, cputime);	/* Must be Before store_dvalue(). */
	store_dvalue(total_kilos);
#ifdef NO_PRINT_LLD
	if(!silent) printf("\n\n\tChildren see throughput for %2ld initial writers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	if(!silent) printf("\tParent sees throughput for %2ld initial writers \t= %10.2f %s/sec\n",num_child,((double)(ptotal)/total_time),unit);
#else
	if(!silent) printf("\n\n\tChildren see throughput for %2lld initial writers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	if(!silent) printf("\tParent sees throughput for %2lld initial writers \t= %10.2f %s/sec\n",num_child,((double)(ptotal)/total_time),unit);
#endif
	if(!silent) printf("\tMin throughput per %s \t\t\t= %10.2f %s/sec \n", port,min_throughput,unit);
	if(!silent) printf("\tMax throughput per %s \t\t\t= %10.2f %s/sec\n", port,max_throughput,unit);
	if(!silent) printf("\tAvg throughput per %s \t\t\t= %10.2f %s/sec\n", port,avg_throughput,unit);
	if(!silent) printf("\tMin xfer \t\t\t\t\t= %10.2f %s\n", min_xfer,unit);
	/* CPU% can be > 100.0 for multiple CPUs */
	if(cpuutilflag)
	{
		if(walltime == 0.0)
		{
			if(!silent) printf("\tCPU utilization: Wall time %8.3f    CPU time %8.3f    CPU utilization %6.2f %%\n\n",
				walltime, cputime, 0.0);
		}
		else
		{
			if(!silent) printf("\tCPU utilization: Wall time %8.3f    CPU time %8.3f    CPU utilization %6.2f %%\n\n",
				walltime, cputime, 100.0 * cputime / walltime);
		}
	}
	if(Cflag)
		for(xyz=0;xyz<num_child;xyz++){
			child_stat = (struct child_stats *) &shmaddr[xyz];
			if(cpuutilflag)
				if(!silent) printf("\tChild[%d] xfer count = %10.2f %s, Throughput = %10.2f %s/sec, wall=%6.3f, cpu=%6.3f, %%=%6.2f\n",
					(long)xyz, child_stat->actual, unit, child_stat->throughput, unit, child_stat->walltime, 
					child_stat->cputime, cpu_util(child_stat->cputime, child_stat->walltime));
			else
				if(!silent) printf("\tChild[%d] xfer count = %10.2f %s, Throughput = %10.2f %s/sec\n",
					(long)xyz, child_stat->actual, unit, child_stat->throughput, unit);
		}
	/**********************************************************/
	/*************** End of intitial writer *******************/
	/**********************************************************/
	sync();
	sleep(2);
	*stop_flag=0;
	if(distributed && master_iozone)
	{
		stop_master_listen(master_listen_socket);
		cleanup_comm();
	}

	/**********************************************************/
	/* Re-write throughput performance test. ******************/
	/**********************************************************/
	time_begin = time_fini = 0.0;
	walltime = 0.0;
	cputime = 0.0;
	jstarttime=0;
	total_kilos=0;
	toutputindex=1;
	strcpy(&toutput[1][0],throughput_tests[1]);
	/* Hooks to start the distributed Iozone client/server code */
	if(distributed)
	{
		use_thread=0;  /* Turn of any Posix threads */
		if(master_iozone)
			master_listen_socket = start_master_listen();
		else
			become_client();
	}
	if(!use_thread)
	{
	   for(xx = 0; xx< num_child ; xx++){
		chid=xx;
		childids[xx] = start_child_proc(THREAD_REWRITE_TEST,numrecs64,reclen);
		if(childids[xx]==-1){
			printf("\nFork failed\n");
			for(xy = 0; xy< xx ; xy++){
				Kill((long long)childids[xy],(long long)SIGTERM);
			}
			exit(28);
		}
		if(childids[xx] == 0){
#ifdef _64BIT_ARCH_
			thread_rwrite_test((void *)xx);
#else
			thread_rwrite_test((void *)((long)xx));
#endif
		}	
	   }
	}
#ifndef NO_THREADS
	else
	{
	   for(xx = 0; xx< num_child ; xx++){	/* Create the children */
#ifdef _64BIT_ARCH_
		childids[xx] = mythread_create( thread_rwrite_test,xx);
#else
		childids[xx] = mythread_create( thread_rwrite_test,(void *)(long)xx);
#endif
		if(childids[xx]==-1){
			printf("\nThread create failed\n");
			for(xy = 0; xy< xx ; xy++){
				Kill((long long)myid,(long long)SIGTERM);
			}
			exit(29);
		}
	   }
	}
#endif
	if((long long)myid == getpid())
	{
		if(distributed && master_iozone)
		{
			start_master_listen_loop((int) num_child);
		}
		for(i=0;i<num_child; i++){
			child_stat = (struct child_stats *)&shmaddr[i];
					/* wait for children to start */
			while(child_stat->flag==CHILD_STATE_HOLD) 
				Poll((long long)1);
		}
		for(i=0;i<num_child; i++)
		{
			child_stat = (struct child_stats *)&shmaddr[i];
			child_stat->flag = CHILD_STATE_BEGIN;	/* tell children to go */
			if(delay_start!=0)
				Poll((long long)delay_start);
			if(distributed && master_iozone)
				tell_children_begin(i);
		}
		starttime1 = time_so_far();
		goto jump3;
	}
	
jump3:
	getout=0;
	if((long long)myid == getpid()){	/* Parent only here */
		for( i = 0; i < num_child; i++){
			child_stat=(struct child_stats *)&shmaddr[i];
			if(distributed && master_iozone)
			{
				wait_dist_join();
				break;
			}
			else
			{
			   if(use_thread)
			   {
				thread_join(childids[i],(void *)&pstatus);
			   }
			   else
			   {
		   		wait(0);
			   }
			}
			if(!jstarttime)
				jstarttime = time_so_far(); 
		}
		jtime = (time_so_far()-jstarttime)-time_res;
		if(jtime < (double).000001) 
		{
			jtime=time_res;
		}
	}
	total_time = (time_so_far() - starttime1)-time_res; /* Parents total time */
	if(total_time  < (double).000001) 
	{
		total_time=time_res;
		if(rec_prob < reclen)
			rec_prob = reclen;
		res_prob=1;
	}
#ifdef JTIME
	total_time=total_time-jtime;/* Remove the join time */
	if(!silent) printf("\nJoin time %10.2f\n",jtime);
#endif
	

	total_kilos=0;
	ptotal=0;

	min_throughput=max_throughput=min_xfer=0;
	if(!silent) printf("\n");
	for(xyz=0;xyz<num_child;xyz++){
		child_stat=(struct child_stats *)&shmaddr[xyz];
		total_kilos+=child_stat->throughput;
		ptotal+=child_stat->actual;
		if(!min_xfer)
			min_xfer=child_stat->actual;
		if(child_stat->actual < min_xfer)
			min_xfer=child_stat->actual;
		if(!min_throughput)
			min_throughput=child_stat->throughput;
		if(child_stat->throughput < min_throughput)
			min_throughput=child_stat->throughput;
		if(child_stat->throughput > max_throughput)
			max_throughput=child_stat->throughput;
		cputime += child_stat->cputime;
		/* Get the earliest start time and latest fini time to calc. elapsed time. */
		if (time_begin == 0.0)
			time_begin = child_stat->start_time;
		else
			if (child_stat->start_time < time_begin)
				time_begin = child_stat->start_time;
		if (child_stat->fini_time > time_fini)
			time_fini = child_stat->fini_time;
	}
	avg_throughput=total_kilos/num_child;
	if(cpuutilflag)
	{
		walltime = time_fini - time_begin;
		if (walltime < cputime_res)
			walltime = 0.0;
		if (cputime < cputime_res)
			cputime = 0.0;
	}

	for(xyz=0;xyz<num_child;xyz++){	/* Reset state to 0 (HOLD) */
		child_stat=(struct child_stats *)&shmaddr[xyz];
		child_stat->flag = CHILD_STATE_HOLD;
	}
	if(cpuutilflag)
		store_times (walltime, cputime);	/* Must be Before store_dvalue(). */
	store_dvalue(total_kilos);
#ifdef NO_PRINT_LLD
	if(!silent) printf("\tChildren see throughput for %2ld rewriters \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	if(!silent) printf("\tParent sees throughput for %2ld rewriters \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#else
	if(!silent) printf("\tChildren see throughput for %2lld rewriters \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	if(!silent) printf("\tParent sees throughput for %2lld rewriters \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#endif
	if(!silent) printf("\tMin throughput per %s \t\t\t= %10.2f %s/sec \n", port,min_throughput,unit);
	if(!silent) printf("\tMax throughput per %s \t\t\t= %10.2f %s/sec\n", port,max_throughput,unit);
	if(!silent) printf("\tAvg throughput per %s \t\t\t= %10.2f %s/sec\n", port,avg_throughput,unit);
	if(!silent) printf("\tMin xfer \t\t\t\t\t= %10.2f %s\n", min_xfer,unit);
	/* CPU% can be > 100.0 for multiple CPUs */
	if(cpuutilflag)
	{
		if(walltime == 0.0)
		{
			if(!silent) printf("\tCPU utilization: Wall time %8.3f    CPU time %8.3f    CPU utilization %6.2f %%\n\n",
				walltime, cputime, 0.0);
		}
		else
		{
			if(!silent) printf("\tCPU utilization: Wall time %8.3f    CPU time %8.3f    CPU utilization %6.2f %%\n\n",
				walltime, cputime, 100.0 * cputime / walltime);
		}
	}
	if(Cflag)
		for(xyz=0;xyz<num_child;xyz++){
			child_stat = (struct child_stats *) &shmaddr[xyz];
			if(cpuutilflag)
				if(!silent) printf("\tChild[%d] xfer count = %10.2f %s, Throughput = %10.2f %s/sec, wall=%6.3f, cpu=%6.3f, %%=%6.2f\n",
					(long)xyz, child_stat->actual, unit, child_stat->throughput, unit, child_stat->walltime, 
					child_stat->cputime, cpu_util(child_stat->cputime, child_stat->walltime));
			else
				if(!silent) printf("\tChild[%d] xfer count = %10.2f %s, Throughput = %10.2f %s/sec\n",
					(long)xyz, child_stat->actual, unit, child_stat->throughput, unit);
		}
	*stop_flag=0;
	/**********************************************************/
	/*************** End of rewrite throughput ****************/
	/**********************************************************/
	if(distributed && master_iozone)
	{
		stop_master_listen(master_listen_socket);
		cleanup_comm();
	}
	sync();
	sleep(2);
next0:
	if(include_tflag)
		if(!(include_mask & READER_MASK))
			goto next1;
	/**************************************************************/
	/*** Reader throughput tests **********************************/
	/**************************************************************/
	toutputindex++;
	strcpy(&toutput[toutputindex][0],throughput_tests[2]);
	time_begin = time_fini = 0.0;
	walltime = 0.0;
	cputime = 0.0;
	jstarttime=0;
	total_kilos=0;
	if(distributed)
	{
		use_thread=0;
		if(master_iozone)
			master_listen_socket=start_master_listen();
		else
			become_client();
	}
	if(!use_thread)
	{
	   for(xx = 0; xx< num_child ; xx++){
		chid=xx;
		childids[xx] = start_child_proc(THREAD_READ_TEST,numrecs64,reclen);
		if(childids[xx]==-1){
			printf("\nFork failed\n");
			for(xy = 0; xy< xx ; xy++){
				Kill((long long)childids[xy],(long long)SIGTERM);
			}
			exit(30);
		}
		if(childids[xx]==0){
#ifdef _64BIT_ARCH_
			thread_read_test((void *)xx);
#else
			thread_read_test((void *)((long)xx));
#endif
		}	
	   }
	}
#ifndef NO_THREADS
	else
	{
	   for(xx = 0; xx< num_child ; xx++){	/* Create the children */
#ifdef _64BIT_ARCH_
		childids[xx] = mythread_create( thread_read_test,xx);
#else
		childids[xx] = mythread_create( thread_read_test,(void *)(long)xx);
#endif
		if(childids[xx]==-1){
			printf("\nThread create failed\n");
			for(xy = 0; xy< xx ; xy++){
				kill((pid_t)myid,(int)SIGTERM);
			}
			exit(31);
		}
	   }
	}
#endif
	if(myid == (long long)getpid()){
		if(distributed && master_iozone)
		{
			start_master_listen_loop((int) num_child);
		}
		for(i=0;i<num_child; i++){ /* wait for children to start */
			child_stat=(struct child_stats *)&shmaddr[i];
			while(child_stat->flag==CHILD_STATE_HOLD)
				Poll((long long)1);
		}
		for(i=0;i<num_child; i++)
		{
			child_stat=(struct child_stats *)&shmaddr[i];
			child_stat->flag = CHILD_STATE_BEGIN; /* tell children to go */
			if(delay_start!=0)
				Poll((long long)delay_start);
			if(distributed && master_iozone)
				tell_children_begin(i);
		}
		starttime1 = time_so_far();
		goto jumpend;
	}
jumpend:
	getout=0;
	if(myid == (long long)getpid()){	/* Parent here */
		for( i = 0; i < num_child; i++){
			child_stat = (struct child_stats *)&shmaddr[i];
			if(distributed && master_iozone)
			{
				wait_dist_join();
				break;
			}
			else
			{
			   if(use_thread)
			   {
				thread_join(childids[i],(void *)&pstatus);
			   }
			   else
			   {
				wait(0);
			   }
			}
			if(!jstarttime)
				jstarttime = time_so_far(); 
		}
		jtime = (time_so_far()-jstarttime)-time_res; 
		if(jtime < (double).000001)
		{
			jtime=time_res; 
		}
	}
	total_time = (time_so_far() - starttime1)-time_res; /* Parents time */
	if(total_time < (double).000001) 
	{
		total_time=time_res; 
		if(rec_prob < reclen)
			rec_prob = reclen;
		res_prob=1;
	}
#ifdef JTIME
	total_time=total_time-jtime;/* Remove the join time */
	if(!silent) printf("\nJoin time %10.2f\n",jtime);
#endif
	
	total_kilos=0;
	ptotal=0;
	min_throughput=max_throughput=min_xfer=0;
	if(!silent) printf("\n");
	for(xyz=0;xyz<num_child;xyz++){
		child_stat=(struct child_stats *)&shmaddr[xyz];
		total_kilos+=child_stat->throughput;
		ptotal+=child_stat->actual;
		if(!min_xfer)
			min_xfer=child_stat->actual;
		if(child_stat->actual < min_xfer)
			min_xfer=child_stat->actual;
		if(!min_throughput)
			min_throughput=child_stat->throughput;
		if(child_stat->throughput < min_throughput)
			min_throughput=child_stat->throughput;
		if(child_stat->throughput > max_throughput)
			max_throughput=child_stat->throughput;
		cputime += child_stat->cputime;
		/* Get the earliest start time and latest fini time to calc. elapsed time. */
		if (time_begin == 0.0)
			time_begin = child_stat->start_time;
		else
			if (child_stat->start_time < time_begin)
				time_begin = child_stat->start_time;
		if (child_stat->fini_time > time_fini)
			time_fini = child_stat->fini_time;
	}
	avg_throughput=total_kilos/num_child;
	if(cpuutilflag)
	{
		walltime = time_fini - time_begin;
		if (walltime < cputime_res)
			walltime = 0.0;
		if (cputime < cputime_res)
			cputime = 0.0;
	}
	if(cpuutilflag)
		store_times (walltime, cputime);	/* Must be Before store_dvalue(). */
	store_dvalue(total_kilos);
#ifdef NO_PRINT_LLD
	if(!silent) printf("\tChildren see throughput for %2ld readers \t\t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	if(!silent) printf("\tParent sees throughput for %2ld readers \t\t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#else
	if(!silent) printf("\tChildren see throughput for %2lld readers \t\t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	if(!silent) printf("\tParent sees throughput for %2lld readers \t\t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#endif
	if(!silent) printf("\tMin throughput per %s \t\t\t= %10.2f %s/sec \n", port,min_throughput,unit);
	if(!silent) printf("\tMax throughput per %s \t\t\t= %10.2f %s/sec\n", port,max_throughput,unit);
	if(!silent) printf("\tAvg throughput per %s \t\t\t= %10.2f %s/sec\n", port,avg_throughput,unit);
	if(!silent) printf("\tMin xfer \t\t\t\t\t= %10.2f %s\n", min_xfer,unit);
	/* CPU% can be > 100.0 for multiple CPUs */
	if(cpuutilflag)
	{
		if(walltime == 0.0)
		{
			if(!silent) printf("\tCPU utilization: Wall time %8.3f    CPU time %8.3f    CPU utilization %6.2f %%\n\n",
				walltime, cputime, 0.0);
		}
		else
		{
			if(!silent) printf("\tCPU utilization: Wall time %8.3f    CPU time %8.3f    CPU utilization %6.2f %%\n\n",
				walltime, cputime, 100.0 * cputime / walltime);
		}
	}
	if(Cflag)
		for(xyz=0;xyz<num_child;xyz++){
			child_stat = (struct child_stats *) &shmaddr[xyz];
			if(cpuutilflag)
				if(!silent) printf("\tChild[%d] xfer count = %10.2f %s, Throughput = %10.2f %s/sec, wall=%6.3f, cpu=%6.3f, %%=%6.2f\n",
					(long)xyz, child_stat->actual, unit, child_stat->throughput, unit, child_stat->walltime, 
					child_stat->cputime, cpu_util(child_stat->cputime, child_stat->walltime));
			else
				if(!silent) printf("\tChild[%d] xfer count = %10.2f %s, Throughput = %10.2f %s/sec\n",
					(long)xyz, child_stat->actual, unit, child_stat->throughput, unit);
		}
	/**********************************************************/
	/*************** End of readers throughput ****************/
	/**********************************************************/
	if(distributed && master_iozone)
	{
		stop_master_listen(master_listen_socket);
		cleanup_comm();
	}
	sync();
	sleep(2);

	if (no_unlink)
		goto next1;	/* rags: skip rereader */

	/**************************************************************/
	/*** ReReader throughput tests **********************************/
	/**************************************************************/
	toutputindex++;
	strcpy(&toutput[toutputindex][0],throughput_tests[3]);
	time_begin = time_fini = 0.0;
	walltime = 0.0;
	cputime = 0.0;
	jstarttime=0;
	*stop_flag=0;
	total_kilos=0;
        /* Hooks to start the distributed Iozone client/server code */
        if(distributed)
        {
                use_thread=0;  /* Turn of any Posix threads */
                if(master_iozone)
                        master_listen_socket = start_master_listen();
                else
                        become_client();
        }
	if(!use_thread)
	{
	   for(xx = 0; xx< num_child ; xx++){
		chid=xx;
		childids[xx] = start_child_proc(THREAD_REREAD_TEST, numrecs64,reclen);
		if(childids[xx]==-1){
			printf("\nFork failed\n");
			for(xy = 0; xy< xx ; xy++){
				Kill((long long)childids[xy],(long long)SIGTERM);
			}
			exit(32);
		}
		if(childids[xx]==0){
#ifdef _64BIT_ARCH_
			thread_rread_test((void *)xx);
#else
			thread_rread_test((void *)((long)xx));
#endif
		}	
	   }
	}
#ifndef NO_THREADS
	else
	{
	   for(xx = 0; xx< num_child ; xx++){	/* Create the children */
		chid=xx;
#ifdef _64BIT_ARCH_
		childids[xx] = mythread_create( thread_rread_test,xx);
#else
		childids[xx] = mythread_create( thread_rread_test,(void *)(long)xx);
#endif
		if(childids[xx]==-1){
			printf("\nThread create failed\n");
			for(xy = 0; xy< xx ; xy++){
				kill((pid_t)myid,(int)SIGTERM);
			}
			exit(33);
		}
	   }
	}
#endif
	if(myid == (long long)getpid()){
                if(distributed && master_iozone)
                {
                        start_master_listen_loop((int) num_child);
                }
		for(i=0;i<num_child; i++){ /* wait for children to start */
			child_stat = (struct child_stats *)&shmaddr[i];
			while(child_stat->flag==CHILD_STATE_HOLD)
				Poll((long long)1);
		}
		for(i=0;i<num_child; i++){
			child_stat = (struct child_stats *)&shmaddr[i];
			child_stat->flag = CHILD_STATE_BEGIN;	/* tell children to go */
			if(delay_start!=0)
				Poll((long long)delay_start);
                       if(distributed && master_iozone)
                                tell_children_begin(i);
		}
		starttime1 = time_so_far();
		goto jumpend2;
	}
	
jumpend2:
	getout=0;
	if(myid == (long long)getpid()){	 /* Parent here */
		for( i = 0; i < num_child; i++){ /* wait for children to stop */
			child_stat = (struct child_stats *)&shmaddr[i];
                        if(distributed && master_iozone)
                        {
                                wait_dist_join();
                                break;
                        }
                        else
                        {
			   if(use_thread)
			   {
				thread_join(childids[i],(void *)&pstatus);
			   }
			   else
			   {
				wait(0);
			   }
			}
			if(!jstarttime)
				jstarttime = time_so_far(); 
		}
		jtime = (time_so_far()-jstarttime)-time_res; 
		if(jtime < (double).000001)
		{
			jtime=time_res;
		}
	}
	total_time = (time_so_far() - starttime1)-time_res; /* Parents time */
	if(total_time < (double).000001) 
	{
		total_time=time_res;
		if(rec_prob < reclen)
			rec_prob = reclen;
		res_prob=1;
	}
#ifdef JTIME
	total_time=total_time-jtime;/* Remove the join time */
	if(!silent) printf("\nJoin time %10.2f\n",jtime);
#endif
	min_throughput=max_throughput=min_xfer=0;
	total_kilos=0;
	ptotal=0;
	if(!silent) printf("\n");
	for(xyz=0;xyz<num_child;xyz++){
		child_stat = (struct child_stats *)&shmaddr[xyz];
		total_kilos+=child_stat->throughput;
		ptotal+=child_stat->actual;
		if(!min_xfer)
			min_xfer=child_stat->actual;
		if(child_stat->actual < min_xfer)
			min_xfer=child_stat->actual;
		if(!min_throughput)
			min_throughput=child_stat->throughput;
		if(child_stat->throughput < min_throughput)
			min_throughput=child_stat->throughput;
		if(child_stat->throughput > max_throughput)
			max_throughput=child_stat->throughput;
		cputime += child_stat->cputime;
		/* Get the earliest start time and latest fini time to calc. elapsed time. */
		if (time_begin == 0.0)
			time_begin = child_stat->start_time;
		else
			if (child_stat->start_time < time_begin)
				time_begin = child_stat->start_time;
		if (child_stat->fini_time > time_fini)
			time_fini = child_stat->fini_time;
	}
	avg_throughput=total_kilos/num_child;
	if(cpuutilflag)
	{
		walltime = time_fini - time_begin;
		if (walltime < cputime_res)
			walltime = 0.0;
		if (cputime < cputime_res)
			cputime = 0.0;
	}
	if(cpuutilflag)
		store_times (walltime, cputime);	/* Must be Before store_dvalue(). */
	store_dvalue(total_kilos);
#ifdef NO_PRINT_LLD
	if(!silent) printf("\tChildren see throughput for %ld re-readers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	if(!silent) printf("\tParent sees throughput for %ld re-readers \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#else
	if(!silent) printf("\tChildren see throughput for %lld re-readers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	if(!silent) printf("\tParent sees throughput for %lld re-readers \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#endif
	if(!silent) printf("\tMin throughput per %s \t\t\t= %10.2f %s/sec \n", port,min_throughput,unit);
	if(!silent) printf("\tMax throughput per %s \t\t\t= %10.2f %s/sec\n", port,max_throughput,unit);
	if(!silent) printf("\tAvg throughput per %s \t\t\t= %10.2f %s/sec\n", port,avg_throughput,unit);
	if(!silent) printf("\tMin xfer \t\t\t\t\t= %10.2f %s\n", min_xfer,unit);
	/* CPU% can be > 100.0 for multiple CPUs */
	if(cpuutilflag)
	{
		if(walltime == 0.0)
		{
			if(!silent) printf("\tCPU utilization: Wall time %8.3f    CPU time %8.3f    CPU utilization %6.2f %%\n\n",
				walltime, cputime, 0.0);
		}
		else
		{
			if(!silent) printf("\tCPU utilization: Wall time %8.3f    CPU time %8.3f    CPU utilization %6.2f %%\n\n",
				walltime, cputime, 100.0 * cputime / walltime);
		}
	}
	if(Cflag)
		for(xyz=0;xyz<num_child;xyz++){
			child_stat = (struct child_stats *) &shmaddr[xyz];
			if(cpuutilflag)
				if(!silent) printf("\tChild[%d] xfer count = %10.2f %s, Throughput = %10.2f %s/sec, wall=%6.3f, cpu=%6.3f, %%=%6.2f\n",
					(long)xyz, child_stat->actual, unit, child_stat->throughput, unit, child_stat->walltime, 
					child_stat->cputime, cpu_util(child_stat->cputime, child_stat->walltime));
			else
				if(!silent) printf("\tChild[%d] xfer count = %10.2f %s, Throughput = %10.2f %s/sec\n",
					(long)xyz, child_stat->actual, unit, child_stat->throughput, unit);
		}
	/**********************************************************/
	/*************** End of re-readers throughput ****************/
	/**********************************************************/
        if(distributed && master_iozone)
	{
                stop_master_listen(master_listen_socket);
		cleanup_comm();
	}

next1:
	if(include_tflag)
		if(!(include_mask & REVERSE_MASK))
			goto next2;
	sync();
	sleep(2);

	/**************************************************************/
	/*** Reverse reader throughput tests **************************/
	/**************************************************************/
	toutputindex++;
	strcpy(&toutput[toutputindex][0],throughput_tests[4]);
	time_begin = time_fini = 0.0;
	walltime = 0.0;
	cputime = 0.0;
	jstarttime=0;
	*stop_flag=0;
	total_kilos=0;
        /* Hooks to start the distributed Iozone client/server code */
        if(distributed)
        {
                use_thread=0;  /* Turn of any Posix threads */
                if(master_iozone)
                        master_listen_socket = start_master_listen();
                else
                        become_client();
        }
	if(!use_thread)
	{
	   for(xx = 0; xx< num_child ; xx++){
		chid=xx;
		childids[xx] = start_child_proc(THREAD_REVERSE_READ_TEST,numrecs64,reclen);
		if(childids[xx]==-1){
			printf("\nFork failed\n");
			for(xy = 0; xy< xx ; xy++){
				Kill((long long)childids[xy],(long long)SIGTERM);
			}
			exit(34);
		}
		if(childids[xx]==0){
#ifdef _64BIT_ARCH_
			thread_reverse_read_test((void *)xx);
#else
			thread_reverse_read_test((void *)((long)xx));
#endif
		}	
	   }
	}
#ifndef NO_THREADS
	else
	{
	   for(xx = 0; xx< num_child ; xx++){	/* Create the children */
		chid=xx;
#ifdef _64BIT_ARCH_
		childids[xx] = mythread_create( thread_reverse_read_test,xx);
#else
		childids[xx] = mythread_create( thread_reverse_read_test,(void *)(long)xx);
#endif
		if(childids[xx]==-1){
			printf("\nThread create failed\n");
			for(xy = 0; xy< xx ; xy++){
				kill((pid_t)myid,(int)SIGTERM);
			}
			exit(35);
		}
	   }
	}
#endif
	if(myid == (long long)getpid()){
                if(distributed && master_iozone)
                {
                        start_master_listen_loop((int) num_child);
                }
		for(i=0;i<num_child; i++){ /* wait for children to start */
			child_stat = (struct child_stats *)&shmaddr[i];
			while(child_stat->flag==CHILD_STATE_HOLD)
				Poll((long long)1);
		}
		for(i=0;i<num_child; i++){
			child_stat = (struct child_stats *)&shmaddr[i];
			child_stat->flag = CHILD_STATE_BEGIN;	/* tell children to go */
			if(delay_start!=0)
				Poll((long long)delay_start);
                       if(distributed && master_iozone)
                                tell_children_begin(i);
		}
		starttime1 = time_so_far();
	}
	
	getout=0;
	if(myid == (long long)getpid()){	 /* Parent here */
		for( i = 0; i < num_child; i++){ /* wait for children to stop */
			child_stat = (struct child_stats *)&shmaddr[i];
                        if(distributed && master_iozone)
                        {
                                wait_dist_join();
                                break;
                        }
                        else
                        {
                           if(use_thread)
                           {
                                thread_join(childids[i],(void *)&pstatus);
                           }
                           else
                           {
                                wait(0);
                           }
                        }
			if(!jstarttime)
				jstarttime = time_so_far(); 
		}
		jtime = (time_so_far()-jstarttime)-time_res;
		if(jtime < (double).000001)
		{
			jtime=time_res;
		}
	}
	total_time = (time_so_far() - starttime1)-time_res; /* Parents time */
	if(total_time < (double).000001) 
	{
		total_time=time_res;
		if(rec_prob < reclen)
			rec_prob = reclen;
		res_prob=1;
	}
#ifdef JTIME
	total_time=total_time-jtime;/* Remove the join time */
	if(!silent) printf("\nJoin time %10.2f\n",jtime);
#endif
	total_kilos=0;
	ptotal=0;
	min_throughput=max_throughput=min_xfer=0;
	if(!silent) printf("\n");
	for(xyz=0;xyz<num_child;xyz++){
		child_stat = (struct child_stats *)&shmaddr[xyz];
		total_kilos+=child_stat->throughput;
		ptotal+=child_stat->actual;
		if(!min_xfer)
			min_xfer=child_stat->actual;
		if(child_stat->actual < min_xfer)
			min_xfer=child_stat->actual;
		if(!min_throughput)
			min_throughput=child_stat->throughput;
		if(child_stat->throughput < min_throughput)
			min_throughput=child_stat->throughput;
		if(child_stat->throughput > max_throughput)
			max_throughput=child_stat->throughput;
		/* walltime += child_stat->walltime; */
		cputime += child_stat->cputime;
		/* Get the earliest start time and latest fini time to calc. elapsed time. */
		if (time_begin == 0.0)
			time_begin = child_stat->start_time;
		else
			if (child_stat->start_time < time_begin)
				time_begin = child_stat->start_time;
		if (child_stat->fini_time > time_fini)
			time_fini = child_stat->fini_time;
	}
	avg_throughput=total_kilos/num_child;
	if(cpuutilflag)
	{
		walltime = time_fini - time_begin;
		if (walltime < cputime_res)
			walltime = 0.0;
		if (cputime < cputime_res)
			cputime = 0.0;
	}
	if(cpuutilflag)
		store_times (walltime, cputime);	/* Must be Before store_dvalue(). */
	store_dvalue(total_kilos);
#ifdef NO_PRINT_LLD
	if(!silent) printf("\tChildren see throughput for %ld reverse readers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	if(!silent) printf("\tParent sees throughput for %ld reverse readers \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#else
	if(!silent) printf("\tChildren see throughput for %lld reverse readers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	if(!silent) printf("\tParent sees throughput for %lld reverse readers \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#endif
	if(!silent) printf("\tMin throughput per %s \t\t\t= %10.2f %s/sec \n", port,min_throughput,unit);
	if(!silent) printf("\tMax throughput per %s \t\t\t= %10.2f %s/sec\n", port,max_throughput,unit);
	if(!silent) printf("\tAvg throughput per %s \t\t\t= %10.2f %s/sec\n", port,avg_throughput,unit);
	if(!silent) printf("\tMin xfer \t\t\t\t\t= %10.2f %s\n", min_xfer,unit);
	/* CPU% can be > 100.0 for multiple CPUs */
	if(cpuutilflag)
	{
		if(walltime == 0.0)
		{
			if(!silent) printf("\tCPU utilization: Wall time %8.3f    CPU time %8.3f    CPU utilization %6.2f %%\n\n",
				walltime, cputime, 0.0);
		}
		else
		{
			if(!silent) printf("\tCPU utilization: Wall time %8.3f    CPU time %8.3f    CPU utilization %6.2f %%\n\n",
				walltime, cputime, 100.0 * cputime / walltime);
		}
	}
	if(Cflag)
		for(xyz=0;xyz<num_child;xyz++){
			child_stat = (struct child_stats *) &shmaddr[xyz];
			if(cpuutilflag)
				if(!silent) printf("\tChild[%d] xfer count = %10.2f %s, Throughput = %10.2f %s/sec, wall=%6.3f, cpu=%6.3f, %%=%6.2f\n",
					(long)xyz, child_stat->actual, unit, child_stat->throughput, unit, child_stat->walltime, 
					child_stat->cputime, cpu_util(child_stat->cputime, child_stat->walltime));
			else
				if(!silent) printf("\tChild[%d] xfer count = %10.2f %s, Throughput = %10.2f %s/sec\n",
					(long)xyz, child_stat->actual, unit, child_stat->throughput, unit);
		}
        if(distributed && master_iozone)
	{
                stop_master_listen(master_listen_socket);
		cleanup_comm();
	}
next2:
	if(include_tflag)
		if(!(include_mask & STRIDE_READ_MASK))
			goto next3;
	/**************************************************************/
	/*** stride reader throughput tests **************************/
	/**************************************************************/
	toutputindex++;
	strcpy(&toutput[toutputindex][0],throughput_tests[5]);
	time_begin = time_fini = 0.0;
	walltime = 0.0;
	cputime = 0.0;
	jstarttime=0;
	sync();
	sleep(2);
	*stop_flag=0;
	total_kilos=0;
        /* Hooks to start the distributed Iozone client/server code */
        if(distributed)
        {
                use_thread=0;  /* Turn of any Posix threads */
                if(master_iozone)
                        master_listen_socket = start_master_listen();
                else
                        become_client();
        }
	if(!use_thread)
	{
	   for(xx = 0; xx< num_child ; xx++){
		chid=xx;
		childids[xx] = start_child_proc(THREAD_STRIDE_TEST,numrecs64,reclen);
		if(childids[xx]==-1){
			printf("\nFork failed\n");
			for(xy = 0; xy< xx ; xy++){
				Kill((long long)childids[xy],(long long)SIGTERM);
			}
			exit(36);
		}
		if(childids[xx]==0){
#ifdef _64BIT_ARCH_
			thread_stride_read_test((void *)xx);
#else
			thread_stride_read_test((void *)((long)xx));
#endif
		}	
	   }
	}
#ifndef NO_THREADS
	else
	{
	   for(xx = 0; xx< num_child ; xx++){	/* Create the children */
		chid=xx;
#ifdef _64BIT_ARCH_
		childids[xx] = mythread_create( thread_stride_read_test,xx);
#else
		childids[xx] = mythread_create( thread_stride_read_test,(void *)(long)xx);
#endif
		if(childids[xx]==-1){
			printf("\nThread create failed\n");
			for(xy = 0; xy< xx ; xy++){
				kill((pid_t)myid,(int)SIGTERM);
			}
			exit(37);
		}
	   }
	}
#endif
	if(myid == (long long)getpid()){
                if(distributed && master_iozone)
                {
                        start_master_listen_loop((int) num_child);
                }
		for(i=0;i<num_child; i++){ /* wait for children to start */
			child_stat = (struct child_stats *)&shmaddr[i];
			while(child_stat->flag==CHILD_STATE_HOLD)
				Poll((long long)1);
		}
		for(i=0;i<num_child; i++){
			child_stat = (struct child_stats *)&shmaddr[i];
			child_stat->flag = CHILD_STATE_BEGIN;	/* tell children to go */
			if(delay_start!=0)
				Poll((long long)delay_start);
                       if(distributed && master_iozone)
                                tell_children_begin(i);
		}
		starttime1 = time_so_far();
	}
	
	getout=0;
	if(myid == (long long)getpid()){	 /* Parent here */
		for( i = 0; i < num_child; i++){ /* wait for children to stop */
			child_stat = (struct child_stats *)&shmaddr[i];
                        if(distributed && master_iozone)
                        {
                                wait_dist_join();
                                break;
                        }
                        else
                        {
                           if(use_thread)
                           {
                                thread_join(childids[i],(void *)&pstatus);
                           }
                           else
                           {
                                wait(0);
                           }
                        }
			if(!jstarttime)
				jstarttime = time_so_far(); 
		}
		jtime = (time_so_far()-jstarttime)-time_res;
		if(jtime < (double).000001) 
		{
			jtime=time_res; 
		}
	}
	total_time = (time_so_far() - starttime1)-time_res; /* Parents time */
	if(total_time < (double).000001) 
	{
		total_time=time_res;
		if(rec_prob < reclen)
			rec_prob = reclen;
		res_prob=1;
	}
#ifdef JTIME
	total_time=total_time-jtime;/* Remove the join time */
	if(!silent) printf("\nJoin time %10.2f\n",jtime);
#endif
	total_kilos=0;
	ptotal=0;
	min_throughput=max_throughput=min_xfer=0;
	if(!silent) printf("\n");
	for(xyz=0;xyz<num_child;xyz++){
		child_stat = (struct child_stats *)&shmaddr[xyz];
		total_kilos+=child_stat->throughput;
		ptotal+=child_stat->actual;
		if(!min_xfer)
			min_xfer=child_stat->actual;
		if(child_stat->actual < min_xfer)
			min_xfer=child_stat->actual;
		if(!min_throughput)
			min_throughput=child_stat->throughput;
		if(child_stat->throughput < min_throughput)
			min_throughput=child_stat->throughput;
		if(child_stat->throughput > max_throughput)
			max_throughput=child_stat->throughput;
		/* walltime += child_stat->walltime; */
		cputime += child_stat->cputime;
		/* Get the earliest start time and latest fini time to calc. elapsed time. */
		if (time_begin == 0.0)
			time_begin = child_stat->start_time;
		else
			if (child_stat->start_time < time_begin)
				time_begin = child_stat->start_time;
		if (child_stat->fini_time > time_fini)
			time_fini = child_stat->fini_time;
	}
	avg_throughput=total_kilos/num_child;
	if(cpuutilflag)
	{
		walltime = time_fini - time_begin;
		if (walltime < cputime_res)
			walltime = 0.0;
		if (cputime < cputime_res)
			cputime = 0.0;
	}
	if(cpuutilflag)
		store_times (walltime, cputime);	/* Must be Before store_dvalue(). */
	store_dvalue(total_kilos);
#ifdef NO_PRINT_LLD
	if(!silent) printf("\tChildren see throughput for %ld stride readers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	if(!silent) printf("\tParent sees throughput for %ld stride readers \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#else
	if(!silent) printf("\tChildren see throughput for %lld stride readers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	if(!silent) printf("\tParent sees throughput for %lld stride readers \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#endif
	if(!silent) printf("\tMin throughput per %s \t\t\t= %10.2f %s/sec \n", port,min_throughput,unit);
	if(!silent) printf("\tMax throughput per %s \t\t\t= %10.2f %s/sec\n", port,max_throughput,unit);
	if(!silent) printf("\tAvg throughput per %s \t\t\t= %10.2f %s/sec\n", port,avg_throughput,unit);
	if(!silent) printf("\tMin xfer \t\t\t\t\t= %10.2f %s\n", min_xfer,unit);
	/* CPU% can be > 100.0 for multiple CPUs */
	if(cpuutilflag)
	{
		if(walltime == 0.0)
		{
			if(!silent) printf("\tCPU utilization: Wall time %8.3f    CPU time %8.3f    CPU utilization %6.2f %%\n\n",
				walltime, cputime, 0.0);
		}
		else
		{
			if(!silent) printf("\tCPU utilization: Wall time %8.3f    CPU time %8.3f    CPU utilization %6.2f %%\n\n",
				walltime, cputime, 100.0 * cputime / walltime);
		}
	}
	if(Cflag)
		for(xyz=0;xyz<num_child;xyz++){
			child_stat = (struct child_stats *) &shmaddr[xyz];
			if(cpuutilflag)
				if(!silent) printf("\tChild[%d] xfer count = %10.2f %s, Throughput = %10.2f %s/sec, wall=%6.3f, cpu=%6.3f, %%=%6.2f\n",
					(long)xyz, child_stat->actual, unit, child_stat->throughput, unit, child_stat->walltime, 
					child_stat->cputime, cpu_util(child_stat->cputime, child_stat->walltime));
			else
				if(!silent) printf("\tChild[%d] xfer count = %10.2f %s, Throughput = %10.2f %s/sec\n",
					(long)xyz, child_stat->actual, unit, child_stat->throughput, unit);
		}
        if(distributed && master_iozone)
	{
                stop_master_listen(master_listen_socket);
		cleanup_comm();
	}
	/**************************************************************/
	/*** random reader throughput tests ***************************/
	/**************************************************************/
next3:
	if(include_tflag)
		if(!(include_mask & RANDOM_RW_MASK))
			goto next4;
	
	toutputindex++;
	strcpy(&toutput[toutputindex][0],throughput_tests[6]);
	time_begin = time_fini = 0.0;
	walltime = 0.0;
	cputime = 0.0;
	jstarttime=0;
	sync();
	sleep(2);
	*stop_flag=0;
	total_kilos=0;
        /* Hooks to start the distributed Iozone client/server code */
        if(distributed)
        {
                use_thread=0;  /* Turn of any Posix threads */
                if(master_iozone)
                        master_listen_socket = start_master_listen();
                else
                        become_client();
        }
	if(!use_thread)
	{
	   for(xx = 0; xx< num_child ; xx++){
		chid=xx;
		childids[xx] = start_child_proc(THREAD_RANDOM_READ_TEST,numrecs64,reclen);
		if(childids[xx]==-1){
			printf("\nFork failed\n");
			for(xy = 0; xy< xx ; xy++){
				Kill((long long)childids[xy],(long long)SIGTERM);
			}
			exit(38);
		}
		if(childids[xx]==0){
#ifdef _64BIT_ARCH_
			thread_ranread_test((void *)xx);
#else
			thread_ranread_test((void *)((long)xx));
#endif
		}	
	   }
	}
#ifndef NO_THREADS
	else
	{
	   for(xx = 0; xx< num_child ; xx++){	/* Create the children */
		chid=xx;
#ifdef _64BIT_ARCH_
		childids[xx] = mythread_create( thread_ranread_test,xx);
#else
		childids[xx] = mythread_create( thread_ranread_test,(void *)(long)xx);
#endif
		if(childids[xx]==-1){
			printf("\nThread create failed\n");
			for(xy = 0; xy< xx ; xy++){
				kill((pid_t)myid,(int)SIGTERM);
			}
			exit(39);
		}
	   }
	}
#endif
	if(myid == (long long)getpid()){
                if(distributed && master_iozone)
                {
                        start_master_listen_loop((int) num_child);
                }
		for(i=0;i<num_child; i++){ /* wait for children to start */
			child_stat = (struct child_stats *)&shmaddr[i];
			while(child_stat->flag==CHILD_STATE_HOLD)
				Poll((long long)1);
		}
		for(i=0;i<num_child; i++){
			child_stat = (struct child_stats *)&shmaddr[i];
			child_stat->flag = CHILD_STATE_BEGIN;	/* tell children to go */
			if(delay_start!=0)
				Poll((long long)delay_start);
                       if(distributed && master_iozone)
                                tell_children_begin(i);
		}
		starttime1 = time_so_far();
	}
	
	getout=0;
	if(myid == (long long)getpid()){	 /* Parent here */
		for( i = 0; i < num_child; i++){ /* wait for children to stop */
			child_stat = (struct child_stats *)&shmaddr[i];
                        if(distributed && master_iozone)
                        {
                                wait_dist_join();
                                break;
                        }
                        else
                        {
                           if(use_thread)
                           {
                                thread_join(childids[i],(void *)&pstatus);
                           }
                           else
                           {
                                wait(0);
                           }
                        }
			if(!jstarttime)
				jstarttime = time_so_far(); 
		}
		jtime = (time_so_far()-jstarttime)-time_res;
		if(jtime < (double).000001) 
		{
			jtime=time_res; 
		}
	}
	total_time = (time_so_far() - starttime1)-time_res; /* Parents time */
	if(total_time < (double).000001) 
	{
		total_time=time_res;
		if(rec_prob < reclen)
			rec_prob = reclen;
		res_prob=1;
	}
#ifdef JTIME
	total_time=total_time-jtime;/* Remove the join time */
	if(!silent) printf("\nJoin time %10.2f\n",jtime);
#endif
	total_kilos=0;
	ptotal=0;
	min_throughput=max_throughput=min_xfer=0;
	if(!silent) printf("\n");
	for(xyz=0;xyz<num_child;xyz++){
		child_stat = (struct child_stats *)&shmaddr[xyz];
		total_kilos+=child_stat->throughput;
		ptotal+=child_stat->actual;
		if(!min_xfer)
			min_xfer=child_stat->actual;
		if(child_stat->actual < min_xfer)
			min_xfer=child_stat->actual;
		if(!min_throughput)
			min_throughput=child_stat->throughput;
		if(child_stat->throughput < min_throughput)
			min_throughput=child_stat->throughput;
		if(child_stat->throughput > max_throughput)
			max_throughput=child_stat->throughput;
		cputime += child_stat->cputime;
		/* Get the earliest start time and latest fini time to calc. elapsed time. */
		if (time_begin == 0.0)
			time_begin = child_stat->start_time;
		else
			if (child_stat->start_time < time_begin)
				time_begin = child_stat->start_time;
		if (child_stat->fini_time > time_fini)
			time_fini = child_stat->fini_time;
	}
	avg_throughput=total_kilos/num_child;
	if(cpuutilflag)
	{
		walltime = time_fini - time_begin;
		if (walltime < cputime_res)
			walltime = 0.0;
		if (cputime < cputime_res)
			cputime = 0.0;
	}
	if(cpuutilflag)
		store_times (walltime, cputime);	/* Must be Before store_dvalue(). */
	store_dvalue(total_kilos);
#ifdef NO_PRINT_LLD
	if(!silent) printf("\tChildren see throughput for %ld random readers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	if(!silent) printf("\tParent sees throughput for %ld random readers \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#else
	if(!silent) printf("\tChildren see throughput for %lld random readers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	if(!silent) printf("\tParent sees throughput for %lld random readers \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#endif
	if(!silent) printf("\tMin throughput per %s \t\t\t= %10.2f %s/sec \n", port,min_throughput,unit);
	if(!silent) printf("\tMax throughput per %s \t\t\t= %10.2f %s/sec\n", port,max_throughput,unit);
	if(!silent) printf("\tAvg throughput per %s \t\t\t= %10.2f %s/sec\n", port,avg_throughput,unit);
	if(!silent) printf("\tMin xfer \t\t\t\t\t= %10.2f %s\n", min_xfer,unit);
	/* CPU% can be > 100.0 for multiple CPUs */
	if(cpuutilflag)
	{
		if(walltime == 0.0)
		{
			if(!silent) printf("\tCPU utilization: Wall time %8.3f    CPU time %8.3f    CPU utilization %6.2f %%\n\n",
				walltime, cputime, 0.0);
		}
		else
		{
			if(!silent) printf("\tCPU utilization: Wall time %8.3f    CPU time %8.3f    CPU utilization %6.2f %%\n\n",
				walltime, cputime, 100.0 * cputime / walltime);
		}
	}
	if(Cflag)
		for(xyz=0;xyz<num_child;xyz++){
			child_stat = (struct child_stats *) &shmaddr[xyz];
			if(cpuutilflag)
				if(!silent) printf("\tChild[%d] xfer count = %10.2f %s, Throughput = %10.2f %s/sec, wall=%6.3f, cpu=%6.3f, %%=%6.2f\n",
					(long)xyz, child_stat->actual, unit, child_stat->throughput, unit, child_stat->walltime, 
					child_stat->cputime, cpu_util(child_stat->cputime, child_stat->walltime));
			else
				if(!silent) printf("\tChild[%d] xfer count = %10.2f %s, Throughput = %10.2f %s/sec\n",
					(long)xyz, child_stat->actual, unit, child_stat->throughput, unit);
		}
        if(distributed && master_iozone)
	{
                stop_master_listen(master_listen_socket);
		cleanup_comm();
	}
next4:
	/**************************************************************/
	/*** random writer throughput tests  **************************/
	/**************************************************************/
	if(include_tflag)
		if(!(include_mask & RANDOM_RW_MASK))
			goto next5;
	
	toutputindex++;
	strcpy(&toutput[toutputindex][0],throughput_tests[7]);
	time_begin = time_fini = 0.0;
	walltime = 0.0;
	cputime = 0.0;
	jstarttime=0;
	sync();
	sleep(2);
	*stop_flag=0;
	total_kilos=0;
        /* Hooks to start the distributed Iozone client/server code */
        if(distributed)
        {
                use_thread=0;  /* Turn of any Posix threads */
                if(master_iozone)
                        master_listen_socket = start_master_listen();
                else
                        become_client();
        }
	if(!use_thread)
	{
	   for(xx = 0; xx< num_child ; xx++){
		chid=xx;
		childids[xx] = start_child_proc(THREAD_RANDOM_WRITE_TEST,numrecs64,reclen);
		if(childids[xx]==-1){
			printf("\nFork failed\n");
			for(xy = 0; xy< xx ; xy++){
				Kill((long long)childids[xy],(long long)SIGTERM);
			}
			exit(38);
		}
		if(childids[xx]==0){
#ifdef _64BIT_ARCH_
			thread_ranwrite_test((void *)xx);
#else
			thread_ranwrite_test((void *)((long)xx));
#endif
		}	
	   }
	}
#ifndef NO_THREADS
	else
	{
	   for(xx = 0; xx< num_child ; xx++){	/* Create the children */
		chid=xx;
#ifdef _64BIT_ARCH_
		childids[xx] = mythread_create( thread_ranwrite_test,xx);
#else
		childids[xx] = mythread_create( thread_ranwrite_test,(void *)(long)xx);
#endif
		if(childids[xx]==-1){
			printf("\nThread create failed\n");
			for(xy = 0; xy< xx ; xy++){
				kill((pid_t)myid,(int)SIGTERM);
			}
			exit(39);
		}
	   }
	}
#endif
	if(myid == (long long)getpid()){
                if(distributed && master_iozone)
                {
                        start_master_listen_loop((int) num_child);
                }
		for(i=0;i<num_child; i++){ /* wait for children to start */
			child_stat = (struct child_stats *)&shmaddr[i];
			while(child_stat->flag==CHILD_STATE_HOLD)
				Poll((long long)1);
		}
		for(i=0;i<num_child; i++){
			child_stat = (struct child_stats *)&shmaddr[i];
			child_stat->flag = CHILD_STATE_BEGIN;	/* tell children to go */
			if(delay_start!=0)
				Poll((long long)delay_start);
                       if(distributed && master_iozone)
                                tell_children_begin(i);
		}
		starttime1 = time_so_far();
	}
	
	getout=0;
	if(myid == (long long)getpid()){	 /* Parent here */
		for( i = 0; i < num_child; i++){ /* wait for children to stop */
			child_stat = (struct child_stats *)&shmaddr[i];
                        if(distributed && master_iozone)
                        {
                                wait_dist_join();
                                break;
                        }
                        else
                        {
                           if(use_thread)
                           {
                                thread_join(childids[i],(void *)&pstatus);
                           }
                           else
                           {
                                wait(0);
                           }
                        }
			if(!jstarttime)
				jstarttime = time_so_far(); 
		}
		jtime = (time_so_far()-jstarttime)-time_res;
		if(jtime < (double).000001) 
		{
			jtime=time_res; 
		}
	}
	total_time = (time_so_far() - starttime1)-time_res; /* Parents time */
	if(total_time < (double).000001) 
	{
		total_time=time_res;
		if(rec_prob < reclen)
			rec_prob = reclen;
		res_prob=1;
	}
#ifdef JTIME
	total_time=total_time-jtime;/* Remove the join time */
	if(!silent) printf("\nJoin time %10.2f\n",jtime);
#endif
	total_kilos=0;
	ptotal=0;
	min_throughput=max_throughput=min_xfer=0;
	if(!silent) printf("\n");
	for(xyz=0;xyz<num_child;xyz++){
		child_stat = (struct child_stats *)&shmaddr[xyz];
		total_kilos+=child_stat->throughput;
		ptotal+=child_stat->actual;
		if(!min_xfer)
			min_xfer=child_stat->actual;
		if(child_stat->actual < min_xfer)
			min_xfer=child_stat->actual;
		if(!min_throughput)
			min_throughput=child_stat->throughput;
		if(child_stat->throughput < min_throughput)
			min_throughput=child_stat->throughput;
		if(child_stat->throughput > max_throughput)
			max_throughput=child_stat->throughput;
		cputime += child_stat->cputime;
		/* Get the earliest start time and latest fini time to calc. elapsed time. */
		if (time_begin == 0.0)
			time_begin = child_stat->start_time;
		else
			if (child_stat->start_time < time_begin)
				time_begin = child_stat->start_time;
		if (child_stat->fini_time > time_fini)
			time_fini = child_stat->fini_time;
	}
	avg_throughput=total_kilos/num_child;
	if(cpuutilflag)
	{
		walltime = time_fini - time_begin;
		if (walltime < cputime_res)
			walltime = 0.0;
		if (cputime < cputime_res)
			cputime = 0.0;
	}
	if(cpuutilflag)
		store_times (walltime, cputime);	/* Must be Before store_dvalue(). */
	store_dvalue(total_kilos);
#ifdef NO_PRINT_LLD
	if(!silent) printf("\tChildren see throughput for %ld random writers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	if(!silent) printf("\tParent sees throughput for %ld random writers \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#else
	if(!silent) printf("\tChildren see throughput for %lld random writers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	if(!silent) printf("\tParent sees throughput for %lld random writers \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#endif
	if(!silent) printf("\tMin throughput per %s \t\t\t= %10.2f %s/sec \n", port,min_throughput,unit);
	if(!silent) printf("\tMax throughput per %s \t\t\t= %10.2f %s/sec\n", port,max_throughput,unit);
	if(!silent) printf("\tAvg throughput per %s \t\t\t= %10.2f %s/sec\n", port,avg_throughput,unit);
	if(!silent) printf("\tMin xfer \t\t\t\t\t= %10.2f %s\n", min_xfer,unit);
	/* CPU% can be > 100.0 for multiple CPUs */
	if(cpuutilflag)
	{
		if(walltime == 0.0)
		{
			if(!silent) printf("\tCPU utilization: Wall time %8.3f    CPU time %8.3f    CPU utilization %6.2f %%\n\n",
				walltime, cputime, 0.0);
		}
		else
		{
			if(!silent) printf("\tCPU utilization: Wall time %8.3f    CPU time %8.3f    CPU utilization %6.2f %%\n\n",
				walltime, cputime, 100.0 * cputime / walltime);
		}
	}
	if(Cflag)
		for(xyz=0;xyz<num_child;xyz++){
			child_stat = (struct child_stats *) &shmaddr[xyz];
			if(cpuutilflag)
				if(!silent) printf("\tChild[%d] xfer count = %10.2f %s, Throughput = %10.2f %s/sec, wall=%6.3f, cpu=%6.3f, %%=%6.2f\n",
					(long)xyz, child_stat->actual, unit, child_stat->throughput, unit, child_stat->walltime, 
					child_stat->cputime, cpu_util(child_stat->cputime, child_stat->walltime));
			else
				if(!silent) printf("\tChild[%d] xfer count = %10.2f %s, Throughput = %10.2f %s/sec\n",
					(long)xyz, child_stat->actual, unit, child_stat->throughput, unit);
		}
        if(distributed && master_iozone)
	{
                stop_master_listen(master_listen_socket);
		cleanup_comm();
	}
next5:
	sleep(2); /* You need this. If you stop and restart the 
		     master_listen it will fail on Linux */
	if (!no_unlink) {
		/**********************************************************/
		/* Cleanup all of the temporary files 			  */
		/* This is not really a test. It behaves like a test so   */
		/* it can unlink all of the same files that the other     */
		/* tests left hanging around.				  */
		/**********************************************************/
		/* Hooks to start the distributed Iozone client/server code */
		if(distributed)
		{
			use_thread=0;  /* Turn of any Posix threads */
			if(master_iozone)
				master_listen_socket = start_master_listen();
			else
				become_client();
		}
		if(!use_thread)
		{
		   for(xx = 0; xx< num_child ; xx++){
			chid=xx;
			childids[xx] = start_child_proc(THREAD_CLEANUP_TEST,numrecs64,reclen);
			if(childids[xx]==-1){
				printf("\nFork failed\n");
				for(xy = 0; xy< xx ; xy++){
					Kill((long long)childids[xy],(long long)SIGTERM);
				}
				exit(28);
			}
			if(childids[xx] == 0){
#ifdef _64BIT_ARCH_
				thread_cleanup_test((void *)xx);
#else
				thread_cleanup_test((void *)((long)xx));
#endif
			}	
		   }
		}
#ifndef NO_THREADS
		else
		{
		   for(xx = 0; xx< num_child ; xx++){	/* Create the children */
#ifdef _64BIT_ARCH_
			childids[xx] = mythread_create( thread_cleanup_test,xx);
#else
			childids[xx] = mythread_create( thread_cleanup_test,(void *)(long)xx);
#endif
			if(childids[xx]==-1){
				printf("\nThread create failed\n");
				for(xy = 0; xy< xx ; xy++){
					Kill((long long)myid,(long long)SIGTERM);
				}
				exit(29);
			}
		   }
		}
#endif
		if((long long)myid == getpid())
		{
			if(distributed && master_iozone)
			{
				start_master_listen_loop((int) num_child);
			}
			for(i=0;i<num_child; i++){
				child_stat = (struct child_stats *)&shmaddr[i];
						/* wait for children to start */
				while(child_stat->flag==CHILD_STATE_HOLD) 
					Poll((long long)1);
			}
			for(i=0;i<num_child; i++)
			{
				child_stat = (struct child_stats *)&shmaddr[i];
				child_stat->flag = CHILD_STATE_BEGIN;	/* tell children to go */
				if(delay_start!=0)
					Poll((long long)delay_start);
				if(distributed && master_iozone)
					tell_children_begin(i);
			}
		}
	
		getout=0;
		if((long long)myid == getpid()){	/* Parent only here */
			for( i = 0; i < num_child; i++){
				child_stat=(struct child_stats *)&shmaddr[i];
				if(distributed && master_iozone)
				{
					wait_dist_join();
					break;
				}
				else
				{
				   if(use_thread)
				   {
					thread_join(childids[i],(void *)&pstatus);
				   }
				   else
				   {
			   		wait(0);
				   }
				}
			}
		}

		for(xyz=0;xyz<num_child;xyz++){	/* Reset state to 0 (HOLD) */
			child_stat=(struct child_stats *)&shmaddr[xyz];
			child_stat->flag = CHILD_STATE_HOLD;
		}
		if(distributed && master_iozone)
		{
			stop_master_listen(master_listen_socket);
			cleanup_comm();
		}
	}
	/********************************************************/
	/* End of cleanup					*/
	/********************************************************/
	sync();
	if(!silent) printf("\n");
	if(!silent) printf("\n");
	return;
}

	
/************************************************************************/
/* Time measurement routines.						*/
/************************************************************************/

#ifdef HAVE_ANSIC_C
static double
time_so_far(void)
#else
static double
time_so_far()
#endif
{
#ifdef Windows
   LARGE_INTEGER freq,counter;
   double wintime,bigcounter;
	/* For Windows the time_of_day() is useless. It increments in 55 milli second   */
	/* increments. By using the Win32api one can get access to the high performance */
	/* measurement interfaces. With this one can get back into the 8 to 9  		*/
	/* microsecond resolution.							*/
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&counter);
        bigcounter=(double)counter.HighPart *(double)0xffffffff +
                (double)counter.LowPart;
        wintime = (double)(bigcounter/(double)freq.LowPart);
        return((double)wintime);
#else
#if defined (OSFV4) || defined(OSFV3) || defined(OSFV5)
  struct timespec gp;

  if (getclock(TIMEOFDAY, (struct timespec *) &gp) == -1)
    perror("getclock");
  return (( (double) (gp.tv_sec)) +
    ( ((float)(gp.tv_nsec)) * 0.000000001 ));
#else
  struct timeval tp;

  if (gettimeofday(&tp, (struct timezone *) NULL) == -1)
    perror("gettimeofday");
  return ((double) (tp.tv_sec)) +
    (((double) tp.tv_usec) * 0.000001 );
#endif
#endif
}

/************************************************************************/
/* FETCHIT ()								*/
/*									*/
/* Routine to make the on chip data cache hot for this buffer. The	*/
/* on chip cache may have been blown by other code in the application	*/
/* or in the OS.  Remember, on some machines, the data cache is direct  */
/* mapped and virtual indexed.						*/
/************************************************************************/

#ifdef HAVE_ANSIC_C
void fetchit(char *buffer,long long length)
#else
void fetchit(buffer,length)
char *buffer;
long long length;
#endif
{
	char *where;
	volatile long long x[4];
	long long i;
	where=(char *)buffer;
	for(i=0;i<(length/cache_line_size);i++)
	{
		x[(i & 3)]=*(where);
		where+=cache_line_size;
		
	}
}

/************************************************************************/
/* Verify that the buffer contains expected pattern			*/
/************************************************************************/
/* sverify == 0 means full check of pattern for every byte.             */
/* severify == 1 means partial check of pattern for each page.          */
/* sverify == 2 means no check, but partial touch for each page.        */
/************************************************************************/

#ifdef HAVE_ANSIC_C
long long 
verify_buffer(volatile char *buffer,long long length, off64_t recnum, long long recsize,unsigned long long patt,
	char sverify)
#else
long long 
verify_buffer(buffer,length, recnum, recsize,patt,sverify)
char *buffer;
long long length; 
off64_t recnum; 
long long recsize;
unsigned long long patt;
char sverify;
#endif
{
	volatile unsigned long long *where;
	volatile unsigned long long dummy;
	long long j,k;
	off64_t file_position=0;
	off64_t i;
	char *where2;
	where=(unsigned long long *)buffer;

	if(!verify)
		printf("\nOOPS You have entered verify_buffer unexpectedly !!! \n");

	if(sverify == 2)
	{
	  for(i=0;i<(length);i+=page_size)
	  {
	      dummy = *where;
	      where+=(page_size/sizeof(long long));
	  }
	  return(0);
	}
	if(sverify == 1)
	{
	  for(i=0;i<(length);i+=page_size)
	  {
	      if((unsigned long long)(*where) != (unsigned long long)((patt<<32) | patt))
	      {
		   file_position = (off64_t)( (recnum * recsize)+ i);
	printf("\n\n");
	printf("Error in file: Found ?%llx? Expecting ?%llx? addr %llx\n",*where, (long long)((patt<<32)|patt),where);
#ifdef NO_PRINT_LLD
	printf("Error in file: Position %ld \n",file_position);
	printf("Record # %ld Record size %ld kb \n",recnum,recsize/1024);
	printf("where %8.8llx loop %ld\n",where,i);
#else
	printf("Error in file: Position %lld \n",file_position);
	printf("Record # %lld Record size %lld kb \n",recnum,recsize/1024);
	printf("where %8.8llx loop %lld\n",where,i);
#endif
		   return(1);
	      }
	      where+=(page_size/sizeof(long long));
	  }
	}
	if(sverify == 0)
	{
	  for(i=0;i<(length/cache_line_size);i++)
	  {
	   for(j=0;j<(cache_line_size/sizeof(long long));j++)
	   {
	      if(*where != (unsigned long long)(patt<<32 | patt))
	      {
		   file_position = (off64_t)( (recnum * recsize))+
			((i*cache_line_size)+j);
		   where2=(char *)where;
		   for(k=0;k<sizeof(long long);k++){
		   	if(*where2 != (char)(patt&0xff))
				break;
		   	where2++;
		   }
		   file_position+=k;
	printf("\n\n");
#ifdef NO_PRINT_LLD
	printf("Error in file: Position %ld %ld %ld \n",i,j,k);
	printf("Error in file: Position %ld \n",file_position);
	printf("Record # %ld Record size %ld kb \n",recnum,recsize/1024);
#else
	printf("Error in file: Position %lld %lld %lld \n",i,j,k);
	printf("Error in file: Position %lld \n",file_position);
	printf("Record # %lld Record size %lld kb \n",recnum,recsize/1024);
#endif
	printf("Found pattern: Char >>%c<< Hex >>%2.2x<<\n", *where2,*where2);
		   return(1);
	      }
	      where++;
	   }	
	  }
	}
	return(0);
}
/************************************************************************/
/* Fill  the buffer 							*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void 
fill_buffer(char *buffer,long long length,long long pattern,char sverify)
#else
void 
fill_buffer(buffer,length,pattern,sverify)
char *buffer;
long long length;
long long pattern;
char sverify;
#endif
{
	unsigned long long *where;
	long long i,j;

	where=(unsigned long long *)buffer;
	if(sverify == 1)
	{
		for(i=0;i<(length);i+=page_size)
		{
			*where = (long long)((pattern<<32) | pattern);
			where+=(page_size/sizeof(long long)); 
			/* printf("Filling page %lld \n",i/page_size);*/
		}	
	}
	else
	{
		for(i=0;i<(length/cache_line_size);i++)
		{
			for(j=0;j<(cache_line_size/sizeof(long long));j++)
			{
				*where = (long long)((pattern<<32) | pattern);
				where++;
			}	
		}
	}
}

/************************************************************************/
/* PURGEIT() 								*/
/* 	 								*/
/* Routine to make the on chip data cache cold for this buffer.		*/
/* Remember, on some machines, the data cache is direct mapped and	*/
/* virtual indexed.							*/
/************************************************************************/

#ifdef HAVE_ANSIC_C
void 
purgeit(char *buffer,long long reclen)
#else
void 
purgeit(buffer,reclen)
char *buffer;
long long reclen;
#endif
{
	char *where;
	long rsize;
	long tsize;
	VOLATILE long long x[200];
	long i,cache_lines_per_rec;
	long cache_lines_per_cache;
	tsize = 200;
	cache_lines_per_rec = (long)(reclen/cache_line_size);
	cache_lines_per_cache = (long)(cache_size/cache_line_size);
	rsize = (long)l_min((long long)cache_lines_per_rec,(long long)cache_lines_per_cache);
#ifdef _64BIT_ARCH_
	where=(char *)pbuffer + ((unsigned long long)buffer & (cache_size-1));
#else
	where=(char *)pbuffer + ((long)buffer & ((long)cache_size-1));
#endif
	for(i=0;i<(rsize);i++)
	{
		x[i%tsize]=*(where);
		where+=cache_line_size;

	}
}

#ifdef HAVE_ANSIC_C
void
prepage(char *buffer,long long reclen)
#else
void
prepage(buffer, reclen)
char *buffer;
long long reclen; 
#endif
{
	char *where;
	long long i;
	where=(char *)buffer;
	for(i=0;i<(reclen/cache_line_size);i++)
	{
		*(where)=PATTERN;
		where+=cache_line_size;
	}
}

/************************************************************************/
/* write_perf_test ()				        		*/
/* Write and re-write test						*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void write_perf_test(off64_t kilo64,long long reclen ,long long *data1,long long *data2)
#else
void write_perf_test(kilo64,reclen ,data1,data2)
off64_t kilo64;
long long reclen;
long long *data1;
long long *data2;
#endif
{
	double starttime1;
	double writetime[2];
	double walltime[2], cputime[2];
	double qtime_start,qtime_stop;
	double compute_val = (double)0;
#ifdef unix
	double qtime_u_start,qtime_u_stop;
	double qtime_s_start,qtime_s_stop;
#endif
	long long i,j;
	off64_t numrecs64,traj_offset;
	long long Index = 0;
	long long file_flags = 0;
	long long traj_size;
	unsigned long long writerate[2];
	off64_t filebytes64;
	char *maddr;
	char *wmaddr,*free_addr;
	int fd,returnval,foo,wval;
#ifdef ASYNC_IO
	struct cache *gc=0;
#else
	long long *gc=0;
#endif

	if(w_traj_flag)
	{
		filebytes64 = w_traj_fsize;
		numrecs64=w_traj_ops;
	}
	else
	{
		numrecs64 = (kilo64*1024)/reclen;
		filebytes64 = numrecs64*reclen;
	}

	if(Q_flag && (!wol_opened))
	{
		wol_opened++;
		wqfd=fopen("wol.dat","a");
		if(wqfd==0)
		{
			printf("Unable to open wol.dat\n");
			exit(40);
		}
		fprintf(wqfd,"Offset in Kbytes   Latency in microseconds\n");
		rwqfd=fopen("rwol.dat","a");
		if(rwqfd==0)
		{
			printf("Unable to open rwol.dat\n");
			exit(41);
		}
		fprintf(rwqfd,"Offset in Kbytes   Latency in microseconds\n");
	}
	fd = 0;
	if(oflag)
		file_flags = O_RDWR|O_SYNC;
	else
		file_flags = O_RDWR;
#if defined(linux)
	if(direct_flag)
		file_flags |=O_DIRECT;
#endif

	for( j=0; j<2; j++)
	{
		if(cpuutilflag)
		{
			walltime[j] = time_so_far();
			cputime[j]  = cputime_so_far();
		}
		if(Uflag) /* Unmount and re-mount the mountpoint */
		{
			purge_buffer_cache();
		}
		if(j==0)
		{
#ifdef _LARGEFILE64_SOURCE 
	  		if((fd = creat64(filename, 0640))<0)
	  		{
				printf("\nCan not create temp file: %s\n", 
					filename);
				perror("creat");
				exit(42);
	  		}
#else
		  	if((fd = creat(filename, 0640))<0)
		  	{
				printf("\nCan not create temp file: %s\n", 
					filename);
				perror("creat");
				exit(43);
		  	}
#endif
		}
		if(fd) 
			close(fd);

#ifdef _LARGEFILE64_SOURCE 
	  	if((fd = open64(filename, (int)file_flags))<0)
	  	{
			printf("\nCan not open temp file: %s\n", 
				filename);
			perror("open");
			exit(44);
	  	}
#else
	  	if((fd = open(filename, (int)file_flags))<0)
	  	{
			printf("\nCan not open temp file: %s\n", 
				filename);
			perror("open");
			exit(45);
	  	}
#endif
		if(file_lock)
			if(mylockf((int) fd, (int) 1, (int)0)!=0)
				printf("File lock for write failed. %d\n",errno);
		if(mmapflag)
		{
			maddr=(char *)initfile(fd,filebytes64,1,PROT_READ|PROT_WRITE);
		}
		if(mmap_mix)
		{
			wval=write(fd, buffer, (size_t) 1);
			if(wval != 1)
			{
#ifdef NO_PRINT_LLD
			    	printf("\nError writing block %ld, fd= %d\n", (long long)0, fd);
#else
			    	printf("\nError writing block %lld, fd= %d\n", (long long)0, fd);
#endif
				if(wval==-1)
					perror("write");
				signal_handler();
			}
#ifdef _LARGEFILE64_SOURCE
			lseek64(fd,(off64_t)0,SEEK_SET);
#else
			lseek(fd,0,SEEK_SET);
#endif
		};
		fsync(fd);
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
		if(async_flag)
			async_init(&gc,fd,direct_flag);
#else
		if(async_flag)
			async_init(&gc,fd,0);
#endif
#endif
		buffer=mainbuffer;
		if(fetchon)
			fetchit(buffer,reclen);
		starttime1 = time_so_far();
#ifdef unix
		if(Q_flag)
		{
			qtime_u_start=utime_so_far();
			qtime_s_start=stime_so_far();
		}
#endif
		if(w_traj_flag)
		{
			rewind(w_traj_fd);
		}
		compute_val=(double)0;
		w_traj_ops_completed=0;
		w_traj_bytes_completed=0;
		for(i=0; i<numrecs64; i++){
			if(w_traj_flag)
			{
				traj_offset=get_traj(w_traj_fd, (long long *)&traj_size,(double *)&compute_time,(long)1);
				reclen=traj_size;
#ifdef _LARGEFILE64_SOURCE
				lseek64(fd,(off64_t)traj_offset,SEEK_SET);
#else
				lseek(fd,(off_t)traj_offset,SEEK_SET);
#endif
			}
			if(Q_flag)
			{
#ifdef _LARGEFILE64_SOURCE
				traj_offset=lseek64(fd,(off64_t)0,SEEK_CUR);
#else
				traj_offset=lseek(fd,(off_t)0,SEEK_CUR);
#endif
			}
			if(compute_flag)
				compute_val+=do_compute(compute_time);
			if(multi_buffer)
			{
				Index +=reclen;
				if(Index > (MAXBUFFERSIZE-reclen))
					Index=0;
				buffer = mbuffer + Index;	
			}
			if(async_flag && no_copy_flag)
			{
				free_addr=buffer=(char *)malloc((size_t)reclen+page_size);
				buffer=(char *)(((long)buffer+(long)page_size) & (long)(~page_size-1));
			}
			if(verify)
				fill_buffer(buffer,reclen,(long long)pattern,sverify);
			if(purge)
				purgeit(buffer,reclen);
			if(Q_flag)
			{
				qtime_start=time_so_far();
			}
			if(mmapflag)
			{
				wmaddr = &maddr[i*reclen];
				fill_area((long long*)buffer,(long long*)wmaddr,(long long)reclen);
				if(!mmapnsflag)
				{
				  if(mmapasflag)
				    msync(wmaddr,(size_t)reclen,MS_ASYNC);
				  if(mmapssflag)
				    msync(wmaddr,(size_t)reclen,MS_SYNC);
				}
			}
			else
			{
			  if(async_flag)
			  {
			     if(no_copy_flag)
			       async_write_no_copy(gc, (long long)fd, buffer, reclen, (i*reclen), depth,free_addr);
			     else
			       async_write(gc, (long long)fd, buffer, reclen, (i*reclen), depth);
			  }
			  else
			  {
			    wval=write(fd, buffer, (size_t ) reclen);
			    if(wval != reclen)
			    {
#ifdef NO_PRINT_LLD
			    	printf("\nError writing block %ld, fd= %d\n", i,
					 fd);
#else
			    	printf("\nError writing block %lld, fd= %d\n", i,
					 fd);
#endif
			    	if(wval == -1)
					perror("write");
				signal_handler();
			    }
			  }
			}
			if(Q_flag)
			{
				qtime_stop=time_so_far();
				if(j==0)
#ifdef NO_PRINT_LLD
				fprintf(wqfd,"%10.1ld %10.0f\n",(traj_offset)/1024,((qtime_stop-qtime_start-time_res))*1000000);
				else
				fprintf(rwqfd,"%10.1ld %10.0f\n",(traj_offset)/1024,((qtime_stop-qtime_start-time_res))*1000000);
#else
				fprintf(wqfd,"%10.1lld %10.0f\n",(traj_offset)/1024,((qtime_stop-qtime_start-time_res))*1000000);
				else
				fprintf(rwqfd,"%10.1lld %10.0f\n",(traj_offset)/1024,((qtime_stop-qtime_start-time_res))*1000000);
#endif
			}
			w_traj_ops_completed++;
			w_traj_bytes_completed+=reclen;
		}
#ifdef unix
		if(Q_flag)
		{
		   qtime_u_stop=utime_so_far();
		   qtime_s_stop=stime_so_far();
		   if(j==0)
			 fprintf(wqfd,"\nSystem time %10.3f User time %10.3f Real %10.3f  (seconds)\n",
				(qtime_s_stop-qtime_s_start)/sc_clk_tck,
				(qtime_u_stop-qtime_u_start)/sc_clk_tck,
				time_so_far()-starttime1);
		   else
			fprintf(rwqfd,"\nSystem time %10.3f User time %10.3f Real %10.3f  (seconds)\n",
				(qtime_s_stop-qtime_s_start)/sc_clk_tck,
				(qtime_u_stop-qtime_u_start)/sc_clk_tck,
				time_so_far()-starttime1);
		}
#endif

#ifdef ASYNC_IO
		if(async_flag)
		{
			end_async(gc);
			gc=0;
		}
#endif
		if(include_flush)
		{
			if(mmapflag)
				msync(maddr,(size_t)filebytes64,MS_SYNC);
			else
				fsync(fd);
		}
		if(file_lock)
			if(mylockf((int) fd),(int)0,(int)0)
				printf("Unlock failed %d\n",errno);
		if(include_close)
		{
			if(mmapflag)
			{
				mmap_end(maddr,(unsigned long long)filebytes64);
			}
			close(fd);
		}
		writetime[j] = ((time_so_far() - starttime1)-time_res)
			-compute_val;
		if(writetime[j] < (double).000001) 
		{
			writetime[j]=time_res;
			if(rec_prob < reclen)
				rec_prob = reclen;
			res_prob=1;
		}
		if(!include_close)
		{
			if(mmapflag)
				msync(maddr,(size_t)filebytes64,MS_SYNC);/* Clean up before read starts */
			else
				fsync(fd);
			if(mmapflag)
			{
				mmap_end(maddr,(unsigned long long)filebytes64);
			}
			close(fd);
		}
		if(cpuutilflag)
		{
			walltime[j] = time_so_far() - walltime[j];
			if (walltime[j] < cputime_res)
				walltime[j] = 0.0;
			cputime[j]  = cputime_so_far() - cputime[j];
			if (cputime[j] < cputime_res)
				cputime[j] = 0.0;
		}
	}
	if(OPS_flag || MS_flag){
	   filebytes64=w_traj_ops_completed;
	   /*filebytes64=filebytes64/reclen;*/
	}else
	   filebytes64=w_traj_bytes_completed;
		
out:
        for(j=0;j<2;j++)
        {
		writerate[j] = 
		      (unsigned long long) ((double) filebytes64 / writetime[j]);
		if(MS_flag)
		{
			writerate[j]=1000000.0*(1.0/writerate[j]);
			continue;
		}
		if(!(OPS_flag || MS_flag))
		   writerate[j] >>= 10;
	}
	data1[0]=writerate[0];
	/* Must save walltime & cputime before calling store_value() for each/any cell.*/
	if(cpuutilflag)
		store_times(walltime[0], cputime[0]);
	store_value((off64_t)writerate[0]);
	if(cpuutilflag)
		store_times(walltime[1], cputime[1]);
	store_value((off64_t)writerate[1]);
#ifdef NO_PRINT_LLD
	if(!silent) printf("%8ld",writerate[0]);
	if(!silent) printf("%8ld",writerate[1]);
	if(!silent) fflush(stdout);
#else
	if(!silent) printf("%8lld",writerate[0]);
	if(!silent) printf("%8lld",writerate[1]);
	if(!silent) fflush(stdout);
#endif
}
/************************************************************************/
/* fwrite_perf_test ()				        		*/
/* fWrite and fre-write test						*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void fwrite_perf_test(off64_t kilo64,long long reclen ,long long *data1,long long *data2)
#else
void fwrite_perf_test(kilo64,reclen ,data1,data2)
off64_t kilo64;
long long reclen;
long long *data1;
long long *data2;
#endif
{
	double starttime1;
	double writetime[2];
	double walltime[2], cputime[2];
	double compute_val = (double)0;
	long long i,j;
	off64_t numrecs64;
	long long Index = 0;
	unsigned long long writerate[2];
	off64_t filebytes64;
	FILE *stream = NULL;
	int fd;
	char *how;
	char *stdio_buf;

	numrecs64 = (kilo64*1024)/reclen;
	filebytes64 = numrecs64*reclen;
	stdio_buf=(char *)malloc((size_t)reclen);
	for( j=0; j<2; j++)
	{
		if(cpuutilflag)
		{
			walltime[j] = time_so_far();
			cputime[j]  = cputime_so_far();
		}
		if(Uflag) /* Unmount and re-mount the mountpoint */
		{
			purge_buffer_cache();
		}
		if(j==0)
			how="w+";
		else
			how="r+";
#ifdef IRIX64
		if((stream=(FILE *)fopen(filename,how)) == 0)
		{
			printf("\nCan not fdopen temp file: %s %lld\n", 
				filename,errno);
			perror("fdopen");
			exit(48);
		}
#else
#ifdef _LARGEFILE64_SOURCE
		if((stream=(FILE *)fopen64(filename,how)) == 0)
		{
#ifdef NO_PRINT_LLD
			printf("\nCan not fdopen temp file: %s %ld\n", 
				filename,errno);
#else
			printf("\nCan not fdopen temp file: %s %lld\n", 
				filename,errno);
#endif
			perror("fdopen");
			exit(49);
		}
#else
		if((stream=(FILE *)fopen(filename,how)) == 0)
		{
#ifdef NO_PRINT_LLD
			printf("\nCan not fdopen temp file: %s %ld\n", 
				filename,errno);
#else
			printf("\nCan not fdopen temp file: %s %lld\n", 
				filename,errno);
#endif
			perror("fdopen");
			exit(50);
		}
#endif
#endif
		fd=open(filename,O_RDONLY);
		fsync(fd);
		setvbuf(stream,stdio_buf,_IOFBF,reclen);
		buffer=mainbuffer;
		if(fetchon)
			fetchit(buffer,reclen);
		starttime1 = time_so_far();
		compute_val=(double)0;
		for(i=0; i<numrecs64; i++){
			if(compute_flag)
				compute_val+=do_compute(compute_time);
			if(multi_buffer)
			{
				Index +=reclen;
				if(Index > (MAXBUFFERSIZE-reclen))
					Index=0;
				buffer = mbuffer + Index;	
			}
			if(verify)
				fill_buffer(buffer,reclen,(long long)pattern,sverify);
			if(purge)
				purgeit(buffer,reclen);
			if(fwrite(buffer, (size_t) reclen, 1, stream) != 1)
			{
#ifdef NO_PRINT_LLD
			    	printf("\nError fwriting block %ld, fd= %d\n", i,
					 fd);
#else
			    	printf("\nError fwriting block %lld, fd= %d\n", i,
					 fd);
#endif
				perror("fwrite");
				signal_handler();
			}
		}

		if(include_flush)
		{
			fflush(stream);
			fsync(fd);
		}
		if(include_close)
		{
			fclose(stream);
		}
		writetime[j] = ((time_so_far() - starttime1)-time_res)
			-compute_val;
		if(writetime[j] < (double).000001) 
		{
			writetime[j]= time_res;
			if(rec_prob < reclen)
				rec_prob = reclen;
			res_prob=1;
		}
		if(!include_close)
		{
			fflush(stream);
			fclose(stream);
		}
		fsync(fd);
		close(fd);

		if(cpuutilflag)
		{
			walltime[j] = time_so_far() - walltime[j];
			if (walltime[j] < cputime_res)
				walltime[j] = 0.0;
			cputime[j]  = cputime_so_far() - cputime[j];
			if (cputime[j] < cputime_res)
				cputime[j] = 0.0;
		}
	}
	free(stdio_buf);
	if(OPS_flag || MS_flag){
	   filebytes64=filebytes64/reclen;
	}
        for(j=0;j<2;j++)
        {
		writerate[j] = 
		      (unsigned long long) ((double) filebytes64 / writetime[j]);
		if(MS_flag)
		{
			writerate[j]=1000000.0*(1.0/writerate[j]);
			continue;
		}
		if(!(OPS_flag || MS_flag))
			writerate[j] >>= 10;
	}
	/* Must save walltime & cputime before calling store_value() for each/any cell.*/
	if(cpuutilflag)
		store_times(walltime[0], cputime[0]);
	store_value((off64_t)writerate[0]);
	if(cpuutilflag)
		store_times(walltime[1], cputime[1]);
	store_value((off64_t)writerate[1]);
	data1[0]=writerate[0];
#ifdef NO_PRINT_LLD
	if(!silent) printf("%9ld",writerate[0]);
	if(!silent) printf("%9ld",writerate[1]);
	if(!silent) fflush(stdout);
#else
	if(!silent) printf("%9lld",writerate[0]);
	if(!silent) printf("%9lld",writerate[1]);
	if(!silent) fflush(stdout);
#endif
}

/************************************************************************/
/* fread_perf_test				        		*/
/* fRead and fre-read test						*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void fread_perf_test(off64_t kilo64,long long reclen,long long *data1,long long *data2)
#else
void fread_perf_test(kilo64,reclen,data1,data2)
off64_t kilo64;
long long reclen;
long long *data1,*data2;
#endif
{
	double starttime2;
	double readtime[2];
	double walltime[2], cputime[2];
	double compute_val = (double)0;
	long long j;
	off64_t i,numrecs64;
	long long Index = 0;
	unsigned long long readrate[2];
	off64_t filebytes64;
	FILE *stream = 0;
	char *stdio_buf;
	int fd;

	numrecs64 = (kilo64*1024)/reclen;
	filebytes64 = numrecs64*reclen;
	stdio_buf=(char *)malloc((size_t)reclen);

	for( j=0; j<2; j++ )
	{
		if(cpuutilflag)
		{
			walltime[j] = time_so_far();
			cputime[j]  = cputime_so_far();
		}

		if(Uflag) /* Unmount and re-mount the mountpoint */
		{
			purge_buffer_cache();
		}
#ifdef IRIX64
		if((stream=(FILE *)fopen(filename,"r")) == 0)
		{
			printf("\nCan not fdopen temp file: %s\n", 
				filename);
			perror("fdopen");
			exit(51);
		}
#else
#ifdef _LARGEFILE64_SOURCE
		if((stream=(FILE *)fopen64(filename,"r")) == 0)
		{
			printf("\nCan not fdopen temp file: %s\n", 
				filename);
			perror("fdopen");
			exit(52);
		}
#else
		if((stream=(FILE *)fopen(filename,"r")) == 0)
		{
			printf("\nCan not fdopen temp file: %s\n", 
				filename);
			perror("fdopen");
			exit(53);
		}
#endif
#endif
		fd=open(filename,O_RDONLY);
		fsync(fd);
		close(fd);
		setvbuf(stream,stdio_buf,_IOFBF,reclen);
		buffer=mainbuffer;
		if(fetchon)
			fetchit(buffer,reclen);
		compute_val=(double)0;
		starttime2 = time_so_far();
		for(i=0; i<numrecs64; i++) 
		{
			if(compute_flag)
				compute_val+=do_compute(compute_time);
                        if(multi_buffer)
                        {
                                Index +=reclen;
                                if(Index > (MAXBUFFERSIZE-reclen))
                                        Index=0;
                                buffer = mbuffer + Index;
                        }
			if(purge)
				purgeit(buffer,reclen);
			if(fread(buffer, (size_t) reclen,1, stream) != 1)
			{
#ifdef _64BIT_ARCH_
#ifdef NO_PRINT_LLD
				printf("\nError freading block %ld %x\n", i,
					(unsigned long long)buffer);
#else
				printf("\nError freading block %ld %x\n", i,
					(unsigned long long)buffer);
#endif
#else
#ifdef NO_PRINT_LLD
				printf("\nError freading block %ld %x\n", i,
					(long)buffer);
#else
				printf("\nError freading block %lld %x\n", i,
					(long)buffer);
#endif
#endif
				perror("read");
				exit(54);
			}
			if(verify){
				if(verify_buffer(buffer,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
					exit(55);
				}
			}
		}
		if(include_flush)
			fflush(stream);
		if(include_close)
		{
			fclose(stream);
		}
		readtime[j] = ((time_so_far() - starttime2)-time_res)
			-compute_val;
		if(readtime[j] < (double).000001) 
		{
			readtime[j]= time_res;
			if(rec_prob < reclen)
				rec_prob = reclen;
			res_prob=1;
		}
		if(!include_close)
		{
			fflush(stream);
			fclose(stream);
		}
		stream = NULL;
		if(cpuutilflag)
		{
			walltime[j] = time_so_far() - walltime[j];
			if (walltime[j] < cputime_res)
				walltime[j] = 0.0;
			cputime[j]  = cputime_so_far() - cputime[j];
			if (cputime[j] < cputime_res)
				cputime[j] = 0.0;
		}
    	}
	free(stdio_buf);
	if(OPS_flag || MS_flag){
	   filebytes64=filebytes64/reclen;
	}
        for(j=0;j<2;j++)
        {
	   	readrate[j] = 
		     (unsigned long long) ((double) filebytes64 / readtime[j]);
		if(MS_flag)
		{
			readrate[j]=1000000.0*(1.0/readrate[j]);
			continue;
		}
		if(!(OPS_flag || MS_flag))
			readrate[j] >>= 10;
	}
	data1[0]=readrate[0];
	data2[0]=1;
	/* Must save walltime & cputime before calling store_value() for each/any cell.*/
	if(cpuutilflag)
		store_times(walltime[0], cputime[0]);
	store_value((off64_t)readrate[0]);
	if(cpuutilflag)
		store_times(walltime[1], cputime[1]);
	store_value((off64_t)readrate[1]);
#ifdef NO_PRINT_LLD
	if(!silent) printf("%8ld",readrate[0]);
	if(!silent) printf("%9ld",readrate[1]);
	if(!silent) fflush(stdout);
#else
	if(!silent) printf("%8lld",readrate[0]);
	if(!silent) printf("%9lld",readrate[1]);
	if(!silent) fflush(stdout);
#endif
}

/************************************************************************/
/* read_perf_test				        		*/
/* Read and re-fread test						*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void read_perf_test(off64_t kilo64,long long reclen,long long *data1,long long *data2)
#else
void read_perf_test(kilo64,reclen,data1,data2)
off64_t kilo64;
long long reclen;
long long *data1,*data2;
#endif
{
	double starttime2;
	double compute_val = (double)0;
	double readtime[2];
	double walltime[2], cputime[2];
#ifdef unix
	double qtime_u_start,qtime_u_stop;
	double qtime_s_start,qtime_s_stop;
#endif
	long long j;
	long long traj_size;
	off64_t i,numrecs64,traj_offset;
	long long Index = 0;
	unsigned long long readrate[2];
	off64_t filebytes64;
	volatile char *buffer1;
	char *maddr;
	char *wmaddr;
	int fd,open_flags;
	double qtime_start,qtime_stop;
#ifdef ASYNC_IO
	struct cache *gc=0;

#else
	long long *gc=0;
#endif
	numrecs64 = (kilo64*1024)/reclen;

	open_flags = O_RDONLY;
#if defined(linux)
	if(direct_flag)
		open_flags |=O_DIRECT;
#endif
	if(r_traj_flag)
	{
		numrecs64=r_traj_ops;
		filebytes64 = r_traj_fsize;
	} else
		filebytes64 = numrecs64*reclen;
	fd = 0;
	if(Q_flag && (!rol_opened))
	{
		rol_opened++;
		rqfd=fopen("rol.dat","a");
		if(rqfd==0)
		{
			printf("Unable to open rol.dat\n");
			exit(56);
		}
		fprintf(rqfd,"Offset in Kbytes   Latency in microseconds\n");
		rrqfd=fopen("rrol.dat","a");
		if(rrqfd==0)
		{
			printf("Unable to open rrol.dat\n");
			exit(57);
		}
		fprintf(rrqfd,"Offset in Kbytes   Latency in microseconds\n");
	}
	/* 
	 * begin real testing
	 */
	for( j=0; j<2; j++ )
	{

		if(cpuutilflag)
		{
			walltime[j] = time_so_far();
			cputime[j]  = cputime_so_far();
		}

		if(Uflag) /* Unmount and re-mount the mountpoint */
		{
			purge_buffer_cache();
		}
#ifdef _LARGEFILE64_SOURCE
		if((fd = open64(filename, open_flags))<0)
		{
			printf("\nCan not open temporary file for read\n");
			perror("open");
			exit(58);
		}
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
		if(async_flag)
			async_init(&gc,fd,direct_flag);
#else
		if(async_flag)
			async_init(&gc,fd,0);
#endif
#endif
#ifdef VXFS
		if(direct_flag)
			ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#else
		if((fd = open(filename, open_flags))<0)
		{
			printf("\nCan not open temporary file for read\n");
			perror("open");
			exit(59);
		}
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
		if(async_flag)
			async_init(&gc,fd,direct_flag);
#else
		if(async_flag)
			async_init(&gc,fd,0);
#endif
#endif
#ifdef VXFS
		if(direct_flag)
			ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif

#endif
		if(file_lock)
			if(mylockf((int) fd, (int) 1, (int)1) != 0)
				printf("File lock for read failed. %d\n",errno);
		if(mmapflag)
		{
			maddr=(char *)initfile(fd,filebytes64,0,PROT_READ);
		}
		fsync(fd);
		/* 
		 *  Need to prime the instruction cache & TLB
		 */
		buffer=mainbuffer;
		if(fetchon)
			fetchit(buffer,reclen);
		if(read(fd, (void *)buffer, (size_t) 1) != 1)
		{
#ifdef _64BIT_ARCH_
			printf("\nError reading block %d %x\n", 0,
				(unsigned long long)buffer);
#else
			printf("\nError reading block %d %x\n", 0,
				(long)buffer);
#endif
			perror("read");
			exit(60);
		}
#ifdef _LARGEFILE64_SOURCE
		lseek64(fd,(off64_t)0,SEEK_SET);
#else
		lseek(fd,0,SEEK_SET);
#endif
		buffer=mainbuffer;

		if(fetchon)
			fetchit(buffer,reclen);
		starttime2 = time_so_far();
#ifdef unix
		if(Q_flag)
		{
			qtime_u_start=utime_so_far();
			qtime_s_start=stime_so_far();
		}
#endif
		if(r_traj_flag)
		{
			rewind(r_traj_fd);
		}
		compute_val=(double)0;
		r_traj_ops_completed=0;
		r_traj_bytes_completed=0;
		for(i=0; i<numrecs64; i++) 
		{
			if(disrupt_flag && ((i%DISRUPT)==0))
			{
				disrupt(fd);
			}
			if(r_traj_flag)
			{
				traj_offset=get_traj(r_traj_fd, (long long *)&traj_size,(double *)&compute_time, (long)0);
				reclen=traj_size;
#ifdef _LARGEFILE64_SOURCE
				lseek64(fd,(off64_t)traj_offset,SEEK_SET);
#else
				lseek(fd,(off_t)traj_offset,SEEK_SET);
#endif
			}
			if(Q_flag)
			{
#ifdef _LARGEFILE64_SOURCE
				traj_offset=lseek64(fd,(off64_t)0,SEEK_CUR);
#else
				traj_offset=lseek(fd,(off_t)0,SEEK_CUR);
#endif
			}
			if(compute_flag)
				compute_val+=do_compute(compute_time);
                        if(multi_buffer)
                        {
                                Index +=reclen;
                                if(Index > (MAXBUFFERSIZE-reclen))
                                        Index=0;
                                buffer = mbuffer + Index;
                        }
			if(purge)
				purgeit(buffer,reclen);
			if(Q_flag)
				qtime_start=time_so_far();
			if(mmapflag)
			{
				wmaddr=&maddr[i*reclen];
				fill_area((long long*)wmaddr,(long long*)buffer,(long long)reclen);
			}
			else
			{
			  if(async_flag)
			  {
			    if(no_copy_flag)
			      async_read_no_copy(gc, (long long)fd, &buffer1, (i*reclen), reclen,
			    	1LL,(numrecs64*reclen),depth);
			    else
			      async_read(gc, (long long)fd, buffer, (i*reclen),reclen,
			    	1LL,(numrecs64*reclen),depth);
			  }
			  else
			  {
			    if(read((int)fd, (void*)buffer, (size_t) reclen) != reclen)
			    {
#ifdef _64BIT_ARCH_
#ifdef NO_PRINT_LLD
				printf("\nError reading block %ld %lx\n", i,
					(unsigned long long)buffer);
#else
				printf("\nError reading block %lld %llx\n", i,
					(unsigned long long)buffer);
#endif
#else
#ifdef NO_PRINT_LLD
				printf("\nError reading block %lld %x\n", i,
					(long)buffer);
#else
				printf("\nError reading block %ld %x\n", i,
					(long)buffer);
#endif
#endif
				perror("read");
				exit(61);
			    }
			  }
			}
			if(verify) {
			  if(async_flag && no_copy_flag)
			  {
				if(verify_buffer(buffer1,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
					exit(62);
				}
			  }
			  else
			  {
				if(verify_buffer(buffer,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
					exit(63);
				}
			  }
			}
			if(async_flag && no_copy_flag)
				async_release(gc);
			buffer1=0;
			if(Q_flag)
			{
				qtime_stop=time_so_far();
				if(j==0)
#ifdef NO_PRINT_LLD
				fprintf(rqfd,"%10.1ld %10.0f\n",(traj_offset)/1024,(qtime_stop-qtime_start-time_res)*1000000);
				else
				fprintf(rrqfd,"%10.1ld %10.0f\n",(traj_offset)/1024,(qtime_stop-qtime_start-time_res)*1000000);
#else
				fprintf(rqfd,"%10.1lld %10.0f\n",(traj_offset)/1024,(qtime_stop-qtime_start-time_res)*1000000);
				else
				fprintf(rrqfd,"%10.1lld %10.0f\n",(traj_offset)/1024,(qtime_stop-qtime_start-time_res)*1000000);
#endif
			}
			r_traj_ops_completed++;
			r_traj_bytes_completed+=reclen;
		}
		if(file_lock)
			if(mylockf((int) fd, (int) 0, (int)1))
				printf("Read unlock failed. %d\n",errno);
#ifdef unix
		if(Q_flag)
		{
		   qtime_u_stop=utime_so_far();
		   qtime_s_stop=stime_so_far();
		   if(j==0)
			 fprintf(rqfd,"\nSystem time %10.3f User time %10.3f Real %10.3f  (seconds)\n",
				(qtime_s_stop-qtime_s_start)/sc_clk_tck,
				(qtime_u_stop-qtime_u_start)/sc_clk_tck,
				time_so_far()-starttime2);
		   else
			fprintf(rrqfd,"\nSystem time %10.3f User time %10.3f Real %10.3f  (seconds)\n",
				(qtime_s_stop-qtime_s_start)/sc_clk_tck,
				(qtime_u_stop-qtime_u_start)/sc_clk_tck,
				time_so_far()-starttime2);
		}
#endif
#ifdef ASYNC_IO
		if(async_flag)
		{
			end_async(gc);
			gc=0;
		}
#endif
		if(include_flush)
		{
			if(mmapflag)
				msync(maddr,(size_t)filebytes64,MS_SYNC);
			else
				fsync(fd);
		}
		if(include_close)
		{
			if(mmapflag)
			{
				mmap_end(maddr,(unsigned long long)filebytes64);
			}
			close(fd);
		}
		readtime[j] = ((time_so_far() - starttime2)-time_res)-compute_val;
		if(readtime[j] < (double).000001) 
		{
			readtime[j]= time_res;
			if(rec_prob < reclen)
				rec_prob = reclen;
			res_prob=1;
		}
		if(!include_close)
		{
			if(mmapflag)
				msync(maddr,(size_t)filebytes64,MS_SYNC);
			else
				fsync(fd);
			if(mmapflag)
			{
				mmap_end(maddr,(unsigned long long)filebytes64);
			}
			close(fd);
		}
		if(cpuutilflag)
		{
			walltime[j] = time_so_far() - walltime[j];
			if (walltime[j] < cputime_res)
				walltime[j] = 0.0;
			cputime[j]  = cputime_so_far() - cputime[j];
			if (cputime[j] < cputime_res)
				cputime[j] = 0.0;
		}
    	}
	if(OPS_flag || MS_flag){
	   filebytes64=r_traj_ops_completed;
	   /*filebytes64=filebytes64/reclen;*/
	} else
	   filebytes64=r_traj_bytes_completed;

        for(j=0;j<2;j++)
        {
	   	readrate[j] = 
		     (unsigned long long) ((double) filebytes64 / readtime[j]);
		if(MS_flag)
		{
			readrate[j]=1000000.0*(1.0/readrate[j]);
			continue;
		}
		if(!(OPS_flag || MS_flag))
			readrate[j] >>= 10;
			
	}
	data1[0]=readrate[0];
	data2[0]=1;
	/* Must save walltime & cputime before calling store_value() for each/any cell.*/
	if(cpuutilflag)
		store_times(walltime[0], cputime[0]);
	store_value((off64_t)readrate[0]);
	if(cpuutilflag)
		store_times(walltime[1], cputime[1]);
	store_value((off64_t)readrate[1]);
#ifdef NO_PRINT_LLD
	if(!silent) printf("%9ld",readrate[0]);
	if(!silent) printf("%9ld",readrate[1]);
	if(!silent) fflush(stdout);
#else
	if(!silent) printf("%9lld",readrate[0]);
	if(!silent) printf("%9lld",readrate[1]);
	if(!silent) fflush(stdout);
#endif
}


/************************************************************************/
/* random_perf_test				        		*/
/* Random read and write test						*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void random_perf_test(off64_t kilo64,long long reclen,long long *data1,long long *data2)
#else
void random_perf_test(kilo64,reclen,data1,data2)
off64_t kilo64;
long long reclen;
long long *data1, *data2;
#endif
{
	double randreadtime[2];
	double starttime2;
	double walltime[2], cputime[2];
	double compute_val = (double)0;
	long long j;
	off64_t i,numrecs64;
	long long Index=0;
	int flags;
	unsigned long long randreadrate[2];
	off64_t filebytes64;
	volatile char *buffer1;
	char *wmaddr;
	char *maddr,*free_addr;
	int fd,wval;
#ifdef ASYNC_IO
	struct cache *gc=0;
#else
	long long *gc=0;
#endif

	numrecs64 = (kilo64*1024)/reclen;
	flags = O_RDWR;
#if defined(linux)
	if(direct_flag)
		flags |=O_DIRECT;
#endif
	fd=0;
	if(oflag)
		flags |= O_SYNC;
	filebytes64 = numrecs64*reclen;
	for( j=0; j<2; j++ )
	{

	     if(cpuutilflag)
	     {
		     walltime[j] = time_so_far();
		     cputime[j]  = cputime_so_far();
	     }
	     if(Uflag) /* Unmount and re-mount the mountpoint */
	     {
			purge_buffer_cache();
	     }
#ifdef _LARGEFILE64_SOURCE
	     if((fd = open64(filename, (int)flags,0640))<0){
			printf("\nCan not open temporary file for read/write\n");
			perror("open");
			exit(66);
	     }
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
		if(async_flag)
			async_init(&gc,fd,direct_flag);
#else
		if(async_flag)
			async_init(&gc,fd,0);
#endif
#endif
#ifdef VXFS
		if(direct_flag)
			ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif

#else
	     if((fd = open(filename, (int)flags,0640))<0){
			printf("\nCan not open temporary file for read/write\n");
			perror("open");
			exit(67);
	     }
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
		if(async_flag)
			async_init(&gc,fd,direct_flag);
#else
		if(async_flag)
			async_init(&gc,fd,0);
#endif
#endif
#ifdef VXFS
	if(direct_flag)
		ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif

#endif
	     if(mmapflag)
	     {
			maddr=(char *)initfile(fd,filebytes64,0,PROT_READ|PROT_WRITE);
	     }
	     buffer=mainbuffer;
	     if(fetchon)
		   fetchit(buffer,reclen);
#ifdef bsd4_2
	     srand();
#else
#ifdef Windows
             srand();
#else
             srand48(0);
#endif
#endif
             compute_val=0;
	     starttime2 = time_so_far();
	     if ( j==0 ){
		for(i=0; i<numrecs64; i++) {
			if(compute_flag)
				compute_val+=do_compute(compute_time);
                        if(multi_buffer)
                        {
                                Index +=reclen;
                                if(Index > (MAXBUFFERSIZE-reclen))
                                        Index=0;
                                buffer = mbuffer + Index;
                        }
			if(purge)
				purgeit(buffer,reclen);
#ifdef bsd4_2
                        offset64 = reclen * (rand()%numrecs64);
#else
#ifdef Windows
			offset64 = reclen * (rand()%numrecs64);
#else
			offset64 = reclen * (lrand48()%numrecs64);
#endif
#endif

#ifdef _LARGEFILE64_SOURCE
			if( !(h_flag || k_flag || mmapflag))
			{
			   if(lseek64( fd, offset64, SEEK_SET )<0)
			   {
				perror("lseek64");
				exit(68);
			   };
			}
#else
			if (!(h_flag || k_flag || mmapflag))
			{
			  if(lseek( fd, (off_t )offset64, SEEK_SET )<0)
			  {
				perror("lseek");
				exit(69);
			  };
			}
#endif
			if(mmapflag)
			{
				wmaddr=&maddr[offset64];
				fill_area((long long*)wmaddr,(long long*)buffer,(long long)reclen);
			}
			else
			{
			  if(async_flag)
			  {
			     if(no_copy_flag)
			        async_read_no_copy(gc, (long long)fd, &buffer1, offset64,reclen,
			    	  0LL,(numrecs64*reclen),depth);
			     else
				 async_read(gc, (long long)fd, buffer, (offset64),reclen,
					    	0LL,(numrecs64*reclen),0LL);
			  }
			  else
			  {
		  	     if(read(fd, (void *)buffer, (size_t)reclen) != reclen)
		  	     {
#ifdef NO_PRINT_LLD
				 printf("\nError reading block at %ld\n",
					 offset64); 
#else
				 printf("\nError reading block at %lld\n",
					 offset64); 
#endif
				 perror("read");
				 exit(70);
		 	     }
			  }
			}
			if(verify){
			  if(async_flag && no_copy_flag)
			  {
				if(verify_buffer(buffer1,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
					exit(71);
				}
			  }
			  else
			  {
				if(verify_buffer(buffer,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
					exit(72);
				}
			  }
			}
			if(async_flag && no_copy_flag)
				async_release(gc);
		}
	     }
	     else
	     {
			for(i=0; i<numrecs64; i++) 
			{
				if(compute_flag)
					compute_val+=do_compute(compute_time);
                        	if(multi_buffer)
                        	{
                               	    Index +=reclen;
                               	    if(Index > (MAXBUFFERSIZE-reclen))
                               	         Index=0;
                               	    buffer = mbuffer + Index;
                        	}
				if(async_flag && no_copy_flag)
				{
					free_addr=buffer=(char *)malloc((size_t)reclen+page_size);
					buffer=(char *)(((long)buffer+(long)page_size) & (long)(~page_size-1));
				}
				if(verify)
					fill_buffer(buffer,reclen,(long long)pattern,sverify);
				if(purge)
					purgeit(buffer,reclen);
#ifdef bsd4_2
				offset64 = reclen * (rand()%numrecs64);
#else
#ifdef Windows
                                offset64 = reclen * (rand()%numrecs64);
#else
				offset64 = reclen * (lrand48()%numrecs64);
#endif
#endif

				if (!(h_flag || k_flag || mmapflag))
				{
#ifdef _LARGEFILE64_SOURCE
				  lseek64( fd, offset64, SEEK_SET );
#else
				  lseek( fd, (off_t)offset64, SEEK_SET );
#endif
				}
				if(mmapflag)
				{
					wmaddr=&maddr[offset64];
					fill_area((long long*)buffer,(long long*)wmaddr,(long long)reclen);
					if(!mmapnsflag)
					{
					  	if(mmapasflag)
						    	msync(wmaddr,(size_t)reclen,MS_ASYNC);
					  	if(mmapssflag)
					    		msync(wmaddr,(size_t)reclen,MS_SYNC);
					}
				}
				else
				{
			  		if(async_flag)
					{
			     		   if(no_copy_flag)
			       		      async_write_no_copy(gc, (long long)fd, buffer, reclen, offset64, 
					   	depth,free_addr);
					   else
			      			async_write(gc, (long long)fd, buffer, reclen, offset64, depth);
			  		}
			  		else
			  		{
			  		  wval=write(fd, buffer,(size_t)reclen);
			  		  if(wval != reclen)
			  		  {
#ifdef NO_PRINT_LLD
						printf("\nError writing block at %ld\n",
							offset64); 
#else
						printf("\nError writing block at %lld\n",
							offset64); 
#endif
						if(wval==-1)
							perror("write");
						signal_handler();
			 		  }
					}
				}
			}
	     } 	/* end of modifications	*kcollins:2-5-96 */
#ifdef ASYNC_IO
	     if(async_flag)
	     {
		end_async(gc);
	        gc=0;
             }	
#endif
	     if(include_flush)
	     {
		if(mmapflag)
			msync(maddr,(size_t)filebytes64,MS_SYNC);/* Clean up before read starts running */
		else
	     		fsync(fd);
	     }
	     if(include_close)
	     {
		if(mmapflag)
		{
			mmap_end(maddr,(unsigned long long)filebytes64);
		}
		close(fd);
	     }
	     randreadtime[j] = ((time_so_far() - starttime2)-time_res)-
			compute_val;
	     if(randreadtime[j] < (double).000001) 
	     {
			randreadtime[j]=time_res;
			if(rec_prob < reclen)
				rec_prob = reclen;
			res_prob=1;
	     }
	    if(!include_close)
	    {
		if(mmapflag)
		{
			msync(maddr,(size_t)filebytes64,MS_SYNC);/* Clean up before read starts running */
		}
		else
	     		fsync(fd);
		if(mmapflag)
			mmap_end(maddr,(unsigned long long)filebytes64);
		close(fd);
 	    }
            if(cpuutilflag)
	    {
	    	walltime[j] = time_so_far() - walltime[j];
		    if (walltime[j] < cputime_res)
			walltime[j] = 0.0;
	    	cputime[j]  = cputime_so_far() - cputime[j];
	    	if (cputime[j] < cputime_res)
			cputime[j] = 0.0;
	    }
    	}
	if(OPS_flag || MS_flag){
	   filebytes64=filebytes64/reclen;
	}
        for(j=0;j<2;j++)
        {
		randreadrate[j] = 
		      (unsigned long long) ((double) filebytes64 / randreadtime[j]);
		if(MS_flag)
		{
			randreadrate[j]=1000000.0*(1.0/randreadrate[j]);
			continue;
		}
		if(!(OPS_flag || MS_flag))
			randreadrate[j] >>= 10;
	}
	/* Must save walltime & cputime before calling store_value() for each/any cell.*/
        if(cpuutilflag)
		store_times(walltime[0], cputime[0]);
	store_value((off64_t)randreadrate[0]);
        if(cpuutilflag)
		store_times(walltime[1], cputime[1]);
	store_value((off64_t)randreadrate[1]);
#ifdef NO_PRINT_LLD
	if(!silent) printf("%8ld",randreadrate[0]);
	if(!silent) printf("%8ld",randreadrate[1]);
	if(!silent) fflush(stdout);
#else
	if(!silent) printf("%8lld",randreadrate[0]);
	if(!silent) printf("%8lld",randreadrate[1]);
	if(!silent) fflush(stdout);
#endif
}

/************************************************************************/
/* reverse_perf_test				        		*/
/* Reverse read test							*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void reverse_perf_test(off64_t kilo64,long long reclen,long long *data1,long long *data2)
#else
void reverse_perf_test(kilo64,reclen,data1,data2)
off64_t kilo64;
long long reclen;
long long *data1,*data2;
#endif
{
	double revreadtime[2];
	double starttime2;
	double walltime[2], cputime[2];
	double compute_val = (double)0;
	long long j;
	off64_t i,numrecs64;
	long long Index = 0;
	unsigned long long revreadrate[2];
	off64_t filebytes64;
	int fd,open_flags;
	char *maddr,*wmaddr;
	volatile char *buffer1;
#ifdef ASYNC_IO
	struct cache *gc=0;
#else
	long long *gc=0;
#endif

	open_flags=O_RDONLY;
#if defined(linux)
	if(direct_flag)
		open_flags |=O_DIRECT;
#endif
	numrecs64 = (kilo64*1024)/reclen;
	filebytes64 = numrecs64*reclen;
	fd = 0;
	for( j=0; j<2; j++ )
	{
		if(cpuutilflag)
		{
			walltime[j] = time_so_far();
			cputime[j]  = cputime_so_far();
		}
		if(Uflag) /* Unmount and re-mount the mountpoint */
		{
			purge_buffer_cache();
		}
#ifdef _LARGEFILE64_SOURCE
	 	if((fd = open64(filename, (int)O_RDONLY))<0){
	 		printf("\nCan not open temporary file for read\n");
	 		perror("open");
	 		exit(75);
	 	}
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
		if(async_flag)
			async_init(&gc,fd,direct_flag);
#else
		if(async_flag)
			async_init(&gc,fd,0);
#endif
#endif
#ifdef VXFS
		if(direct_flag)
			ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif

#else
	 	if((fd = open(filename, open_flags))<0){
	 		printf("\nCan not open temporary file for read\n");
	 		perror("open");
	 		exit(76);
	 	}
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
		if(async_flag)
			async_init(&gc,fd,direct_flag);
#else
		if(async_flag)
			async_init(&gc,fd,0);
#endif
#endif
#ifdef VXFS
		if(direct_flag)
			ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif

#endif
		if(mmapflag)
		{
			maddr=(char *)initfile(fd,filebytes64,0,PROT_READ);
		}
		fsync(fd);
		buffer=mainbuffer;
		mbuffer=mainbuffer;
		if(fetchon)
			fetchit(buffer,reclen);
		starttime2 = time_so_far();
#ifdef _LARGEFILE64_SOURCE
		if (!(h_flag || k_flag || mmapflag))
		{
		  if(lseek64( fd, (off64_t)-reclen, SEEK_END )<0)
		  {
			perror("lseek64");
			exit(77);
		  };
		}
#else
		if (!(h_flag || k_flag || mmapflag))
		{
		  if(lseek( fd, (off_t)-reclen, SEEK_END )<0)
		  {
			perror("lseek");
			exit(78);
		  };
		}
#endif
		for(i=0; i<numrecs64; i++) 
		{
			if(compute_flag)
				compute_val+=do_compute(compute_time);
                        if(multi_buffer)
                        {
                                Index +=reclen;
                                if(Index > (MAXBUFFERSIZE-reclen))
                                        Index=0;
                                buffer = mbuffer + Index;
                        }

			if(purge)
				purgeit(buffer,reclen);
			if(mmapflag)
			{
				wmaddr = &maddr[((numrecs64-1)-i)*reclen];
				fill_area((long long*)wmaddr,(long long*)buffer,(long long)reclen);
			}
			else
			if(async_flag)
			{
			    if(no_copy_flag)
			       async_read_no_copy(gc, (long long)fd, &buffer1, ((((numrecs64-1)-i)*reclen)),
			          reclen, -1LL,(numrecs64*reclen),depth);
			    else
			       async_read(gc, (long long)fd, buffer, (((numrecs64-1)-i)*reclen),
			       	  reclen,-1LL,(numrecs64*reclen),depth);
			}else
			{
				if(read((int)fd, (void*)buffer, (size_t) reclen) != reclen)
				{
#ifdef NO_PRINT_LLD
					printf("\nError reading block %ld\n", i); 
#else
					printf("\nError reading block %lld\n", i); 
#endif
					perror("read");
					exit(79);
				}
			}
			if(verify){
			   if(async_flag && no_copy_flag)
			   {
				if(verify_buffer(buffer1,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
					exit(80);
				}
			   }
			   else
			   {
				if(verify_buffer(buffer,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
					exit(81);
				}
			   }
			}
			if(async_flag && no_copy_flag)
				async_release(gc);
			if (!(h_flag || k_flag || mmapflag))
			{
#ifdef _LARGEFILE64_SOURCE
			  lseek64( fd, (off64_t)-2*reclen, SEEK_CUR );
#else
			  lseek( fd, (off_t)(-2*reclen), SEEK_CUR );
#endif
			}
		}
#ifdef ASYNC_IO
		if(async_flag)
		{
			end_async(gc);
			gc=0;
		}
#endif
		if(include_flush)	
		{
			if(mmapflag)
				msync(maddr,(size_t)filebytes64,MS_SYNC);
			else
				fsync(fd);
		}
		if(include_close)
		{
			if(mmapflag)
			{
				mmap_end(maddr,(unsigned long long)filebytes64);
			}
			close(fd);
		}
		revreadtime[j] = ((time_so_far() - starttime2)-time_res)
			-compute_val;
		if(revreadtime[j] < (double).000001) 
		{
			revreadtime[j]= time_res;
			if(rec_prob < reclen)
				rec_prob = reclen;
			res_prob=1;
		}
		if(!include_close)
		{
			if(mmapflag)
				msync(maddr,(size_t)filebytes64,MS_SYNC);
			else
				fsync(fd);
			if(mmapflag)
			{
				mmap_end(maddr,(unsigned long long)filebytes64);
			}
			close(fd);
		}
		if(cpuutilflag)
		{
			walltime[j] = time_so_far() - walltime[j];
			if (walltime[j] < cputime_res)
				walltime[j] = 0.0;
			cputime[j]  = cputime_so_far() - cputime[j];
			if (cputime[j] < cputime_res)
				cputime[j] = 0.0;
		}
        }
	if(OPS_flag || MS_flag){
	   filebytes64=filebytes64/reclen;
	}
	for(j=0;j<2;j++){
	    	revreadrate[j] = 
		      (unsigned long long) ((double) filebytes64 / revreadtime[j]);
		if(MS_flag)
		{
			revreadrate[j]=1000000.0*(1.0/revreadrate[j]);
			continue;
		}
		if(!(OPS_flag || MS_flag))
			revreadrate[j] >>= 10;
	}
	/* Must save walltime & cputime before calling store_value() for each/any cell.*/
	if(cpuutilflag)
		store_times(walltime[0], cputime[0]);
	store_value((off64_t)revreadrate[0]);
#ifdef NO_PRINT_LLD
	if(!silent) printf("%8ld",revreadrate[0]);
#else
	if(!silent) printf("%8lld",revreadrate[0]);
#endif
	if(!silent) fflush(stdout);
}

/************************************************************************/
/* rewriterec_perf_test				        		*/
/* Re-write the same record 						*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void rewriterec_perf_test(off64_t kilo64 ,long long reclen,long long *data1,long long *data2)
#else
void rewriterec_perf_test(kilo64 ,reclen,data1,data2)
off64_t kilo64;
long long reclen;
long long *data1,*data2;
#endif
{
	double writeintime;
	double starttime1;
	double walltime, cputime;
	double compute_val = (double)0;
	long long i;
	off64_t numrecs64;
	long long flags;
	long long Index=0;
	unsigned long long writeinrate;
	off64_t filebytes64;
	int fd,wval;
	char *maddr;
	char *wmaddr,*free_addr;
#ifdef ASYNC_IO
	struct cache *gc=0;
#else
	long long *gc=0;
#endif

	numrecs64 = (kilo64*1024)/reclen;
	filebytes64 = numrecs64*reclen;
	flags = O_RDWR|O_CREAT|O_TRUNC;
#if defined(linux)
	if(direct_flag)
		flags |=O_DIRECT;
#endif
	if(oflag)
		flags |= O_SYNC;
	if (!no_unlink)
		unlink(dummyfile[0]);
	if(Uflag) /* Unmount and re-mount the mountpoint */
	{
		purge_buffer_cache();
	}
#ifdef _LARGEFILE64_SOURCE
        if((fd = open64(dummyfile[0], (int)flags,0640))<0)
        {
                    printf("\nCan not open temporary file %s for write.\n",dummyfile[0]);
		    perror("open");
                    exit(84);
        }
#else
        if((fd = open(dummyfile[0], (int)flags, 0640))<0)
        {
                    printf("\nCan not open temporary file %s for write.\n",dummyfile[0]);
		    perror("open");
                    exit(85);
        }
#endif
#ifdef VXFS
	if(direct_flag)
		ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
	if(mmapflag)
	{
		maddr=(char *)initfile(fd,filebytes64,1,PROT_READ|PROT_WRITE);
	}
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
	if(async_flag)
		async_init(&gc,fd,direct_flag);
#else
	if(async_flag)
		async_init(&gc,fd,0);
#endif
#endif
	fsync(fd);
	buffer=mainbuffer;
	mbuffer=mainbuffer;
	if(fetchon)
		fetchit(buffer,reclen);
	/*
	wval=write(fd, buffer, (size_t) reclen);
	if(wval != reclen)
	{
#ifdef NO_PRINT_LLD
	    	printf("\nError writing block %ld, fd= %d\n", 0, fd);
#else
	    	printf("\nError writing block %lld, fd= %d\n", 0, fd);
#endif
		if(wval==-1)
			perror("write");
		signal_handler();
	}
	*/
	starttime1 = time_so_far();
	if(cpuutilflag)
	{
		walltime = time_so_far();
		cputime  = cputime_so_far();
	}
	for(i=0; i<numrecs64; i++){
		if(compute_flag)
			compute_val+=do_compute(compute_time);
        	if(multi_buffer)
        	{
                	Index +=reclen;
                        if(Index > (MAXBUFFERSIZE-reclen))
                                Index=0;
                        buffer = mbuffer + Index;
                }
		if(async_flag && no_copy_flag)
		{
			free_addr=buffer=(char *)malloc((size_t)reclen+page_size);
			buffer=(char *)(((long)buffer+(long)page_size) & (long)(~page_size-1));
		}
		if(verify)
			fill_buffer(buffer,reclen,(long long)pattern,sverify);
		if(purge)
			purgeit(buffer,reclen);
		if(mmapflag)
		{
			wmaddr = &maddr[0];
			fill_area((long long*)buffer,(long long*)wmaddr,(long long)reclen);
			if(!mmapnsflag)
			{
			  if(mmapasflag)
			    msync(wmaddr,(size_t)reclen,MS_ASYNC);
			  if(mmapssflag)
			    msync(wmaddr,(size_t)reclen,MS_SYNC);
			}
		}
		else
		{
			  if(async_flag)
			  {
			     if(no_copy_flag)
			       async_write_no_copy(gc, (long long)fd, buffer, reclen, (i*reclen), depth,free_addr);
			     else
			       async_write(gc, (long long)fd, buffer, reclen, (i*reclen), depth);
			  }
			  else
			  {
			       wval=write(fd, buffer, (size_t) reclen);
			       if(wval != reclen)
			       {
#ifdef NO_PRINT_LLD
		    		   printf("\nError writing block %ld, fd= %d\n", i, fd);
#else
		    		   printf("\nError writing block %lld, fd= %d\n", i, fd);
#endif
				   if(wval==-1)
				   	perror("write");
				   signal_handler();
			       }
			  }
		}
		if (!(h_flag || k_flag || mmapflag))
		{
#ifdef _LARGEFILE64_SOURCE
		  lseek64(fd, (off64_t)0,SEEK_SET);
#else
		  lseek(fd, 0,SEEK_SET);
#endif
		}
	}

#ifdef ASYNC_IO
	if(async_flag)
	{
		end_async(gc);
		gc=0;
	}
#endif
	if(include_flush)
	{
		if(mmapflag)
			msync(maddr,(size_t)filebytes64,MS_SYNC);/* Clean up before read starts running */
		else
			fsync(fd);
	}
	if(include_close)
	{
		if(mmapflag)
			mmap_end(maddr,(unsigned long long)filebytes64);
		close(fd);
	}
	writeintime = ((time_so_far() - starttime1)-time_res)-
		compute_val;
	if(cpuutilflag)
	{
		walltime = time_so_far() - walltime;
		if (walltime < cputime_res)
			walltime = 0.0;
		cputime  = cputime_so_far() - cputime;
		if (cputime < cputime_res)
			cputime = 0.0;
	}
	if(writeintime < (double).000001) 
	{
		writeintime= time_res;
		if(rec_prob < reclen)
			rec_prob = reclen;
		res_prob=1;
	}

	if(!include_close)
	{
		if(mmapflag)
			msync(maddr,(size_t)filebytes64,MS_SYNC);/* Clean up before read starts running */
		else
			fsync(fd);
		if(mmapflag)
			mmap_end(maddr,(unsigned long long)filebytes64);
		close(fd);
	}

	if(OPS_flag || MS_flag){
	   filebytes64=filebytes64/reclen;
	}
    	writeinrate = (unsigned long long) ((double) filebytes64 / writeintime);
	if(MS_flag)
	{
		writeinrate=1000000.0*(1.0/writeinrate);
	}
	if(!(OPS_flag || MS_flag))
		writeinrate >>= 10;
	/* Must save walltime & cputime before calling store_value() for each/any cell.*/
	if(cpuutilflag)
		store_times(walltime, cputime);
	store_value((off64_t)writeinrate);
#ifdef NO_PRINT_LLD
	if(!silent) printf("%8ld",writeinrate);
#else
	if(!silent) printf("%8lld",writeinrate);
#endif
	if(!silent) fflush(stdout);
}

/************************************************************************/
/* read_stride_perf_test			        		*/
/* Read with a constant stride test					*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void read_stride_perf_test(off64_t kilos64,long long reclen,long long *data1,long long *data2)
#else
void read_stride_perf_test(kilos64,reclen,data1,data2)
off64_t kilos64;
long long reclen;
long long *data1, *data2;
#endif
{
	double strideintime;
	double starttime1;
	double compute_val = (double)0;
	double walltime, cputime;
	off64_t i,xx;
	off64_t numrecs64,current_position;
	long long Index = 0;
	off64_t savepos64 = 0;
	unsigned long long strideinrate;
	off64_t filebytes64;
	long long uu;
	off64_t internal_offset = 0;
	off64_t stripewrap=0;
	int fd,open_flags;
	volatile char *buffer1;
	char *maddr;
	char *wmaddr;
#ifdef ASYNC_IO
	struct cache *gc=0;
#else
	long long *gc=0;
#endif

	open_flags=O_RDONLY;
#if defined(linux)
	if(direct_flag)
		open_flags |=O_DIRECT;
#endif
	next64 = (off64_t)0;
	numrecs64 = (kilos64*1024)/reclen;
	filebytes64 = numrecs64*reclen;
	if(Uflag) /* Unmount and re-mount the mountpoint */
	{
		purge_buffer_cache();
	}
#ifdef _LARGEFILE64_SOURCE
        if((fd = open64(filename, (int)open_flags, 0640))<0)
        {
                    printf("\nCan not open temporary file for read\n");
		    perror("open");
                    exit(86);
        }
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
	if(async_flag)
		async_init(&gc,fd,direct_flag);
#else
	if(async_flag)
		async_init(&gc,fd,0);
#endif
#endif
#ifdef VXFS
	if(direct_flag)
		ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#else
        if((fd = open(filename, open_flags, 0640))<0)
        {
                    printf("\nCan not open temporary file for read\n");
		    perror("open");
                    exit(87);
        }
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
	if(async_flag)
		async_init(&gc,fd,direct_flag);
#else
	if(async_flag)
		async_init(&gc,fd,0);
#endif
#endif
#ifdef VXFS
	if(direct_flag)
		ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif

#endif
	if(mmapflag)
	{
		maddr=(char *)initfile(fd,filebytes64,0,PROT_READ);
	}
	fsync(fd);
	current_position=0;
	buffer=mainbuffer;
	mbuffer=mainbuffer;
	if(fetchon)
		fetchit(buffer,reclen);
	starttime1 = time_so_far();
	if(cpuutilflag)
	{
		walltime = time_so_far();
		cputime  = cputime_so_far();
	}
	for(i=0; i<numrecs64; i++){
		if(compute_flag)
			compute_val+=do_compute(compute_time);
        	if(multi_buffer)
                {
                       Index +=reclen;
                       if(Index > (MAXBUFFERSIZE-reclen))
                                Index=0;
                       buffer = mbuffer + Index;
                }
		if(purge)
			purgeit(buffer,reclen);
		if(verify)
		{
			savepos64=current_position/reclen;
		}
		if(mmapflag)
		{
			wmaddr = &maddr[current_position];
			fill_area((long long*)wmaddr,(long long*)buffer,(long long)reclen);
		}
		else
		{
			if(async_flag)
			{
			    if(no_copy_flag)
			      async_read_no_copy(gc, (long long)fd, &buffer1, current_position,
			      	reclen, stride,(numrecs64*reclen),depth);
			    else
			       async_read(gc, (long long)fd, buffer, current_position, reclen,
			    	 stride,(numrecs64*reclen),depth);
		   	}
			else
			{
		   	  if((uu=read((int)fd, (void*)buffer, (size_t) reclen)) != reclen)
		   	  {
#ifdef NO_PRINT_LLD
		    		printf("\nError reading block %ld, fd= %d Filename %s Read returned %ld\n", i, fd,filename,uu);
		    		printf("\nSeeked to %ld Reclen = %ld\n", savepos64,reclen);
#else
		    		printf("\nError reading block %lld, fd= %d Filename %s Read returned %lld\n", i, fd,filename,uu);
		    		printf("\nSeeked to %lld Reclen = %lld\n", savepos64,reclen);
#endif
				perror("read");
		    		exit(88);
		   	  }
			}
		}
		current_position+=reclen;
		if(verify){
			if(async_flag && no_copy_flag)
			{
			   if(verify_buffer(buffer1,reclen, (off64_t)savepos64 ,reclen,(long long)pattern,sverify)){
				exit(89);
			   }
			}
			else
			{
			   if(verify_buffer(buffer,reclen, (off64_t)savepos64 ,reclen,(long long)pattern,sverify)){
				exit(90);
			   }
			}
		}
		if(async_flag && no_copy_flag)
			async_release(gc);
			
		/* This is a bit tricky.  The goal is to read with a stride through
		   the file. The problem is that you need to touch all of the file
		   blocks. So.. the first pass through you read with a constant stride.
		   When you hit eof then add 1 to the beginning offset of the next
		   time through the file. The rub here is that eventually adding
		   1 will cause the initial start location plus the STRIDE to be
		   beyond eof. So... when this happens the initial offset for the
		   next pass needs to be set back to 0.
		*/
		if(current_position + (stride * reclen) >= (numrecs64 * reclen)-reclen) 
		{
			current_position=0;

			stripewrap++;
			  
			if(numrecs64 <= stride)
			{
				current_position=0;
			}
			else
			{
#ifdef _LARGEFILE64_SOURCE
				current_position = (off64_t)((stripewrap)%numrecs64)*reclen;
#else
				current_position = (long long)((stripewrap)%numrecs64)*reclen;
#endif
			}

			if (!(h_flag || k_flag || mmapflag))
			{
#ifdef _LARGEFILE64_SOURCE
			  if(lseek64(fd,current_position,SEEK_SET)<0)
			  {
				perror("lseek64");
				exit(91);
			  }
#else
			  if(lseek(fd,(off_t )current_position,SEEK_SET)<0)
			  {
				perror("lseek");
				exit(92);
			  }
#endif
			}
		}
		else			
		{
			current_position+=(stride*reclen)-reclen;
			if (!(h_flag || k_flag || mmapflag))
			{
#ifdef _LARGEFILE64_SOURCE
			  if(lseek64(fd,(off64_t)current_position,SEEK_SET)<0)
			  {
				perror("lseek64");
				exit(93);
			  };
#else
			  if(lseek(fd,(off_t )current_position,SEEK_SET)<0)
			  {
				perror("lseek");
				exit(94);
			  };
#endif
			}
		}
	}
	if(cpuutilflag)
	{
		walltime = time_so_far() - walltime;
		if (walltime < cputime_res)
			walltime = 0.0;
		cputime  = cputime_so_far() - cputime;
		if (cputime < cputime_res)
			cputime = 0.0;
	}

#ifdef ASYNC_IO
	if(async_flag)
	{
		end_async(gc);
		gc=0;
	}
#endif
	if(include_flush)
	{
		if(mmapflag)
			msync(maddr,(size_t)filebytes64,MS_SYNC);
		else
			fsync(fd);
	}
	if(include_close)
	{
		if(mmapflag)
		{
			mmap_end(maddr,(unsigned long long)filebytes64);
		}
		close(fd);
	}
	strideintime = ((time_so_far() - starttime1)-time_res)
		-compute_val;
	if(strideintime < (double).000001) 
	{
		strideintime= time_res;
		if(rec_prob < reclen)
			rec_prob = reclen;
		res_prob=1;
	}

	if(!include_close)
	{
		if(mmapflag)
			msync(maddr,(size_t)filebytes64,MS_SYNC);
		else
			fsync(fd);
		if(mmapflag)
		{
			mmap_end(maddr,(unsigned long long)filebytes64);
		}
		close(fd);
	}

	if(OPS_flag || MS_flag){
	   filebytes64=filebytes64/reclen;
	}
    	strideinrate = (unsigned long long) ((double) filebytes64 / strideintime);
	if(MS_flag)
	{
		strideinrate=1000000.0*(1.0/strideinrate);
	}
	if(!(OPS_flag || MS_flag))
		strideinrate >>= 10;
	/* Must save walltime & cputime before calling store_value() for each/any cell.*/
	if(cpuutilflag)
		store_times(walltime, cputime);
	store_value((off64_t)strideinrate);
#ifdef NO_PRINT_LLD
	if(!silent) printf("%8ld",strideinrate);
#else
	if(!silent) printf("%8lld",strideinrate);
#endif
	if(!silent) fflush(stdout);
}

#ifdef HAVE_PREAD
/************************************************************************/
/* pwrite_perf_test				        		*/
/* pwrite and re-write test						*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void pwrite_perf_test(off64_t kilos64,long long reclen,long long *data1,long long *data2)
#else
void pwrite_perf_test(kilos64,reclen,data1,data2)
off64_t kilos64;
long long reclen;
long long *data1,*data2;
#endif
{
	double pwritetime[2];
	double starttime1;
	double walltime[2], cputime[2];
	double compute_val = (double)0;
	long long i,j;
	long long Index = 0;
	unsigned long long pwriterate[2];
	off64_t filebytes64;
	long long flags_here = 0;
	int fd;

	numrecs64 = (kilos64*1024)/reclen;
	filebytes64 = numrecs64*reclen;
	fd = 0;
	if(oflag){
		flags_here = O_SYNC|O_RDWR;
	}
	else
	{
		flags_here = O_RDWR;
	}
#if defined(linux)
	if(direct_flag)
		flags_here |=O_DIRECT;
#endif
	for( j=0; j<2; j++)
	{
		if(cpuutilflag)
		{
			walltime[j] = time_so_far();
			cputime[j]  = cputime_so_far();
		}
		if(Uflag) /* Unmount and re-mount the mountpoint */
		{
			purge_buffer_cache();
		}
		if( j==0 )
		{
#ifdef _LARGEFILE64_SOURCE
		  	if((fd = creat64(filename, 0640))<0)
		  	{
				printf("\nCan not create temp file: %s\n", 
					filename);
				perror("creat");
				exit(95);
		  	}
#else
		  	if((fd = creat(filename, 0640))<0)
		  	{
				printf("\nCan not create temp file: %s\n", 
					filename);
				perror("creat");
				exit(96);
		  	}
#endif
#ifdef _LARGEFILE64_SOURCE
			if((fd = open64(filename, (int)flags_here))<0)
			{
				printf("\nCan not open temp file: %s\n", 
					filename);
				perror("open");
				exit(97);
			}
#ifdef VXFS
			if(direct_flag)
				ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#else
			if((fd = open(filename, (int)flags_here))<0)
			{
				printf("\nCan not open temp file: %s\n", 
					filename);
				perror("open");
				exit(98);
			}
#ifdef VXFS
			if(direct_flag)
				ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#endif
		}
		else
		{
#ifdef _LARGEFILE64_SOURCE
			  if((fd = open64(filename, (int)flags_here))<0)
			  {
				printf("\nCan not open temp file: %s\n", 
					filename);
				perror("open");
				exit(99);
			  }
#ifdef VXFS
			  if(direct_flag)
				ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#else
			  if((fd = open(filename, (int)flags_here))<0)
			  {
				printf("\nCan not open temp file: %s\n", 
					filename);
				perror("open");
				exit(100);
			  }
#ifdef VXFS
			  if(direct_flag)
				ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#endif
		}
		buffer=mainbuffer;
		mbuffer=mainbuffer;
		if(fetchon)
			fetchit(buffer,reclen);
		starttime1 = time_so_far();
		for(i=0; i<numrecs64; i++){
			if(compute_flag)
				compute_val+=do_compute(compute_time);
                        if(multi_buffer)
                        {
                                Index +=reclen;
                                if(Index > (MAXBUFFERSIZE-reclen))
                                        Index=0;
                                buffer = mbuffer + Index;
                        }
			if(verify)
				fill_buffer(buffer,reclen,(long long)pattern,sverify);
			if(purge)
				purgeit(buffer,reclen);
			if(pwrite(fd, buffer, (size_t) reclen,
				(i * reclen )) != reclen)
			{
#ifdef NO_PRINT_LLD
			    	printf("\nError pwriting block %ld, fd= %d\n", i,
					 fd);
#else
			    	printf("\nError pwriting block %lld, fd= %d\n", i,
					 fd);
#endif
				perror("pwrite");
				signal_handler();
			}
		}
		if(include_flush)
		{
			fsync(fd);
		}
		if(include_close)
		{
			close(fd);
		}
		pwritetime[j] = ((time_so_far() - starttime1)-time_res)
			-compute_val;
		if(pwritetime[j] < (double).000001) 
		{
			pwritetime[j]= time_res;
			if(rec_prob < reclen)
				rec_prob = reclen;
			res_prob=1;
		}
		if(!include_close)
		{
			fsync(fd);
			close(fd);
		}

		if(cpuutilflag)
		{
			walltime[j] = time_so_far() - walltime[j];
			if (walltime[j] < cputime_res)
				walltime[j] = 0.0;
			cputime[j]  = cputime_so_far() - cputime[j];
			if (cputime[j] < cputime_res)
				cputime[j] = 0.0;
		}
	}
	if(OPS_flag || MS_flag){
	   filebytes64=filebytes64/reclen;
	}
	for(j=0;j<2;j++){
		pwriterate[j] = 
		      (unsigned long long) ((double) filebytes64 / pwritetime[j]);
		if(MS_flag)
		{
			pwriterate[j]=1000000.0*(1.0/pwriterate[j]);
			continue;
		}
		if(!(OPS_flag || MS_flag))
			pwriterate[j] >>= 10;
	}
	/* Must save walltime & cputime before calling store_value() for each/any cell.*/
	if(cpuutilflag)
		store_times(walltime[0], cputime[0]);
	store_value((off64_t)pwriterate[0]);
	if(cpuutilflag)
		store_times(walltime[1], cputime[1]);
	store_value((off64_t)pwriterate[1]);
#ifdef NO_PRINT_LLD
	if(!silent) printf("%8ld",pwriterate[0]);
	if(!silent) printf("%9ld",pwriterate[1]);
	if(!silent) fflush(stdout);
#else
	if(!silent) printf("%8lld",pwriterate[0]);
	if(!silent) printf("%9lld",pwriterate[1]);
	if(!silent) fflush(stdout);
#endif
}

/************************************************************************/
/* pread_perf_test				        		*/
/* pread and re-pread test						*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void pread_perf_test(off64_t kilos64,long long reclen,long long *data1,long long *data2)
#else
void pread_perf_test(kilos64,reclen,data1,data2)
off64_t kilos64;
long long reclen;
long long *data1, *data2;
#endif
{
	double starttime2;
	double preadtime[2];
	double walltime[2], cputime[2];
	double compute_val = (double)0;
	long long numrecs64,i;
	long long j;
	long long Index = 0;
	unsigned long long preadrate[2];
	off64_t filebytes64;
	int fd,open_flags;

	open_flags=O_RDONLY;
#if defined(linux)
	if(direct_flag)
		open_flags |=O_DIRECT;
#endif
	numrecs64 = (kilos64*1024)/reclen;
	filebytes64 = numrecs64*reclen;
	fd = 0;
	for( j=0; j<2; j++ ) 		/* Pread and Re-Pread */
	{
		if(cpuutilflag)
		{
			walltime[j] = time_so_far();
			cputime[j]  = cputime_so_far();
		}
		if(Uflag) /* Unmount and re-mount the mountpoint */
		{
			purge_buffer_cache();
		}
#ifdef _LARGEFILE64_SOURCE
		if((fd = open64(filename, (int)open_flags))<0)
		{
			printf("\nCan not open temporary file for read\n");
			perror("open");
			exit(101);
		}
#ifdef VXFS
		if(direct_flag)
			ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#else
		if((fd = open(filename, open_flags))<0)
		{
			printf("\nCan not open temporary file for read\n");
			perror("open");
			exit(102);
		}
#ifdef VXFS
		if(direct_flag)
			ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#endif
		buffer=mainbuffer;
		mbuffer=mainbuffer;
		if(fetchon)
			fetchit(buffer,reclen);
		starttime2 = time_so_far();
		for(i=0; i<numrecs64; i++) 
		{
			if(compute_flag)
				compute_val+=do_compute(compute_time);
                        if(multi_buffer)
                        {
                                Index +=reclen;
                                if(Index > (MAXBUFFERSIZE-reclen))
                                        Index=0;
                                buffer = mbuffer + Index;
                        }

			if(purge)
				purgeit(buffer,reclen);
			if(pread((int)fd, (void*)buffer, (size_t) reclen,(i * reclen) ) 
					!= reclen)
			{
#ifdef NO_PRINT_LLD
				printf("\nError reading block %ld %x\n", i,buffer);
#else
				printf("\nError reading block %lld %x\n", i,buffer);
#endif
				perror("pread");
				exit(103);
			}
			if(verify){
				if(verify_buffer(buffer,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
					exit(104);
				}
			}
		}
		if(include_flush)
			fsync(fd);
		if(include_close)
			close(fd);
		preadtime[j] = ((time_so_far() - starttime2)-time_res)
			-compute_val;
		if(preadtime[j] < (double).000001) 
		{
			preadtime[j]= time_res;
			if(rec_prob < reclen)
				rec_prob = reclen;
			res_prob=1;
		}
		if(!include_close)
		{
			fsync(fd);
			close(fd);
		}

		if(cpuutilflag)
		{
			walltime[j] = time_so_far() - walltime[j];
			if (walltime[j] < cputime_res)
				walltime[j] = 0.0;
			cputime[j]  = cputime_so_far() - cputime[j];
			if (cputime[j] < cputime_res)
				cputime[j] = 0.0;
		}
    	}

	filebytes64 = numrecs64*reclen;
	if(OPS_flag || MS_flag){
	   filebytes64=filebytes64/reclen;
	}
	for(j=0;j<2;j++){
		preadrate[j] = 
			(unsigned long long) ((double) filebytes64 / preadtime[j]);
		if(MS_flag)
		{
			preadrate[j]=1000000.0*(1.0/preadrate[j]);
			continue;
		}
		if(!(OPS_flag || MS_flag))
			preadrate[j] >>= 10;
	}
	/* Must save walltime & cputime before calling store_value() for each/any cell.*/
	if(cpuutilflag)
		store_times(walltime[0], cputime[0]);
	store_value((off64_t)preadrate[0]);
	if(cpuutilflag)
		store_times(walltime[1], cputime[1]);
	store_value((off64_t)preadrate[1]);
#ifdef NO_PRINT_LLD
	if(!silent) printf("%8ld",preadrate[0]);
	if(!silent) printf("%9ld",preadrate[1]);
	if(!silent) fflush(stdout);
#else
	if(!silent) printf("%8lld",preadrate[0]);
	if(!silent) printf("%9lld",preadrate[1]);
	if(!silent) fflush(stdout);
#endif
}

/************************************************************************/
/* pwritev_perf_test				        		*/
/* pwritev and re-pwritev test						*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void pwritev_perf_test(off64_t kilos64,long long reclen,long long *data1,long long *data2)
#else
void pwritev_perf_test(kilos64,reclen,data1,data2)
off64_t kilos64;
long long reclen;
long long *data1,*data2;
#endif
{
	double starttime1;
	double pwritevtime[2];
	double walltime[2], cputime[2];
	double compute_val = (double)0;
	long long list_off[PVECMAX];
	long long numvecs,j,xx;
	unsigned long long pwritevrate[2];
	off64_t filebytes64,i;
	off64_t numrecs64;
	int fd;
	long long flags_here;

	numrecs64 = (kilos64*1024)/reclen;
	filebytes64 = numrecs64*reclen;
	buffer = mainbuffer;
	fd = 0;
	if(oflag)
		flags_here = O_SYNC|O_RDWR;
	else
		flags_here = O_RDWR;
#if defined(linux)
	if(direct_flag)
		flags_here |=O_DIRECT;
#endif
	 
	for( j=0; j<2; j++)
	{
		if(cpuutilflag)
		{
			walltime[j] = time_so_far();
			cputime[j]  = cputime_so_far();
		}
		if(Uflag) /* Unmount and re-mount the mountpoint */
		{
			purge_buffer_cache();
		}
		if( j==0 )
		{
#ifdef _LARGEFILE64_SOURCE
		  	if((fd = creat64(filename, 0640))<0)
		  	{
				printf("\nCan not create temp file: %s\n", 
					filename);
				perror("creat");
				exit(107);
		  	}
#else
		  	if((fd = creat(filename, 0640))<0)
		  	{
				printf("\nCan not create temp file: %s\n", 
					filename);
				perror("creat");
				exit(108);
		  	}
#endif
#ifdef _LARGEFILE64_SOURCE
			if((fd = open64(filename, (int)flags_here))<0)
			{
				printf("\nCan not open temp file: %s\n", 
					filename);
				perror("open");
				exit(109);
			}
#ifdef VXFS
			if(direct_flag)
				ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#else
			if((fd = open(filename, (int)flags_here))<0)
			{
				printf("\nCan not open temp file: %s\n", 
					filename);
				perror("open");
				exit(110);
			}
#ifdef VXFS
			if(direct_flag)
				ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#endif
		}
		else
		{
#ifdef _LARGEFILE64_SOURCE
			if((fd = open64(filename, (int)flags_here))<0)
			{
				printf("\nCan not open temp file: %s\n", 
					filename);
				perror("open");
				exit(111);
			}
#ifdef VXFS
			if(direct_flag)
				ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#else
			if((fd = open(filename, (int)flags_here))<0)
			{
				printf("\nCan not open temp file: %s\n", 
					filename);
				perror("open");
				exit(112);
			}
#ifdef VXFS
			if(direct_flag)
				ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#endif
		}
		buffer=mainbuffer;
		mbuffer=mainbuffer;
		if(fetchon)
			fetchit(buffer,reclen);
		numvecs=PVECMAX;
                if(numrecs64 < numvecs) numvecs=numrecs64;
                if(MAXBUFFERSIZE/reclen < PVECMAX) numvecs=MAXBUFFERSIZE/reclen;

#ifdef bsd4_2
	        srand(0);
#else
#ifdef Windows
                srand(0);
#else
	        srand48(0);
#endif
#endif
		starttime1 = time_so_far();
		for(i=0; i<numrecs64; i+=numvecs){
			if(compute_flag)
				compute_val+=do_compute(compute_time);
			if((numrecs64-i) < numvecs) 
				numvecs=numrecs64-i;
			create_list((long long *)list_off, reclen, numrecs64);
			for(xx=0;xx<numvecs;xx++)
			{
				piov[xx].piov_base = 
					(caddr_t)(buffer+(xx * reclen));
				if(verify)
					fill_buffer(piov[xx].piov_base,reclen,(long long)pattern,sverify);
				piov[xx].piov_len = reclen;
				piov[xx].piov_offset = list_off[xx];
				if(purge)
					purgeit(piov[xx].piov_base,reclen);
			}
			if(pwritev(fd, piov,numvecs) != (reclen*numvecs))
			{
#ifdef NO_PRINT_LLD
			    	printf("\nError pwriteving block %ld, fd= %d\n", i,
					 fd);
#else
			    	printf("\nError pwriteving block %lld, fd= %d\n", i,
					 fd);
#endif
				perror("pwritev");
			    	exit(113);
			}
		}

		if(include_flush)
			fsync(fd);
		if(include_close)
			close(fd);
		pwritevtime[j] = ((time_so_far() - starttime1)-time_res)
			-compute_val;
		if(pwritevtime[j] < (double).000001) 
		{
			pwritevtime[j]= time_res;
			if(rec_prob < reclen)
				rec_prob = reclen;
			res_prob=1;
		}
		if(!include_close)
		{
			fsync(fd);
			close(fd);
		}

		if(cpuutilflag)
		{
			walltime[j] = time_so_far() - walltime[j];
			if (walltime[j] < cputime_res)
				walltime[j] = 0.0;
			cputime[j]  = cputime_so_far() - cputime[j];
			if (cputime[j] < cputime_res)
				cputime[j] = 0.0;
		}
	}
	if(OPS_flag || MS_flag){
	   filebytes64=filebytes64/reclen;
	}
	for(j=0;j<2;j++){
	    	pwritevrate[j] = 
		      (unsigned long long) ((double) filebytes64 / pwritevtime[j]);
		if(MS_flag)
		{
			pwritevrate[j]=1000000.0*(1.0/pwritevrate[j]);
			continue;
		}
		if(!(OPS_flag || MS_flag))
			pwritevrate[j] >>= 10;
	}
	/* Must save walltime & cputime before calling store_value() for each/any cell.*/
	if(cpuutilflag)
		store_times(walltime[0], cputime[0]);
	store_value((off64_t)pwritevrate[0]);
	if(cpuutilflag)
		store_times(walltime[1], cputime[1]);
	store_value((off64_t)pwritevrate[1]);
#ifdef NO_PRINT_LLD
	if(!silent) printf("%9ld",pwritevrate[0]);
	if(!silent) printf("%10ld",pwritevrate[1]);
	if(!silent) fflush(stdout);
#else
	if(!silent) printf("%9lld",pwritevrate[0]);
	if(!silent) printf("%10lld",pwritevrate[1]);
	if(!silent) fflush(stdout);
#endif
}


/**************************************************************************/
/* create_list() 							  */
/* Creates a list of PVECMAX entries that are unique (non over lapping ). */
/* Each of these offsets are then used in a vector (preadv/pwritev)	  */
/**************************************************************************/
#ifdef HAVE_ANSIC_C
void create_list(long long *list_off, long long reclen, off64_t numrecs64)
#else
void create_list(list_off, reclen, numrecs64)
long long *list_off;
long long reclen; 
off64_t numrecs64;
#endif
{
	off64_t offset;
	long long found,i,j;
	long long numvecs;

	numvecs = PVECMAX;
	if(numrecs64< numvecs)
		numvecs = numrecs64;
	for(j=0;j<numvecs;j++)
		list_off[j]=0;	
	for(j=0;j<numvecs;j++)
	{
again:
		found = 0;
#ifdef bsd4_2
		offset64 = reclen * (rand()%numrecs64);
#else
#ifdef Windows
                offset64 = reclen * (rand()%numrecs64);
#else
		offset64 = reclen * (lrand48()%numrecs64);
#endif
#endif

		for(i=0;i<j;i++)
		{
			if(list_off[i] == offset64)
			{
				found++;
				break;
			}
		}
		if(!found)
			list_off[j]=offset64;
		else
		{
			goto again;
		}
	}
}
/************************************************************************/
/* preadv_perf_test				        		*/
/* preadv and re-preadv test						*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void preadv_perf_test(off64_t kilos64,long long reclen,long long *data1,long long *data2)
#else
void preadv_perf_test(kilos64,reclen,data1,data2)
off64_t kilos64;
long long reclen;
long long *data1,*data2;
#endif
{
	double starttime2;
	double preadvtime[2];
	double walltime[2], cputime[2];
	double compute_val = (double)0;
	long long list_off[PVECMAX];
	long long numvecs,i,j,xx;
	off64_t numrecs64;
	unsigned long long preadvrate[2];
	off64_t filebytes64;
	int fd,open_flags;

	open_flags=O_RDONLY;
#if defined(linux)
	if(direct_flag)
		open_flags |=O_DIRECT;
#endif
	numrecs64 = (kilos64*1024)/reclen;
	filebytes64 = numrecs64*reclen;
	buffer = mainbuffer;
	fd = 0;
	for( j=0; j<2; j++ )
	{
		if(cpuutilflag)
		{
			walltime[j] = time_so_far();
			cputime[j]  = cputime_so_far();
		}
		if(Uflag) /* Unmount and re-mount the mountpoint */
		{
			purge_buffer_cache();
		}

#ifdef _LARGEFILE64_SOURCE
		if((fd = open64(filename, (int)open_flags))<0)
		{
			printf("\nCan not open temporary file for preadv\n");
			perror("open");
			exit(114);
		}
#ifdef VXFS
		if(direct_flag)
			ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#else
		if((fd = open(filename, open_flags))<0)
		{
			printf("\nCan not open temporary file for preadv\n");
			perror("open");
			exit(115);
		}
#ifdef VXFS
		if(direct_flag)
			ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#endif
		buffer=mainbuffer;
		mbuffer=mainbuffer;
		if(fetchon)
			fetchit(buffer,reclen);
		numvecs=PVECMAX;
                if(numrecs64 < numvecs) numvecs=numrecs64;
                if(MAXBUFFERSIZE/reclen < PVECMAX) numvecs=MAXBUFFERSIZE/reclen;

#ifdef bsd4_2
	        srand();
#else
#ifdef Windows
                 srand();
#else
	        srand48(0);
#endif
#endif
		starttime2 = time_so_far();
		for(i=0; i<(numrecs64); i+=numvecs) 
		{
			if(compute_flag)
				compute_val+=do_compute(compute_time);
			if((numrecs64-i) < numvecs) 
				numvecs=numrecs64-i;
			create_list((long long *)list_off, reclen, numrecs64);
			for(xx=0;xx<numvecs;xx++)
			{
				piov[xx].piov_base = 
					(caddr_t)(buffer+(xx * reclen));
				piov[xx].piov_len = reclen;
				piov[xx].piov_offset = list_off[xx];
				if(purge)
				   purgeit(piov[xx].piov_base,reclen);
			}
			if(preadv(fd, piov, numvecs) != (numvecs * reclen))
			{
#ifdef NO_PRINT_LLD
				printf("\nError preadving block %ld \n", i);
#else
				printf("\nError preadving block %lld \n", i);
#endif
				perror("preadv");
				exit(116);
			}
		}
		if(include_flush)
			fsync(fd);
		if(include_close)
			close(fd);
		preadvtime[j] = ((time_so_far() - starttime2)-time_res)
			-compute_val;
		if(preadvtime[j] < (double).000001) 
		{
			preadvtime[j]= time_res;
			if(rec_prob < reclen)
				rec_prob = reclen;
			res_prob=1;
		}
		if(!include_close)
		{
			fsync(fd);
			close(fd);
		}

		if(cpuutilflag)
		{
			walltime[j] = time_so_far() - walltime[j];
			if (walltime[j] < cputime_res)
				walltime[j] = 0.0;
			cputime[j]  = cputime_so_far() - cputime[j];
			if (cputime[j] < cputime_res)
				cputime[j] = 0.0;
		}
    	}
	if(OPS_flag || MS_flag){
	   filebytes64=filebytes64/reclen;
	}
	for(j=0;j<2;j++){
	    	preadvrate[j] = 
		      (unsigned long long) ((double) filebytes64 / preadvtime[j]);
		if(MS_flag)
		{
			preadvrate[j]=1000000.0*(1.0/preadvrate[j]);
			continue;
		}
		if(!(OPS_flag || MS_flag))
			preadvrate[j] >>= 10;
	}
	/* Must save walltime & cputime before calling store_value() for each/any cell.*/
	if(cpuutilflag)
		store_times(walltime[0], cputime[0]);
	store_value((off64_t)preadvrate[0]);
	if(cpuutilflag)
		store_times(walltime[1], cputime[1]);
	store_value((off64_t)preadvrate[1]);
#ifdef NO_PRINT_LLD
	if(!silent) printf("%10ld",preadvrate[0]);
	if(!silent) printf("%9ld",preadvrate[1]);
	if(!silent) printf("\n");
	if(!silent) fflush(stdout);
#else
	if(!silent) printf("%10lld",preadvrate[0]);
	if(!silent) printf("%9lld",preadvrate[1]);
	if(!silent) printf("\n");
	if(!silent) fflush(stdout);
#endif
}

/************************************************************************/
/* print_header()							*/
/* Prints the header for the output from Iozone.			*/
/************************************************************************/
#endif
#ifdef HAVE_ANSIC_C
void print_header(void)
#else
void print_header()
#endif
{
	if(Eflag)
	{
    	   if(!silent) printf(CONTROL_STRING2,
		" ", 
		" ",
		" ",
		" ",
		" ",
		" ",
		"random",			/*kcollins:2-5-96*/
		"random",			/*kcollins:2-5-96*/
		"bkwd",
		"record",
		"stride",
		" ",
		" ",
		" ",
		" ",
		" ",
		" ",
		" ",
		" ",
		" ",
		" ",
		" ",
		" "
		);
    	if(!silent) printf(CONTROL_STRING2,
		"KB", 
		"reclen",
		"write",
		"rewrite",
		"read",
		"reread",
		"read",				/*kcollins:2-5-96*/
		"write",			/*kcollins:2-5-96*/
		"read",
		"rewrite",
		"read",
		"fwrite",
		"frewrite",
		"fread",
		"freread",
		"pwrite",
		"repwrite",
		"pread",
		"repread",
		"pwritev",
		"repwritev",
		"preadv",
		"repreadv"
		);
	}else 
	if(RWONLYflag){				/*kcollins 8-21-96*/
    	   if(!silent) printf(CONTROL_STRING4,		/*kcollins 8-21-96*/
		" ", 				/*kcollins 8-21-96*/
		" ",				/*kcollins 8-21-96*/
		" ",				/*kcollins 8-21-96*/
		" ",				/*kcollins 8-21-96*/
		" ",				/*kcollins 8-21-96*/
		" "				/*kcollins 8-21-96*/
		);				/*kcollins 8-21-96*/
    	if(!silent) printf(CONTROL_STRING4,			/*kcollins 8-21-96*/
		"KB", 				/*kcollins 8-21-96*/
		"reclen",			/*kcollins 8-21-96*/
		"write",			/*kcollins 8-21-96*/
		"rewrite",			/*kcollins 8-21-96*/
		"read",				/*kcollins 8-21-96*/
		"reread"			/*kcollins 8-21-96*/
		);				/*kcollins 8-21-96*/
	}else{
	   if(!(mmapflag || async_flag))
	   {
    	   	if(!silent) printf(CONTROL_STRING3,
			" ", 
			" ",
			" ",
			" ",
			" ",
			" ",
			"random",			/*kcollins:2-5-96*/
			"random",			/*kcollins:2-5-96*/
			"bkwd",
			"record",
			"stride",
			"",
			"",
			"",
			""
			);
    		if(!silent) printf(CONTROL_STRING3,
			"KB", 
			"reclen",
			"write",
			"rewrite",
			"read",
			"reread",
			"read",				/*kcollins:2-5-96*/
			"write",			/*kcollins:2-5-96*/
			"read",
			"rewrite",
			"read",
			"fwrite",
			"frewrite",
			"fread",
			"freread" 
			);
		}else
		{
    	   		if(!silent) printf(CONTROL_STRING3,
				" ", 
				" ",
				" ",
				" ",
				" ",
				" ",
				"random",			/*kcollins:2-5-96*/
				"random",			/*kcollins:2-5-96*/
				"bkwd",
				"record",
				"stride",
				"",
				"",
				"",
				""
				);
    			if(!silent) printf(CONTROL_STRING3,
				"KB", 
				"reclen",
				"write",
				"rewrite",
				"read",
				"reread",
				"read",				/*kcollins:2-5-96*/
				"write",			/*kcollins:2-5-96*/
				"read",
				"rewrite",
				"read",
				"",
				"",
				"",
				"" 
				);
		}
	}
}

/************************************************************************/
/* store_value()							*/
/* Stores a value in an in memory array. Used by the report function    */
/* to re-organize the output for Excel					*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void
store_value(off64_t value)
#else
store_value(value)
off64_t value;
#endif
{
	report_array[current_x][current_y]=value;
	current_x++;
	if(current_x > max_x)
		max_x=current_x;
	if(current_y > max_y)
		max_y=current_y;
	if(max_x >= MAX_X)
	{
		printf("\nMAX_X too small\n");
		exit(117);
	}
	if(max_y >= MAX_Y)
	{
		printf("\nMAX_Y too small\n");
		exit(118);
	}
}

/************************************************************************/
/* store_times()							*/
/* Stores runtime (walltime & cputime) in a memory array.		*/
/* Used by the report function to re-organize the output for Excel	*/
/* For now, must be called immediately before calling store_value() for	*/
/* each cell.								*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void
store_times(double walltime, double cputime)
#else
store_times(walltime, cputime)
double walltime, cputime;
#endif
{
	runtimes [current_x][current_y].walltime = walltime;
	runtimes [current_x][current_y].cputime  = cputime;
	runtimes [current_x][current_y].cpuutil  = cpu_util(cputime, walltime);
}

/************************************************************************/
/* dump_report()							*/
/* Dumps the Excel report on standard output.				*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void dump_report(long long who)
#else
dump_report(who)
long long who;
#endif
{
	long long i,j;
	off64_t current_file_size;
	long long counter=0;
	off64_t rec_size;

	if(bif_flag)
		bif_column++;
	if(!silent) printf("      ");

	/* 
	 * Need to reconstruct the record size list
	 * as the crossover in -a changed the list.
	*/
	del_record_sizes();
	init_record_sizes(orig_min_rec_size, orig_max_rec_size);

	for(rec_size=get_next_record_size(0); rec_size <= orig_max_rec_size;
		rec_size=get_next_record_size(rec_size))
	{
		if (rec_size == 0) break;
		if(bif_flag)
			do_float(bif_fd,(double)(rec_size/1024),bif_row,bif_column++);
#ifdef NO_PRINT_LLD
		if(!silent) printf("  %c%ld%c",042,rec_size/1024,042);
#else
		if(!silent) printf("  %c%lld%c",042,rec_size/1024,042);
#endif
	}
	if(!silent) printf("\n");
	if(bif_flag)
	{
		bif_column=0;
		bif_row++;
	}

	current_file_size = report_array[0][0];
	if(bif_flag)
	{
		do_float(bif_fd,(double)(current_file_size),bif_row,bif_column++);
	}
#ifdef NO_PRINT_LLD
	if(!silent) printf("%c%ld%c  ",042,current_file_size,042);
#else
	if(!silent) printf("%c%lld%c  ",042,current_file_size,042);
#endif
	for(i=0;i<=max_y;i++){
		if(report_array[0][i] != current_file_size){
			if(!silent) printf("\n");
			current_file_size = report_array[0][i];
			if(bif_flag)
			{
				bif_row++;
				bif_column=0;
				do_float(bif_fd,(double)(current_file_size),bif_row,bif_column++);
			}
#ifdef NO_PRINT_LLD
			if(!silent) printf("%c%ld%c  ",042,current_file_size,042);
#else
			if(!silent) printf("%c%lld%c  ",042,current_file_size,042);
#endif
		}
		if(bif_flag)
			do_float(bif_fd,(double)(report_array[who][i]),bif_row,bif_column++);
#ifdef NO_PRINT_LLD
		if(!silent) printf(" %ld ",report_array[who][i]);
#else
		if(!silent) printf(" %lld ",report_array[who][i]);
#endif
	}
	if(bif_flag)
	{
		bif_row++;
		bif_column=0;
	}
	if(!silent) printf("\n");
}

/************************************************************************/
/* Wrapper that dumps each of the collected data sets.			*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void dump_excel(void)
#else
void dump_excel()
#endif
{
	printf("include_mask = %llx\n",include_mask);
	if(bif_flag)
	{
		bif_fd=create_xls(bif_filename);
		do_label(bif_fd,command_line,bif_row++,bif_column);
	}
	if(!silent) printf("Excel output is below:\n");

    if ((!include_tflag) || (include_mask & WRITER_MASK)) {
	if(bif_flag)
		do_label(bif_fd,"Writer Report",bif_row++,bif_column);
	if(!silent) printf("\n%cWriter report%c\n",042,042);
	dump_report(2); 
	if(bif_flag)
		do_label(bif_fd,"Re-writer Report",bif_row++,bif_column);
	if(!silent) printf("\n%cRe-writer report%c\n",042,042);
	dump_report(3); 
    }

    if ((!include_tflag) || (include_mask & READER_MASK)) {
	if(bif_flag)
		do_label(bif_fd,"Reader Report",bif_row++,bif_column);
	if(!silent) printf("\n%cReader report%c\n",042,042);
	dump_report(4); 
	if(bif_flag)
		do_label(bif_fd,"Re-reader Report",bif_row++,bif_column);
	if(!silent) printf("\n%cRe-Reader report%c\n",042,042);
	dump_report(5); 
    }

	if ((!include_tflag) || (include_mask & RANDOM_RW_MASK)) {
		if(bif_flag)
			do_label(bif_fd,"Random Read Report",bif_row++,bif_column);
		if(!silent) printf("\n%cRandom read report%c\n",042,042);
		dump_report(6); 
		if(bif_flag)
			do_label(bif_fd,"Random Write Report",bif_row++,bif_column);
		if(!silent) printf("\n%cRandom write report%c\n",042,042);
		dump_report(7); 
	}

	if ((!include_tflag) || (include_mask & REVERSE_MASK)) {
		if(bif_flag)
			do_label(bif_fd,"Backward Read Report",bif_row++,bif_column);
		if(!silent) printf("\n%cBackward read report%c\n",042,042);
		dump_report(8); 
	}

	if ((!include_tflag) || (include_mask & REWRITE_REC_MASK)) {
		if(bif_flag)
			do_label(bif_fd,"Record Rewrite Report",bif_row++,bif_column);
		if(!silent) printf("\n%cRecord rewrite report%c\n",042,042);
		dump_report(9); 
	}

	if ((!include_tflag) || (include_mask & STRIDE_READ_MASK)) {
		if(bif_flag)
			do_label(bif_fd,"Stride Read Report",bif_row++,bif_column);
		if(!silent) printf("\n%cStride read report%c\n",042,042);
		dump_report(10); 
	}

	if ((!include_tflag) || (include_mask & FWRITER_MASK)) {
		if(bif_flag)
			do_label(bif_fd,"Fwrite Report",bif_row++,bif_column);
		if(!silent) printf("\n%cFwrite report%c\n",042,042);
		dump_report(11); 
		if(bif_flag)
			do_label(bif_fd,"Re-fwrite Report",bif_row++,bif_column);
		if(!silent) printf("\n%cRe-Fwrite report%c\n",042,042);
		dump_report(12); 
	}

	if ((!include_tflag) || (include_mask & FREADER_MASK)) {
		if(bif_flag)
			do_label(bif_fd,"Fread Report",bif_row++,bif_column);
		if(!silent) printf("\n%cFread report%c\n",042,042);
		dump_report(13); 
		if(bif_flag)
			do_label(bif_fd,"Re-fread Report",bif_row++,bif_column);
		if(!silent) printf("\n%cRe-Fread report%c\n",042,042);
		dump_report(14); 
	}

#ifdef HAVE_PREAD
	if(Eflag)
	{
		if ((!include_tflag) || (include_mask & PWRITER_MASK)) {
			if(bif_flag)
				do_label(bif_fd,"Pwrite Report",bif_row++,bif_column);
			if(!silent) printf("\n%cPwrite report%c\n",042,042);
			dump_report(15); 
			if(bif_flag)
				do_label(bif_fd,"Re-pwrite Report",bif_row++,bif_column);
		 	if(!silent) printf("\n%cRe-Pwrite report%c\n",042,042);
		 	dump_report(16); 
		}

		if ((!include_tflag) || (include_mask & PREADER_MASK)) {
			if(bif_flag)
				do_label(bif_fd,"Pread Report",bif_row++,bif_column);
		 	if(!silent) printf("\n%cPread report%c\n",042,042);
		 	dump_report(17); 
			if(bif_flag)
				do_label(bif_fd,"Re-pread Report",bif_row++,bif_column);
		 	if(!silent) printf("\n%cRe-Pread report%c\n",042,042);
		 	dump_report(18); 
		}

		if ((!include_tflag) || (include_mask & PWRITEV_MASK)) {
			if(bif_flag)
				do_label(bif_fd,"Pwritev Report",bif_row++,bif_column);
 			if(!silent) printf("\n%cPwritev report%c\n",042,042);
 			dump_report(19); 
			if(bif_flag)
				do_label(bif_fd,"Re-pwritev Report",bif_row++,bif_column);
 			if(!silent) printf("\n%cRe-Pwritev report%c\n",042,042);
 			dump_report(20); 
		}

		if ((!include_tflag) || (include_mask & PREADV_MASK)) {
			if(bif_flag)
				do_label(bif_fd,"Preadv Report",bif_row++,bif_column);
 			if(!silent) printf("\n%cPreadv report%c\n",042,042);
 			dump_report(21); 
			if(bif_flag)
				do_label(bif_fd,"Re-preadv Report",bif_row++,bif_column);
 			if(!silent) printf("\n%cRe-Preadv report%c\n",042,042);
 			dump_report(22); 
		}
	}
#endif
	if (cpuutilflag)
		dump_cputimes();
	if(bif_flag)
		close_xls(bif_fd);
}

/************************************************************************/
/* dump_times()		 						*/
/* Dumps the Excel CPU times report to stdout and to the bif file.	*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void dump_times(long long who)
#else
dump_times(who)
long long who;
#endif
{
	long long i;
	off64_t current_file_size;
	off64_t rec_size;

	if (bif_flag)
		bif_column++;
	if(!silent) printf("      ");

	for (rec_size = get_next_record_size(0); rec_size <= orig_max_rec_size;
		rec_size = get_next_record_size(rec_size))
	{
		if (rec_size == 0) break;
		if (bif_flag)
			do_float(bif_fd, (double)(rec_size/1024), bif_row, bif_column++);
#ifdef NO_PRINT_LLD
		if(!silent) printf("  %c%ld%c",042,rec_size/1024,042);
#else
		if(!silent) printf("  %c%lld%c",042,rec_size/1024,042);
#endif
	}
	if(!silent) printf("\n");
	if (bif_flag)
	{
		bif_column=0;
		bif_row++;
	}

	current_file_size = report_array[0][0];
	if (bif_flag)
	{
		do_float(bif_fd, (double)(current_file_size), bif_row, bif_column++);
	}
#ifdef NO_PRINT_LLD
	if(!silent) printf("%c%ld%c  ",042,current_file_size,042);
#else
	if(!silent) printf("%c%lld%c  ",042,current_file_size,042);
#endif
	for (i = 0; i <= max_y; i++) {
		if (report_array[0][i] != current_file_size) {
			if(!silent) printf("\n");
			current_file_size = report_array[0][i];
			if (bif_flag)
			{
				bif_row++;
				bif_column=0;
				do_float(bif_fd, (double)(current_file_size), bif_row, bif_column++);
			}
#ifdef NO_PRINT_LLD
			if(!silent) printf("%c%ld%c  ",042,current_file_size,042);
#else
			if(!silent) printf("%c%lld%c  ",042,current_file_size,042);
#endif
		}
		if (bif_flag)
			do_float(bif_fd, (double)(runtimes [who][i].cpuutil), bif_row, bif_column++);
		if(!silent) printf(" %6.2f", runtimes [who][i].cpuutil);
	}
	if(!silent) printf("\n");
	if (bif_flag)
	{
		bif_row++;
		bif_column=0;
	}
}

/************************************************************************/
/* Wrapper that dumps each of the collected data sets.			*/
/* This one dumps only the collected CPU times.				*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void dump_cputimes(void)
#else
void dump_cputimes()
#endif
{
	bif_row++;
	bif_column = 0;

    if ((!include_tflag) || (include_mask & WRITER_MASK)) {
	if(bif_flag)
		do_label(bif_fd, "Writer CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
	if(!silent) printf("\n%cWriter CPU utilization report (Zero values should be ignored)%c\n",042,042);
	dump_times(2); 
	if(bif_flag)
		do_label(bif_fd, "Re-writer CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
	if(!silent) printf("\n%cRe-writer CPU utilization report (Zero values should be ignored)%c\n",042,042);
	dump_times(3); 
    }

    if ((!include_tflag) || (include_mask & READER_MASK)) {
	if(bif_flag)
		do_label(bif_fd, "Reader CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
	if(!silent) printf("\n%cReader CPU utilization report (Zero values should be ignored)%c\n",042,042);
	dump_times(4); 
	if(bif_flag)
		do_label(bif_fd, "Re-reader CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
	if(!silent) printf("\n%cRe-Reader CPU utilization report (Zero values should be ignored)%c\n",042,042);
	dump_times(5); 
    }

	if ((!include_tflag) || (include_mask & RANDOM_RW_MASK)) {
		if(bif_flag)
			do_label(bif_fd, "Random Read CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
		if(!silent) printf("\n%cRandom read CPU utilization report (Zero values should be ignored)%c\n",042,042);
		dump_times(6); 
		if(bif_flag)
			do_label(bif_fd, "Random Write CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
		if(!silent) printf("\n%cRandom write CPU utilization report (Zero values should be ignored)%c\n",042,042);
		dump_times(7); 
	}

	if ((!include_tflag) || (include_mask & REVERSE_MASK)) {
		if(bif_flag)
			do_label(bif_fd, "Backward Read CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
		if(!silent) printf("\n%cBackward read CPU utilization report (Zero values should be ignored)%c\n",042,042);
		dump_times(8); 
	}

	if ((!include_tflag) || (include_mask & REWRITE_REC_MASK)) {
		if(bif_flag)
			do_label(bif_fd, "Record Rewrite CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
		if(!silent) printf("\n%cRecord rewrite CPU utilization report (Zero values should be ignored)%c\n",042,042);
		dump_times(9); 
	}

	if ((!include_tflag) || (include_mask & STRIDE_READ_MASK)) {
		if(bif_flag)
			do_label(bif_fd, "Stride Read CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
		if(!silent) printf("\n%cStride read CPU utilization report (Zero values should be ignored)%c\n",042,042);
		dump_times(10); 
	}

	if ((!include_tflag) || (include_mask & FWRITER_MASK)) {
		if(bif_flag)
			do_label(bif_fd, "Fwrite CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
		if(!silent) printf("\n%cFwrite CPU utilization report (Zero values should be ignored)%c\n",042,042);
		dump_times(11); 
		if(bif_flag)
			do_label(bif_fd, "Re-fwrite CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
		if(!silent) printf("\n%cRe-Fwrite CPU utilization report (Zero values should be ignored)%c\n",042,042);
		dump_times(12); 
	}

	if ((!include_tflag) || (include_mask & FREADER_MASK)) {
		if(bif_flag)
			do_label(bif_fd, "Fread CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
		if(!silent) printf("\n%cFread CPU utilization report (Zero values should be ignored)%c\n",042,042);
		dump_times(13); 
		if(bif_flag)
			do_label(bif_fd, "Re-fread CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
		if(!silent) printf("\n%cRe-Fread CPU utilization report (Zero values should be ignored)%c\n",042,042);
		dump_times(14); 
	}

#ifdef HAVE_PREAD
	if(Eflag)
	{
		if ((!include_tflag) || (include_mask & PWRITER_MASK)) {
			if(bif_flag)
				do_label(bif_fd, "Pwrite CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
			if(!silent) printf("\n%cPwrite CPU utilization report (Zero values should be ignored)%c\n",042,042);
			dump_times(15); 
			if(bif_flag)
				do_label(bif_fd, "Re-pwrite CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
		 	if(!silent) printf("\n%cRe-Pwrite CPU utilization report (Zero values should be ignored)%c\n",042,042);
		 	dump_times(16); 
		}

		if ((!include_tflag) || (include_mask & PREADER_MASK)) {
			if(bif_flag)
				do_label(bif_fd, "Pread CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
		 	if(!silent) printf("\n%cPread CPU utilization report (Zero values should be ignored)%c\n",042,042);
		 	dump_times(17); 
			if(bif_flag)
				do_label(bif_fd, "Re-pread CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
		 	if(!silent) printf("\n%cRe-Pread CPU utilization report (Zero values should be ignored)%c\n",042,042);
		 	dump_times(18); 
		}

		if ((!include_tflag) || (include_mask & PWRITEV_MASK)) {
			if(bif_flag)
				do_label(bif_fd, "Pwritev CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
 			if(!silent) printf("\n%cPwritev CPU utilization report (Zero values should be ignored)%c\n",042,042);
 			dump_times(19); 
			if(bif_flag)
				do_label(bif_fd, "Re-pwritev CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
 			if(!silent) printf("\n%cRe-Pwritev CPU utilization report (Zero values should be ignored)%c\n",042,042);
 			dump_times(20); 
		}

		if ((!include_tflag) || (include_mask & PREADV_MASK)) {
			if(bif_flag)
				do_label(bif_fd, "Preadv CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
			if(!silent) printf("\n%cPreadv CPU utilization report (Zero values should be ignored)%c\n",042,042);
 			dump_times(21); 
			if(bif_flag)
				do_label(bif_fd, "Re-preadv CPU utilization report (Zero values should be ignored)", bif_row++, bif_column);
			if(!silent) printf("\n%cRe-Preadv CPU utilization report (Zero values should be ignored)%c\n",042,042);
			dump_times(22); 
		}
	}
#endif
}

/************************************************************************/
/* Internal memory allocation mechanism. Uses shared memory or mmap 	*/
/************************************************************************/
char *
alloc_mem(size)
long long size;
{
	long long size1;
	char *addr;
	int shmid;
	int tfd;
	char *dumb;
	

	size1=l_max(size,page_size);
	if(!distributed)
	{
		if(!trflag)
		{
			addr=(char *)malloc((size_t)size1);
			return(addr);
		}
	}
#ifdef SHARED_MEM
	size1=l_max(size,page_size);
	size1=(size1 +page_size) & ~(page_size-1);
	shmid=(int)shmget((key_t)(IPC_PRIVATE), (size_t)size1 , (int)(IPC_CREAT|0666));
        if(shmid < (int)0)
        {
                printf("\nUnable to get shared memory segment(shmget)\n");
#ifdef NO_PRINT_LLD
                printf("shmid = %d, size = %ld, size1 = %d, Error %d\n",shmid,size,(size_t)size1,errno);
#else
                printf("shmid = %d, size = %lld, size1 = %d, Error %d\n",shmid,size,(size_t)size1,errno);
#endif
                exit(119);
        }
        /*addr = (char *)shmat(shmid, 0, SHM_W);*/
	/* Some systems will not take the above but
	 * will default to read/write if no flags
	 * are provided. (AIX)
	 * The POSIX standard states that if SHM_RDONLY
	 * is not specified then it will be read/write.
	 */
        addr = (char *)shmat((int)shmid, 0, 0);
#ifdef _64BIT_ARCH_
        if((long long)addr == (long long)-1)
#else
        if((long)addr == (long)-1)
#endif
        {
                printf("\nUnable to get shared memory segment\n");
                printf("..Error %d\n",errno);
                exit(120);
        }
	shmctl(shmid, IPC_RMID, 0);
	return(addr);
#else

	size1=l_max(size,page_size);
	size1=(size1 +page_size) & ~(page_size-1);
#ifdef bsd4_2
	if((tfd = creat("mmap.tmp", 0666))<0)
	{
		printf("Unable to create tmp file\n");
		exit(121);
	}
	addr=(char *)mmap(0,&size1,PROT_WRITE|PROT_READ,
		MAP_ANON|MAP_SHARED, tfd, 0);
	unlink("mmap.tmp");
#else
#if defined(solaris) 
	if((tfd = creat("mmap.tmp", 0666))<0)
	{
		printf("Unable to create tmp file\n");
		exit(121);
	}
	tfd=open("mmap.tmp",O_RDWR);
	dumb=(char *)malloc((size_t)size1);
	bzero(dumb,size1);
	write(tfd,dumb,size1);
	free(dumb);
	addr=(char *)mmap(0,(size_t)size1,PROT_WRITE|PROT_READ,
		MAP_SHARED, tfd, 0);
	unlink("mmap.tmp");
#else
#if defined(SCO) || defined(SCO_Unixware_gcc)
        if((tfd = creat("mmap.tmp", 0666))<0)
        {
                printf("Unable to create tmp file\n");
                exit(121);
        }
        tfd=open("mmap.tmp",O_RDWR);
        dumb=(char *)malloc((size_t)size1);
	bzero(dumb,size1);
        write(tfd,dumb,size1);
        free(dumb);
        addr=(char *)mmap(0,(size_t)size1,PROT_WRITE|PROT_READ,
                MAP_SHARED, tfd, 0);
        unlink("mmap.tmp");
#else
	addr=(char *)mmap(0,(size_t)size1,PROT_WRITE|PROT_READ,
		MAP_ANONYMOUS|MAP_SHARED, -1, 0);
#endif
#endif
#endif
	if((char *)addr == (char *)-1)
	{
		printf("\nUnable to get memory segment\n");
		printf("Error %d\n",errno);
		exit(122);
	}
	if(debug1)
		printf("Got shared memory for size %d\n",size1);

	return(addr);
#endif
}

/************************************************************************/
/* Implementation of poll() function.					*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void Poll(long long time1)
#else
void Poll(time1)
long  long time1;
#endif
{
	struct timeval howlong;
	howlong.tv_sec=(int)(time1/100000);
	howlong.tv_usec=(int)(time1%100000); /* Get into u.s. */
	select(0, 0, 0, 0, &howlong);
}

/************************************************************************/
/* Implementation of max() function.					*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
long long l_max(long long one,long long two)
#else
long long l_max(one,two)
long long one,two;
#endif
{
	if(one > two)
		return(one);
	else
		return(two);
}

/************************************************************************/
/* Internal Kill. With stonewalling disabled, kill does nothing 	*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void Kill(long long pid,long long sig)
#else
void Kill(pid,sig)
long long pid,sig;
#endif
{
	if(!xflag)
	{
		/*printf("Killing %d\n",pid);*/
		kill((pid_t)pid,(int)sig);
	}
}
/************************************************************************/
/* Implementation of min() function.					*/
/************************************************************************/

#ifdef HAVE_ANSIC_C
long long l_min(long long num1,long long num2)
#else
long long l_min(num1,num2)
long long num1,num2;
#endif
{
	if(num1 >= num2)
		return num2;
	else
		return num1;
}

/************************************************************************/
/* Routine to call throughput tests many times.				*/
/************************************************************************/

#ifdef HAVE_ANSIC_C
void
multi_throughput_test(long long mint,long long maxt)
#else
void multi_throughput_test(mint, maxt)
long long mint, maxt;
#endif
{
	long long i,xx;
	for(i=mint;i<=maxt;i++){
		num_child =i;
		throughput_test();
		current_x=0;
		current_y++;
	}
	if(Rflag)
		dump_throughput();

}



/************************************************************************/
/* Routine to purge the buffer cache by unmounting drive.		*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void
purge_buffer_cache()
#else
purge_buffer_cache()
#endif
{
	char command[1024];
	strcpy(command,"umount ");
	strcat(command, mountname);
	system(command);
	strcpy(command,"mount ");
	strcat(command, mountname);
	system(command);
}

/************************************************************************/
/* Thread write test				        		*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void *
thread_write_test(void *x)
#else
void *
thread_write_test( x)
#endif
{

	struct child_stats *child_stat;
	double starttime1 = 0;
	double temp_time;
	double walltime, cputime;
	double compute_val = (double)0;
	double delay = (double)0;
	double thread_qtime_stop,thread_qtime_start;
	off64_t traj_offset;
	long long tt,flags,traj_size;
	long long w_traj_bytes_completed;
	long long w_traj_ops_completed;
	FILE *w_traj_fd;
	int fd;
	long long recs_per_buffer;
	long long stopped,i;
	off64_t written_so_far, read_so_far, re_written_so_far,re_read_so_far;
	long long xx;
	char *dummyfile [MAXSTREAMS];           /* name of dummy file     */
	char *nbuff;
	char *maddr;
	char *wmaddr,*free_addr;
	int anwser,bind_cpu,wval;
	off64_t filebytes64;
	char tmpname[256];
	FILE *thread_wqfd;

#ifdef ASYNC_IO
	struct cache *gc=0;

#else
	long long *gc=0;
#endif

	if(w_traj_flag)
	{
		filebytes64 = w_traj_fsize;
		numrecs64=w_traj_ops;
	}
	else
	{
		filebytes64 = numrecs64*reclen;
	}
	written_so_far=read_so_far=re_written_so_far=re_read_so_far=0;
	w_traj_bytes_completed=w_traj_ops_completed=0;
	recs_per_buffer = cache_size/reclen ;
#ifdef NO_THREADS
	xx=chid;
#else
	if(use_thread)
	{
		xx = (long long)((long)x);
	}
	else
	{
		xx=chid;
	}
#endif
#ifndef NO_THREADS
#ifdef _HPUX_SOURCE
	if(ioz_processor_bind)
	{
		 bind_cpu=(begin_proc+(int)xx)%num_processors;
		 pthread_processor_bind_np(PTHREAD_BIND_FORCED_NP,
                         (pthread_spu_t *)&anwser, (pthread_spu_t)bind_cpu, pthread_self());
		my_nap(40);	/* Switch to new cpu */
	}
#endif
#endif
	if(use_thread)
		nbuff=barray[xx];
	else
		nbuff=buffer;
	if(debug1 )
	{
	   if(use_thread)
#ifdef NO_PRINT_LLD
		printf("\nStarting child %ld\n",xx);
#else
		printf("\nStarting child %lld\n",xx);
#endif
	   else
#ifdef NO_PRINT_LLD
		printf("\nStarting process %d slot %ld\n",getpid(),xx);
#else
		printf("\nStarting process %d slot %lld\n",getpid(),xx);
#endif
		
	}
	dummyfile[xx]=(char *)malloc((size_t)MAXNAMESIZE);
#ifdef NO_PRINT_LLD
	sprintf(dummyfile[xx],"%s.DUMMY.%ld",filearray[xx],xx);
#else
	sprintf(dummyfile[xx],"%s.DUMMY.%lld",filearray[xx],xx);
#endif
	/*****************/
	/* Children only */
	/*******************************************************************/
	/* Initial write throughput performance test. **********************/
	/*******************************************************************/
#ifdef _LARGEFILE64_SOURCE
	if((fd = creat64(dummyfile[xx], 0640))<0)
	{
		perror(dummyfile[xx]);
		exit(123);
	}
#else
	if((fd = creat(dummyfile[xx], 0640))<0)
	{
		perror(dummyfile[xx]);
		exit(124);
	}
#endif
	close(fd);
	if(oflag)
		flags=O_RDWR|O_SYNC;
	else
		flags=O_RDWR;
#if defined(linux)
	if(direct_flag)
		flags |=O_DIRECT;
#endif
#ifdef _LARGEFILE64_SOURCE
	if((fd = open64(dummyfile[xx], (int)flags))<0)
	{
		printf("\nCan not open temp file: %s\n", 
			filename);
		perror("open");
		exit(125);
	}
#else
	if((fd = open(dummyfile[xx], (int)flags))<0)
	{
		printf("\nCan not open temp file: %s\n", 
			filename);
		perror("open");
		exit(126);
	}
#endif
#ifdef VXFS
	if(direct_flag)
		ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
	if(async_flag)
		async_init(&gc,fd,direct_flag);
#else
	if(async_flag)
		async_init(&gc,fd,0);
#endif
#endif
	if(mmapflag)
	{
		maddr=(char *)initfile(fd,(filebytes64),1,PROT_READ|PROT_WRITE);
	}
	if(reclen < cache_size )
	{
		recs_per_buffer = cache_size/reclen ;
		nbuff=&nbuff[(xx%recs_per_buffer)*reclen];
	}
	if(fetchon)			/* Prefetch into processor cache */
		fetchit(nbuff,reclen);
	if(verify)
		fill_buffer(nbuff,reclen,(long long)pattern,sverify);

	if(w_traj_flag)
		w_traj_fd=open_w_traj();

	child_stat = (struct child_stats *)&shmaddr[xx];	
	child_stat->throughput = 0;
	child_stat->actual = 0;
	child_stat->flag=CHILD_STATE_READY; /* Tell parent child is ready to go */
	if(distributed && client_iozone)
		tell_master_ready(chid);
	if(distributed && client_iozone)
	{
		if(cdebug)
		{
			fprintf(newstdout,"Child %d waiting for go from master\n",(int)xx);
			fflush(newstdout);
		}
		wait_for_master_go(chid);
		if(cdebug)
		{
			fprintf(newstdout,"Child %d received go from master\n",(int)xx);
			fflush(newstdout);
		}
	}
	else
	{
		while(child_stat->flag!=CHILD_STATE_BEGIN)   /* Wait for signal from parent */
			Poll((long long)1);
	}

	written_so_far=0;
	child_stat = (struct child_stats *)&shmaddr[xx];
	child_stat->actual = 0;
	child_stat->throughput = 0;
	stopped=0;
	if(file_lock)
		if(mylockf((int) fd, (int) 1, (int)0) != 0)
			printf("File lock for write failed. %d\n",errno);
	if(Q_flag)
	{
		sprintf(tmpname,"Child_%d_wol.dat",(int)xx);
		thread_wqfd=fopen(tmpname,"a");
		if(thread_wqfd==0)
		{
			printf("Unable to open %s\n",tmpname);
			exit(40);
		}
		fprintf(thread_wqfd,"Offset in Kbytes   Latency in microseconds\n");
	}
	starttime1 = time_so_far();
	child_stat->start_time = starttime1;
	if(cpuutilflag)
	{
		walltime = starttime1;
		cputime = cputime_so_far();
	}
	if(w_traj_flag)
		rewind(w_traj_fd);
	for(i=0; i<numrecs64; i++){
		if(w_traj_flag)
		{
			traj_offset=get_traj(w_traj_fd, (long long *)&traj_size,(double *)&delay, (long)1);
			reclen=traj_size;
#ifdef _LARGEFILE64_SOURCE
			lseek64(fd,(off64_t)traj_offset,SEEK_SET);
#else
			lseek(fd,(off_t)traj_offset,SEEK_SET);
#endif
		}
		if(Q_flag)
		{
#ifdef _LARGEFILE64_SOURCE
			traj_offset=lseek64(fd,(off64_t)0,SEEK_CUR);
#else
			traj_offset=lseek(fd,(off_t)0,SEEK_CUR);
#endif
		}
		if(compute_flag)
			compute_val+=do_compute(delay);
		if(*stop_flag && !stopped){
			if(include_flush)
			{
				if(mmapflag)
					msync(maddr,(size_t)filebytes64,MS_SYNC);
				else
					fsync(fd);
			}
			child_stat->throughput = 
				(time_so_far() - starttime1)-time_res;
			if(child_stat->throughput < (double).000001) 
			{
				child_stat->throughput = time_res;
				if(rec_prob < reclen)
					rec_prob = reclen;
				res_prob=1;
			}

			if(OPS_flag){
			   /*written_so_far=(written_so_far*1024)/reclen;*/
			   written_so_far=w_traj_ops_completed;
			}
			child_stat->throughput = 
			  (double)written_so_far/child_stat->throughput;
			child_stat->actual = (double)written_so_far;
			if(debug1)
			{
				printf("\n(%ld) Stopped by another\n", (long)xx);
			}
			stopped=1;
		}
		if(purge)
			purgeit(nbuff,reclen);
		if(Q_flag)
		{
			thread_qtime_start=time_so_far();
		}
again:		
		if(mmapflag)
		{
			wmaddr = &maddr[i*reclen];
			fill_area((long long*)nbuff,(long long*)wmaddr,(long long)reclen);
			if(!mmapnsflag)
			{
			  if(mmapasflag)
			    msync(wmaddr,(size_t)reclen,MS_ASYNC);
			  if(mmapssflag)
			    msync(wmaddr,(size_t)reclen,MS_SYNC);
			}
		}
		else
		{
		   if(async_flag)
		   {
			     if(no_copy_flag)
			     {
				free_addr=nbuff=(char *)malloc((size_t)reclen+page_size);
				nbuff=(char *)(((long)nbuff+(long)page_size) & (long)(~page_size-1));
				if(verify)
					fill_buffer(nbuff,reclen,(long long)pattern,sverify);
			        async_write_no_copy(gc, (long long)fd, nbuff, reclen, (i*reclen), depth,free_addr);
			     }
			     else
				async_write(gc, (long long)fd, nbuff, reclen, (i*reclen), depth);
		   }
		   else
		   {
		      wval=write(fd, nbuff, (size_t) reclen);
		      if(wval != reclen)
		      {
			if(*stop_flag && !stopped){
				if(include_flush)
				{
					if(mmapflag)
						msync(maddr,(size_t)filebytes64,MS_SYNC);
					else
						fsync(fd);
				}
				temp_time = time_so_far();
				child_stat->stop_time = temp_time;
				child_stat->throughput = 
					(temp_time - starttime1)-time_res;
				if(child_stat->throughput < (double).000001) 
				{
					child_stat->throughput= time_res;
					if(rec_prob < reclen)
						rec_prob = reclen;
					res_prob=1;
				}

				if(OPS_flag){
				   /*written_so_far=(written_so_far*1024)/reclen;*/
				   written_so_far=w_traj_ops_completed;
				}
				child_stat->throughput = 
				  (double)written_so_far/child_stat->throughput;
				child_stat->actual = (double)written_so_far;
				if(debug1)
				{
					printf("\n(%ld) Stopped by another\n", (long)xx);
				}
				stopped=1;
				goto again;
			}
			/* Note: Writer must finish even though told
			   to stop. Otherwise the readers will fail.
			   The code will capture bytes transfered
			   before told to stop but let the writer
			   complete.
			*/
#ifdef NO_PRINT_LLD
		    	printf("\nError writing block %ld, fd= %d\n", i,
				 fd);
#else
		    	printf("\nError writing block %lld, fd= %d\n", i,
				 fd);
#endif
			if(wval==-1)
				perror("write");
			if (!no_unlink)
				unlink(dummyfile[xx]);
			child_stat->flag = CHILD_STATE_HOLD;
		    	exit(127);
		      }
		    }
		}
		if(Q_flag)
		{
			thread_qtime_stop=time_so_far();
#ifdef NO_PRINT_LLD
			fprintf(thread_wqfd,"%10.1ld %10.0f\n",(traj_offset)/1024,((thread_qtime_stop-thread_qtime_start-time_res))*1000000);
#else
			fprintf(thread_wqfd,"%10.1lld %10.0f\n",(traj_offset)/1024,((thread_qtime_stop-thread_qtime_start-time_res))*1000000);
#endif
		}
		w_traj_ops_completed++;
		w_traj_bytes_completed+=reclen;
		written_so_far+=reclen/1024;
		if(*stop_flag)
		{
			written_so_far-=reclen/1024;
			w_traj_bytes_completed-=reclen;
		}
	}
	

	if(file_lock)
		if(mylockf((int) fd, (int) 0, (int)0))
			printf("Write unlock failed. %d\n",errno);
	
#ifdef ASYNC_IO
	if(async_flag)
	{
		end_async(gc);
		gc=0;
	}
#endif
	if(!xflag)
	{
		*stop_flag=1;
		if(distributed && client_iozone)
			send_stop();
	}
	
	if(include_flush)
	{
		if(mmapflag)
			msync(maddr,(size_t)filebytes64,MS_SYNC);
		else
			fsync(fd);
	}
	if(include_close)
	{
		if(mmapflag)
			mmap_end(maddr,(unsigned long long)filebytes64);
		close(fd);
	}
	if(!stopped){
		temp_time = time_so_far();
		child_stat->throughput = ((temp_time - starttime1)-time_res)
			-compute_val;
		if(child_stat->throughput < (double).000001) 
		{
			child_stat->throughput= time_res;
			if(rec_prob < reclen)
				rec_prob = reclen;
			res_prob=1;
		}

		if(OPS_flag){
		   /*written_so_far=(written_so_far*1024)/reclen;*/
		   written_so_far=w_traj_ops_completed;
		}
		child_stat->throughput =
			(double)written_so_far/child_stat->throughput;
		child_stat->actual = (double)written_so_far;
		child_stat->stop_time = temp_time;
	}
	if(cdebug)
	{
		fprintf(newstdout,"Child: throughput %f actual %f \n",child_stat->throughput,
			child_stat->actual);
		fflush(newstdout);
	}
	if(distributed && client_iozone)
		tell_master_stats(THREAD_WRITE_TEST, chid, child_stat->throughput, 
			child_stat->actual, child_stat->stop_time,
			child_stat->start_time,child_stat->fini_time,(char)*stop_flag,
			(long long)CHILD_STATE_HOLD);
			
	if(cpuutilflag)
	{
		child_stat->fini_time = time_so_far();
		cputime = cputime_so_far() - cputime;
		if (cputime < cputime_res)
			cputime = 0.0;
		child_stat->cputime = cputime;
		walltime = time_so_far() - walltime;
		if (walltime < cputime_res)
			walltime = 0.0;
		child_stat->walltime = walltime;
	}
	if (debug1) {
		printf(" child/slot: %lld, start-fini: %8.3f-%8.3f=%8.3f %8.3fC" " -> %6.2f%%\n",
			xx, child_stat->start_time, child_stat->fini_time, walltime, cputime,
			cpu_util(cputime, walltime));
	}
	child_stat->flag = CHILD_STATE_HOLD; /* Tell parent I'm done */
	stopped=0;
	/*******************************************************************/
	/* End write performance test. *************************************/
	/*******************************************************************/
	if(debug1)
#ifdef NO_PRINT_LLD
		printf("\nChild finished %ld\n",xx);
#else
		printf("\nChild finished %lld\n",xx);
#endif
	if(!include_close)
	{
		if(mmapflag)
		{
			msync(maddr,(size_t)numrecs64*reclen,MS_SYNC); /*Clean up before read starts running*/ 
			mmap_end(maddr,(unsigned long long)numrecs64*reclen);
		}else
			fsync(fd);
			
		close(fd);
	}
	if(Q_flag && (thread_wqfd !=0) )
		fclose(thread_wqfd);
	free(dummyfile[xx]);
	if(w_traj_flag)
		fclose(w_traj_fd);
#ifdef NO_THREADS
	exit(0);
#else
	if(use_thread)
		thread_exit();
	else
		exit(0);
#endif
return(0);
}

/************************************************************************/
/* Thread re-write test				        		*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void *
thread_rwrite_test(void *x)
#else
void *
thread_rwrite_test(x)
#endif
{
	/************************/
	/* Children only here 	*/
	/************************/
	struct child_stats *child_stat;
	long long xx;
	long long tt;
	double compute_val = (double)0;
	double walltime, cputime;
	double delay = (double)0;
	double thread_qtime_stop,thread_qtime_start;
	off64_t traj_offset;
	long long w_traj_bytes_completed;
	long long w_traj_ops_completed;
	int fd;
	FILE *w_traj_fd;
	long long flags = 0;
	double starttime1 = 0;
	double temp_time;
	long long recs_per_buffer,traj_size;
	long long i;
	off64_t written_so_far, read_so_far, re_written_so_far,re_read_so_far=0;
	char *dummyfile [MAXSTREAMS];           /* name of dummy file     */
	char *nbuff;
	char *maddr,*free_addr;
	char *wmaddr;
	int anwser,bind_cpu,wval;
	FILE *thread_rwqfd;
	char tmpname[256];
#ifdef ASYNC_IO
	struct cache *gc=0;

#else
	long long *gc=0;
#endif

	w_traj_bytes_completed=w_traj_ops_completed=0;
	written_so_far=read_so_far=re_written_so_far=re_read_so_far=0;
	recs_per_buffer = cache_size/reclen ;
	if(w_traj_flag)
	{
		filebytes64 = w_traj_fsize;
		numrecs64=w_traj_ops;
	}
	else
	{
		filebytes64 = numrecs64*reclen;
	}
#ifdef NO_THREADS
	xx=chid;
#else
	if(use_thread)
		xx=(long long)((long)x);
	else
	{
		xx=chid;
	}
#endif
#ifndef NO_THREADS
#ifdef _HPUX_SOURCE
	if(ioz_processor_bind)
	{
		 bind_cpu=(begin_proc+(int)xx)%num_processors;
		 pthread_processor_bind_np(PTHREAD_BIND_FORCED_NP,
                         (pthread_spu_t *)&anwser, (pthread_spu_t)bind_cpu, pthread_self());
		my_nap(40);	/* Switch to new cpu */
	}
#endif
#endif
	if(use_thread)
		nbuff=barray[xx];
	else
		nbuff=buffer;
	child_stat = (struct child_stats *)&shmaddr[xx];
	child_stat->throughput = 0;
	child_stat->actual = 0;
	if(debug1)
	{
	   if(use_thread)
#ifdef NO_PRINT_LLD
		printf("\nStarting child %ld\n",xx);
#else
		printf("\nStarting child %lld\n",xx);
#endif
	   else
#ifdef NO_PRINT_LLD
		printf("\nStarting process %d slot %ld\n",getpid(),xx);
#else
		printf("\nStarting process %d slot %lld\n",getpid(),xx);
#endif
		
	}
	dummyfile[xx]=(char *)malloc((size_t)MAXNAMESIZE);
#ifdef NO_PRINT_LLD
	sprintf(dummyfile[xx],"%s.DUMMY.%ld",filearray[xx],xx);
#else
	sprintf(dummyfile[xx],"%s.DUMMY.%lld",filearray[xx],xx);
#endif
	flags = O_RDWR;
	if(oflag)
		flags|= O_SYNC;
#if defined(linux)
	if(direct_flag)
		flags |=O_DIRECT;
#endif
#ifdef _LARGEFILE64_SOURCE
	if((fd = open64(dummyfile[xx], (int)flags))<0)
	{
#ifdef NO_PRINT_LLD
		printf("\nChild %ld\n",xx);
#else
		printf("\nChild %lld\n",xx);
#endif
		child_stat->flag = CHILD_STATE_HOLD;
		perror(dummyfile[xx]);
		exit(128);
	}
#else
	if((fd = open(dummyfile[xx], (int)flags))<0)
	{
#ifdef NO_PRINT_LLD
		printf("\nChild %ld\n",xx);
#else
		printf("\nChild %lld\n",xx);
#endif
		child_stat->flag = CHILD_STATE_HOLD;
		perror(dummyfile[xx]);
		exit(129);
	}
#endif
#ifdef VXFS
	if(direct_flag)
		ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
	if(async_flag)
		async_init(&gc,fd,direct_flag);
#else
	if(async_flag)
		async_init(&gc,fd,0);
#endif
#endif
	if(mmapflag)
	{
		maddr=(char *)initfile(fd,(numrecs64*reclen),1,PROT_READ|PROT_WRITE);
	}
	if(fetchon)
		fetchit(nbuff,reclen);
	if(w_traj_flag)
		w_traj_fd=open_w_traj();
	if(Q_flag)
	{
		sprintf(tmpname,"Child_%d_rwol.dat",(int)xx);
		thread_rwqfd=fopen(tmpname,"a");
		if(thread_rwqfd==0)
		{
			printf("Unable to open %s\n",tmpname);
			exit(40);
		}
		fprintf(thread_rwqfd,"Offset in Kbytes   Latency in microseconds\n");
	}
	child_stat->flag = CHILD_STATE_READY;
	if(distributed && client_iozone)
		tell_master_ready(chid);
	if(distributed && client_iozone)
	{
		if(cdebug)
			fprintf(newstdout,"Child %d waiting for go from master\n",(int)xx);
		wait_for_master_go(chid);
		if(cdebug)
			fprintf(newstdout,"Child %d received go from master\n",(int)xx);
	}
	else
	{
		while(child_stat->flag!=CHILD_STATE_BEGIN)   /* Wait for signal from parent */
			Poll((long long)1);
	}
	starttime1 = time_so_far();
	child_stat->start_time = starttime1;
	if(cpuutilflag)
	{
		walltime = starttime1;
		cputime = cputime_so_far();
	}
	if(file_lock)
		if(mylockf((int) fd, (int) 1, (int)0) != 0)
			printf("File lock for write failed. %d\n",errno);
	if(cpuutilflag)
	{
		child_stat->start_time = starttime1;
		walltime = starttime1;
		cputime = cputime_so_far();
	}
	if(w_traj_flag)
		rewind(w_traj_fd);
	for(i=0; i<numrecs64; i++){
		if(w_traj_flag)
		{
			traj_offset=get_traj(w_traj_fd, (long long *)&traj_size,(double *)&delay,(long)1);
			reclen=traj_size;
#ifdef _LARGEFILE64_SOURCE
			lseek64(fd,(off64_t)traj_offset,SEEK_SET);
#else
			lseek(fd,(off_t)traj_offset,SEEK_SET);
#endif
		}
		if(Q_flag)
		{
#ifdef _LARGEFILE64_SOURCE
			traj_offset=lseek64(fd,(off64_t)0,SEEK_CUR);
#else
			traj_offset=lseek(fd,(off_t)0,SEEK_CUR);
#endif
		}
		if(compute_flag)
			compute_val+=do_compute(delay);
		if(*stop_flag)
		{
			if(debug1)
				printf("\nStop_flag 1\n");
			break;
		}
		if(async_flag && no_copy_flag)
		{
			free_addr=buffer=(char *)malloc((size_t)reclen+page_size);
			nbuff=(char *)(((long)nbuff+(long)page_size) & (long)(~page_size-1));
		}
		if(verify)
			fill_buffer(nbuff,reclen,(long long)pattern,sverify);
		if(purge)
			purgeit(nbuff,reclen);
		if(Q_flag)
		{
			thread_qtime_start=time_so_far();
		}
		if(mmapflag)
		{
			wmaddr = &maddr[i*reclen];
			fill_area((long long*)nbuff,(long long*)wmaddr,(long long)reclen);
			if(!mmapnsflag)
			{
			  if(mmapasflag)
			    msync(wmaddr,(size_t)reclen,MS_ASYNC);
			  if(mmapssflag)
			    msync(wmaddr,(size_t)reclen,MS_SYNC);
			}
		}
		else
		{
			if(async_flag)
			{
			     if(no_copy_flag)
			         async_write_no_copy(gc, (long long)fd, nbuff, reclen, (i*reclen), depth,free_addr);
			     else
			         async_write(gc, (long long)fd, nbuff, reclen, (i*reclen), depth);
			}
			else
			{
			   wval=write(fd, nbuff, (size_t) reclen);
			   if(wval != reclen)
			   {
				if(*stop_flag)
				{
					if(debug1)
						printf("\nStop_flag 2\n");
					break;
				}
#ifdef NO_PRINT_LLD
		    		printf("\nError writing block %ld, fd= %d\n", i,
					 fd);
#else
		    		printf("\nError writing block %lld, fd= %d\n", i,
					 fd);
#endif
				if(wval==-1)
					perror("write");
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = CHILD_STATE_HOLD;
				signal_handler();
			   }
			}
		}
		re_written_so_far+=reclen/1024;
	   	w_traj_ops_completed++;
		w_traj_bytes_completed+=reclen;
		if(*stop_flag)
		{
			re_written_so_far-=reclen/1024;
			w_traj_bytes_completed-=reclen;
		}
		if(Q_flag)
		{
			thread_qtime_stop=time_so_far();
#ifdef NO_PRINT_LLD
			fprintf(thread_rwqfd,"%10.1ld %10.0f\n",(traj_offset)/1024,((thread_qtime_stop-thread_qtime_start-time_res))*1000000);
#else
			fprintf(thread_rwqfd,"%10.1lld %10.0f\n",(traj_offset)/1024,((thread_qtime_stop-thread_qtime_start-time_res))*1000000);
#endif
		}
	}
	if(file_lock)
		if(mylockf((int) fd, (int) 0, (int)0))
			printf("Write unlock failed. %d\n",errno);
#ifdef ASYNC_IO
	if(async_flag)
	{
		end_async(gc);
		gc=0;
	}
#endif
	if(include_flush)
	{
		if(mmapflag)
		{
			msync(maddr,(size_t)(filebytes64),MS_SYNC);
		}else
			fsync(fd);
	}
	if(include_close)
	{
		if(mmapflag)
		{
			mmap_end(maddr,(unsigned long long)filebytes64);
		}
		close(fd);
	}
	temp_time=time_so_far();
	child_stat=(struct child_stats *)&shmaddr[xx];
	child_stat->throughput = ((temp_time - starttime1)-time_res)
		-compute_val;
	if(child_stat->throughput < (double).000001) 
	{
		child_stat->throughput= time_res;
		if(rec_prob < reclen)
			rec_prob = reclen;
		res_prob=1;
	}

	child_stat->stop_time = temp_time;
	if(OPS_flag){
	   /*re_written_so_far=(re_written_so_far*1024)/reclen;*/
	   re_written_so_far=w_traj_ops_completed;
	}
	child_stat->throughput = 
		(double)re_written_so_far/child_stat->throughput;
	child_stat->actual = (double)re_written_so_far;
	if(!xflag)
	{
		*stop_flag=1;
		if(distributed && client_iozone)
			send_stop();
	}
	if(cdebug)
		fprintf(newstdout,"Child: throughput %f actual %f \n",child_stat->throughput,
			child_stat->actual);
	if(distributed && client_iozone)
		tell_master_stats(THREAD_REWRITE_TEST, chid, child_stat->throughput, 
			child_stat->actual, child_stat->stop_time,
			child_stat->start_time,child_stat->fini_time,(char)*stop_flag,
			(long long)CHILD_STATE_HOLD);
	if(cpuutilflag)
	{
		child_stat->fini_time = time_so_far();
		cputime = cputime_so_far() - cputime;
		if (cputime < cputime_res)
			cputime = 0.0;
		child_stat->cputime = cputime;
		walltime = time_so_far() - walltime;
		if (walltime < cputime_res)
			walltime = 0.0;
		child_stat->walltime = walltime;
	}
	child_stat->flag = CHILD_STATE_HOLD;	/* Tell parent I'm done */
	if(!include_close)
	{
		if(mmapflag)
		{
			msync(maddr,(size_t)(filebytes64),MS_SYNC);
			mmap_end(maddr,(unsigned long long)filebytes64);
		}
		else
			fsync(fd);
		close(fd);
	}
	free(dummyfile[xx]);

	if(Q_flag && (thread_rwqfd !=0) )
		fclose(thread_rwqfd);

	if(w_traj_flag)
		fclose(w_traj_fd);
	if(debug1)
#ifdef NO_PRINT_LLD
		printf("\nChild Stopping  %ld\n",xx);
#else
		printf("\nChild Stopping  %lld\n",xx);
#endif
#ifdef NO_THREADS
	exit(0);
#else
	if(use_thread)
		thread_exit();
	else
		exit(0);
#endif
return(0);
}

/************************************************************************/
/* Thread read test				        		*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void *
thread_read_test(void *x)
#else
void *
thread_read_test(x)
#endif
{
	long long xx;
	struct child_stats *child_stat;
	long long tt;
	double walltime, cputime;
	long long r_traj_bytes_completed;
	long long r_traj_ops_completed;
	int fd;
	FILE *r_traj_fd,*thread_rqfd;
	long long flags = 0;
	off64_t traj_offset;
	double starttime1 = 0;
	double delay = 0;
	double temp_time;
	double thread_qtime_start,thread_qtime_stop;
	double compute_val = (double)0;
	off64_t written_so_far, read_so_far, re_written_so_far,re_read_so_far;
	long long recs_per_buffer,traj_size;
	off64_t i;
	char *dummyfile[MAXSTREAMS];           /* name of dummy file     */
	char *nbuff;
	char *maddr;
	char *wmaddr;
	char tmpname[256];
	volatile char *buffer1;
	int anwser,bind_cpu;
#ifdef ASYNC_IO
	struct cache *gc=0;
#else
	long long *gc=0;
#endif

	r_traj_bytes_completed=r_traj_ops_completed=0;
	written_so_far=read_so_far=re_written_so_far=re_read_so_far=0;
	recs_per_buffer = cache_size/reclen ;
	if(r_traj_flag)
	{
		filebytes64 = r_traj_fsize;
		numrecs64=r_traj_ops;
	}
	else
	{
		filebytes64 = numrecs64*reclen;
	}
	
#ifdef NO_THREADS
	xx=chid;
#else
	if(use_thread)
		xx = (long long)((long)x);
	else
	{
		xx=chid;
	}
#endif
#ifndef NO_THREADS
#ifdef _HPUX_SOURCE
	if(ioz_processor_bind)
	{
		 bind_cpu=(begin_proc+(int)xx)%num_processors;
		 pthread_processor_bind_np(PTHREAD_BIND_FORCED_NP,
                         (pthread_spu_t *)&anwser, (pthread_spu_t)bind_cpu, pthread_self());
		my_nap(40);	/* Switch to new cpu */
	}
#endif
#endif
	if(use_thread)
		nbuff=barray[xx];
	else
		nbuff=buffer;
	dummyfile[xx]=(char *)malloc((size_t)MAXNAMESIZE);
#ifdef NO_PRINT_LLD
	sprintf(dummyfile[xx],"%s.DUMMY.%ld",filearray[xx],xx);
#else
	sprintf(dummyfile[xx],"%s.DUMMY.%lld",filearray[xx],xx);
#endif
	if(oflag)
		flags=O_RDONLY|O_SYNC;
	else
		flags=O_RDONLY;
#if defined(linux)
	if(direct_flag)
		flags |=O_DIRECT;
#endif
#ifdef _LARGEFILE64_SOURCE
	if((fd = open64(dummyfile[xx], (int)flags))<0)
	{
		perror(dummyfile[xx]);
		exit(130);
	}
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
	if(async_flag)
		async_init(&gc,fd,direct_flag);
#else
	if(async_flag)
		async_init(&gc,fd,0);
#endif
#endif
#ifdef VXFS
	if(direct_flag)
		ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#else
	if((fd = open(dummyfile[xx], (int)flags))<0)
	{
		perror(dummyfile[xx]);
		exit(131);
	}
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
	if(async_flag)
		async_init(&gc,fd,direct_flag);
#else
	if(async_flag)
		async_init(&gc,fd,0);
#endif
#endif
#ifdef VXFS
	if(direct_flag)
		ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif

#endif
	if(mmapflag)
	{
		maddr=(char *)initfile(fd,(numrecs64*reclen),0,PROT_READ);
	}
	child_stat = (struct child_stats *)&shmaddr[xx];
	child_stat->throughput = 0;
	child_stat->actual = 0;
	if(debug1)
	{
	   if(use_thread)
#ifdef NO_PRINT_LLD
		printf("\nStarting child %ld\n",xx);
#else
		printf("\nStarting child %lld\n",xx);
#endif
	   else
#ifdef NO_PRINT_LLD
		printf("\nStarting process %d slot %ld\n",getpid(),xx);
#else
		printf("\nStarting process %d slot %lld\n",getpid(),xx);
#endif
		
	}
	/*****************/
	/* Children only */
	/*****************/
	if(Q_flag)
	{
		sprintf(tmpname,"Child_%d_rol.dat",(int)xx);
		thread_rqfd=fopen(tmpname,"a");
		if(thread_rqfd==0)
		{
			printf("Unable to open %s\n",tmpname);
			exit(40);
		}
		fprintf(thread_rqfd,"Offset in Kbytes   Latency in microseconds\n");
	}

	if(r_traj_flag)
		r_traj_fd=open_r_traj();
	if(fetchon)
		fetchit(nbuff,reclen);
	child_stat=(struct child_stats *)&shmaddr[xx];
	child_stat->flag = CHILD_STATE_READY;
	if(distributed && client_iozone)
        {
		tell_master_ready(chid);
                wait_for_master_go(chid);
        }
        else
        {
		/* Wait for signal from parent */
                while(child_stat->flag!=CHILD_STATE_BEGIN)   
                        Poll((long long)1);
        }
	if(file_lock)
		if(mylockf((int) fd, (int) 1, (int)1) != 0)
			printf("File lock for read failed. %d\n",errno);
	starttime1 = time_so_far();
	child_stat->start_time = starttime1;
	if(cpuutilflag)
	{
		walltime = starttime1;
		cputime = cputime_so_far();
	}

	if(r_traj_flag)
		rewind(r_traj_fd);
	for(i=0; i<numrecs64; i++){
		if(disrupt_flag && ((i%DISRUPT)==0))
		{
			disrupt(fd);
		}
		if(r_traj_flag)
		{
			traj_offset=get_traj(r_traj_fd, (long long *)&traj_size,(double *)&delay,(long)0);
			reclen=traj_size;
#ifdef _LARGEFILE64_SOURCE
			lseek64(fd,(off64_t)traj_offset,SEEK_SET);
#else
			lseek(fd,(off_t)traj_offset,SEEK_SET);
#endif
		}
		if(Q_flag)
		{
#ifdef _LARGEFILE64_SOURCE
			traj_offset=lseek64(fd,(off64_t)0,SEEK_CUR);
#else
			traj_offset=lseek(fd,(off_t)0,SEEK_CUR);
#endif
		}
		if(compute_flag)
			compute_val+=do_compute(delay);
		if(*stop_flag)
		{
			if(debug1)
				printf("\n(%ld) Stopped by another 2\n", (long)xx);
			break;
		}
		if(purge)
			purgeit(nbuff,reclen);
		if(Q_flag)
                {
                        thread_qtime_start=time_so_far();
                }
		if(mmapflag)
		{
			wmaddr = &maddr[i*reclen];
			fill_area((long long*)wmaddr,(long long*)nbuff,(long long)reclen);
		}
		else
		{
			  if(async_flag)
			  {
			    if(no_copy_flag)
			      async_read_no_copy(gc, (long long)fd, &buffer1, (i*reclen), reclen,
			    	1LL,(numrecs64*reclen),depth);
			    else
			      async_read(gc, (long long)fd, nbuff, (i*reclen), reclen,
			    	1LL,(numrecs64*reclen),depth);
			  }
			  else
			  {
			      if(read((int)fd, (void*)nbuff, (size_t) reclen) != reclen)
			      {
				if(*stop_flag)
				{
					if(debug1)
						printf("\n(%ld) Stopped by another 2\n", (long)xx);
					break;
				}
#ifdef NO_PRINT_LLD
		    		printf("\nError reading block %ld, fd= %d\n", i,
					 fd);
#else
		    		printf("\nError reading block %lld, fd= %d\n", i,
					 fd);
#endif
				perror("read");
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = CHILD_STATE_HOLD;
		    		exit(132);
			      }
			  }
		}
		if(verify){
		   if(async_flag && no_copy_flag)
		   {
			   if(verify_buffer(buffer1,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = CHILD_STATE_HOLD;
				exit(133);
			   }
		   }
		   else
		   {
			   if(verify_buffer(nbuff,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = CHILD_STATE_HOLD;
				exit(134);
			   }
		   }
		}
		if(async_flag && no_copy_flag)
			async_release(gc);
		read_so_far+=reclen/1024;
		r_traj_bytes_completed+=reclen;
		r_traj_ops_completed++;
		if(*stop_flag)
		{
			read_so_far-=reclen/1024;
			r_traj_bytes_completed-=reclen;
		}
                if(Q_flag)
                {
                        thread_qtime_stop=time_so_far();
#ifdef NO_PRINT_LLD
                        fprintf(thread_rqfd,"%10.1ld %10.0f\n",(traj_offset)/1024,((thread_qtime_stop-thread_qtime_start-time_res))*1000000);
#else
                        fprintf(thread_rqfd,"%10.1lld %10.0f\n",(traj_offset)/1024,((thread_qtime_stop-thread_qtime_start-time_res))*1000000);
#endif
                }

	}
	if(file_lock)
		if(mylockf((int) fd, (int) 0, (int)1))
			printf("Read unlock failed. %d\n",errno);
#ifdef ASYNC_IO
	if(async_flag)
	{
		end_async(gc);
		gc=0;
	}
#endif
	if(include_flush)
	{
		if(mmapflag)
		{
			msync(maddr,(size_t)(filebytes64),MS_SYNC);
		}else
			fsync(fd);
	}
	if(include_close)
	{	
		if(mmapflag)
		{
			mmap_end(maddr,(unsigned long long)filebytes64);
		}
		close(fd);
	}
	temp_time = time_so_far();
	child_stat->stop_time = temp_time;
	child_stat=(struct child_stats *)&shmaddr[xx];
	child_stat->throughput = ((temp_time - starttime1)-time_res)
		-compute_val;
	if(child_stat->throughput < (double).000001) 
	{
		child_stat->throughput= time_res;
		if(rec_prob < reclen)
			rec_prob = reclen;
		res_prob=1;
	}

	if(OPS_flag){
	   /*read_so_far=(read_so_far*1024)/reclen;*/
	   read_so_far=r_traj_ops_completed;
	}
	child_stat->throughput = read_so_far/child_stat->throughput;
	child_stat->actual = read_so_far;
	if(!xflag)
	{
		*stop_flag=1;
		if(distributed && client_iozone)
			send_stop();
	}
        if(cdebug)
                fprintf(newstdout,"Child: throughput %f actual %f \n",child_stat->throughput,
                        child_stat->actual);
        if(distributed && client_iozone)
                tell_master_stats(THREAD_READ_TEST, chid, child_stat->throughput,
                        child_stat->actual, child_stat->stop_time,
                        child_stat->start_time,child_stat->fini_time,(char)*stop_flag,
                        (long long)CHILD_STATE_HOLD);
	if(cpuutilflag)
	{
		child_stat->fini_time = time_so_far();
		cputime = cputime_so_far() - cputime;
		if (cputime < cputime_res)
			cputime = 0.0;
		child_stat->cputime = cputime;
		walltime = time_so_far() - walltime;
		if (walltime < cputime_res)
			walltime = 0.0;
		child_stat->walltime = walltime;
	}
	child_stat->flag = CHILD_STATE_HOLD; 	/* Tell parent I'm done */
	/*fsync(fd);*/
	if(!include_close)
	{
		if(mmapflag)
		{
			msync(maddr,(size_t)(filebytes64),MS_SYNC);
			mmap_end(maddr,(unsigned long long)filebytes64);
		}else
			fsync(fd);
		close(fd);
	}
        if(Q_flag && (thread_rqfd !=0) )
                fclose(thread_rqfd);
	free(dummyfile[xx]);
	if(r_traj_flag)
		fclose(r_traj_fd);
	if(debug1)
#ifdef NO_PRINT_LLD
		printf("\nChild finished %ld\n",xx);
#else
		printf("\nChild finished %lld\n",xx);
#endif
#ifdef NO_THREADS
	exit(0);
#else
	if(use_thread)
		thread_exit();
	else
		exit(0);
#endif
return(0);
}

/************************************************************************/
/* Thread re-read test				        		*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void *
thread_rread_test(void *x)
#else
void *
thread_rread_test(x)
#endif
{
	long long xx;
	char *nbuff;
	struct child_stats *child_stat;
	long long tt;
	int fd;
	FILE *r_traj_fd,*thread_rrqfd;
	long long r_traj_bytes_completed;
	double walltime, cputime;
	long long r_traj_ops_completed;
	off64_t traj_offset;
	long long flags = 0;
	double starttime1 = 0;
	double delay = 0;
	double temp_time;
	double thread_qtime_start,thread_qtime_stop;
	double compute_val = (double)0;
	long long recs_per_buffer,traj_size;
	off64_t i;
	off64_t written_so_far, read_so_far, re_written_so_far,
		re_read_so_far;
	char *dummyfile[MAXSTREAMS];           /* name of dummy file     */
	char *maddr;
	char *wmaddr;
	volatile char *buffer1;
	int anwser,bind_cpu;
	char tmpname[256];
#ifdef ASYNC_IO
	struct cache *gc=0;
#else
	long long *gc=0;
#endif
	/*****************/
	/* Children only */
	/*****************/
	r_traj_bytes_completed=r_traj_ops_completed=0;
	written_so_far=read_so_far=re_written_so_far=re_read_so_far=0;
	recs_per_buffer = cache_size/reclen ;
#ifdef NO_THREADS
	xx=chid;
#else
	if(r_traj_flag)
	{
		filebytes64 = r_traj_fsize;
		numrecs64=r_traj_ops;
	}
	else
	{
		filebytes64 = numrecs64*reclen;
	}
	if(use_thread)
		xx = (long long)((long)x);
	else
	{
		xx=chid;
	}
#endif
#ifndef NO_THREADS
#ifdef _HPUX_SOURCE
	if(ioz_processor_bind)
	{
		 bind_cpu=(begin_proc+(int)xx)%num_processors;
		 pthread_processor_bind_np(PTHREAD_BIND_FORCED_NP,
                         (pthread_spu_t *)&anwser, (pthread_spu_t)bind_cpu, pthread_self());
		my_nap(40);	/* Switch to new cpu */
	}
#endif
#endif
	if(use_thread)
		nbuff=barray[xx];
	else
		nbuff=buffer;
	if(debug1)
	{
	   if(use_thread)
#ifdef NO_PRINT_LLD
		printf("\nStarting child %ld\n",xx);
#else
		printf("\nStarting child %lld\n",xx);
#endif
	   else
#ifdef NO_PRINT_LLD
		printf("\nStarting process %d slot %ld\n",getpid(),xx);
#else
		printf("\nStarting process %d slot %lld\n",getpid(),xx);
#endif
		
	}
	dummyfile[xx]=(char *)malloc((size_t)MAXNAMESIZE);
#ifdef NO_PRINT_LLD
	sprintf(dummyfile[xx],"%s.DUMMY.%ld",filearray[xx],xx);
#else
	sprintf(dummyfile[xx],"%s.DUMMY.%lld",filearray[xx],xx);
#endif
	if(oflag)
		flags=O_RDONLY|O_SYNC;
	else
		flags=O_RDONLY;
#if defined(linux)
	if(direct_flag)
		flags |=O_DIRECT;
#endif
#ifdef _LARGEFILE64_SOURCE
	if((fd = open64(dummyfile[xx], (int)flags))<0)
	{
		perror(dummyfile[xx]);
		exit(135);
	}
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
	if(async_flag)
		async_init(&gc,fd,direct_flag);
#else
	if(async_flag)
		async_init(&gc,fd,0);
#endif
#endif
#ifdef VXFS
	if(direct_flag)
		ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#else
	if((fd = open(dummyfile[xx], (int)flags))<0)
	{
		perror(dummyfile[xx]);
		exit(136);
	}
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
	if(async_flag)
		async_init(&gc,fd,direct_flag);
#else
	if(async_flag)
		async_init(&gc,fd,0);
#endif
#endif
#ifdef VXFS
	if(direct_flag)
		ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#endif
	if(mmapflag)
	{
		maddr=(char *)initfile(fd,(filebytes64),0,PROT_READ);
	}
	if(r_traj_flag)
		r_traj_fd=open_r_traj();
	if(fetchon)
		fetchit(nbuff,reclen);
        if(Q_flag)
        {
                sprintf(tmpname,"Child_%d_rrol.dat",(int)xx);
                thread_rrqfd=fopen(tmpname,"a");
                if(thread_rrqfd==0)
                {
                        printf("Unable to open %s\n",tmpname);
                        exit(40);
                }
		fprintf(thread_rrqfd,"Offset in Kbytes   Latency in microseconds\n");
        }

	child_stat = (struct child_stats *)&shmaddr[xx];
	child_stat->throughput = 0;
	child_stat->actual = 0;
	child_stat->flag = CHILD_STATE_READY;

	if(distributed && client_iozone)
	{
		tell_master_ready(chid);
		wait_for_master_go(chid);
	}
	else
	
		while(child_stat->flag==CHILD_STATE_READY)	/* wait for parent to say go */
			Poll((long long)1);
	if(file_lock)
		if(mylockf((int) fd, (int) 1, (int)1) != 0)
			printf("File lock for read failed. %d\n",errno);
	starttime1 = time_so_far();
	child_stat->start_time = starttime1;
	if(cpuutilflag)
	{
		walltime = starttime1;
		cputime = cputime_so_far();
	}

	if(r_traj_flag)
		rewind(r_traj_fd);
	for(i=0; i<numrecs64; i++){
		if(disrupt_flag && ((i%DISRUPT)==0))
		{
			disrupt(fd);
		}
		if(r_traj_flag)
		{
			traj_offset=get_traj(r_traj_fd, (long long *)&traj_size,(double *)&delay,(long)0);
			reclen=traj_size;
#ifdef _LARGEFILE64_SOURCE
			lseek64(fd,(off64_t)traj_offset,SEEK_SET);
#else
			lseek(fd,(off_t)traj_offset,SEEK_SET);
#endif
		}
		if(Q_flag)
		{
#ifdef _LARGEFILE64_SOURCE
			traj_offset=lseek64(fd,(off64_t)0,SEEK_CUR);
#else
			traj_offset=lseek(fd,(off_t)0,SEEK_CUR);
#endif
		}
		if(compute_flag)
			compute_val+=do_compute(delay);
		if(*stop_flag)
		{
			if(debug1)
				printf("\n(%ld) Stopped by another 3\n", (long)xx);
			break;
		}
		if(purge)
			purgeit(nbuff,reclen);
                if(Q_flag)
                {
                        thread_qtime_start=time_so_far();
                }
		if(mmapflag)
		{
			wmaddr = &maddr[i*reclen];
			fill_area((long long*)wmaddr,(long long*)nbuff,(long long)reclen);
		}
		else
		{
			  if(async_flag)
			  {
			    if(no_copy_flag)
			      async_read_no_copy(gc, (long long)fd, &buffer1, (i*reclen),reclen,
			    	1LL,(filebytes64),depth);
			    else
			      async_read(gc, (long long)fd, nbuff, (i*reclen),reclen,
			    	1LL,(filebytes64),depth);
			  }
			  else
			  {
			      if(read((int)fd, (void*)nbuff, (size_t) reclen) != reclen)
			      {
				if(*stop_flag)
				{
					if(debug1)
						printf("\n(%ld) Stopped by another 4\n", (long)xx);
					break;
				}
#ifdef NO_PRINT_LLD
		    		printf("\nError writing block %ld, fd= %d\n", i,
					 fd);
#else
		    		printf("\nError writing block %lld, fd= %d\n", i,
					 fd);
#endif
				perror("read");
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = CHILD_STATE_HOLD;
		    		exit(137);
			      }
			   }
		}
		if(verify){
		   if(async_flag && no_copy_flag)
		   {
			if(verify_buffer(buffer1,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = CHILD_STATE_HOLD;
				exit(138);
			}
		   }
		   else
		   {
			if(verify_buffer(nbuff,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = CHILD_STATE_HOLD;
				exit(139);
			}
		   }
		}
		if(async_flag && no_copy_flag)
			async_release(gc);
		re_read_so_far+=reclen/1024;
		r_traj_bytes_completed+=reclen;
		r_traj_ops_completed++;
		if(*stop_flag)
		{
			re_read_so_far-=reclen/1024;
			r_traj_bytes_completed-=reclen;
		}
                if(Q_flag)
                {
                        thread_qtime_stop=time_so_far();
#ifdef NO_PRINT_LLD
                        fprintf(thread_rrqfd,"%10.1ld %10.0f\n",(traj_offset)/1024,((thread_qtime_stop-thread_qtime_start-time_res))*1000000);
#else
                        fprintf(thread_rrqfd,"%10.1lld %10.0f\n",(traj_offset)/1024,((thread_qtime_stop-thread_qtime_start-time_res))*1000000);
#endif
                }

	}
	if(file_lock)
		if(mylockf((int) fd, (int) 0, (int)1))
			printf("Read unlock failed. %d\n",errno);
	/*fsync(fd);*/
#ifdef ASYNC_IO
	if(async_flag)
	{
		end_async(gc);
		gc=0;
	}
#endif
	if(include_flush)
	{
		if(mmapflag)
		{
			msync(maddr,(size_t)(filebytes64),MS_SYNC);
		}else
			fsync(fd);
	}
	if(include_close)
	{	
		if(mmapflag)
		{
			mmap_end(maddr,(unsigned long long)filebytes64);
		}
		close(fd);
	}
	temp_time = time_so_far();
	child_stat->stop_time = temp_time;
	child_stat->throughput = ((temp_time - starttime1)-time_res)
		-compute_val;
	if(child_stat->throughput < (double).000001) 
	{
		child_stat->throughput= time_res;
		if(rec_prob < reclen)
			rec_prob = reclen;
		res_prob=1;
	}

	if(OPS_flag){
	   /*re_read_so_far=(re_read_so_far*1024)/reclen;*/
	   re_read_so_far=r_traj_ops_completed;
	}
	child_stat->throughput = re_read_so_far/child_stat->throughput;
	child_stat->actual = re_read_so_far;
	if(!xflag)
	{
		*stop_flag=1;
		if(distributed && client_iozone)
			send_stop();
	}
	if(distributed && client_iozone)
	{
		tell_master_stats(THREAD_REREAD_TEST,chid, child_stat->throughput,
			child_stat->actual, child_stat->stop_time,
			child_stat->start_time,child_stat->fini_time,(char)*stop_flag,
			(long long)CHILD_STATE_HOLD);
	}
	if(cpuutilflag)
	{
		child_stat->fini_time = time_so_far();
		cputime = cputime_so_far() - cputime;
		if (cputime < cputime_res)
			cputime = 0.0;
		child_stat->cputime = cputime;
		walltime = time_so_far() - walltime;
		if (walltime < cputime_res)
			walltime = 0.0;
		child_stat->walltime = walltime;
	}
	child_stat->flag = CHILD_STATE_HOLD;	/* Tell parent I'm done */
	if(!include_close)
	{
		if(mmapflag)
		{
			msync(maddr,(size_t)(filebytes64),MS_SYNC);
			mmap_end(maddr,(unsigned long long)filebytes64);
		}else
			fsync(fd);
		close(fd);
	}
        if(Q_flag && (thread_rrqfd !=0) )
                fclose(thread_rrqfd);
	free(dummyfile[xx]);
	if(r_traj_flag)
		fclose(r_traj_fd);
	if(debug1)
#ifdef NO_PRINT_LLD
		printf("\nChild finished %ld\n",xx);
#else
		printf("\nChild finished %lld\n",xx);
#endif
#ifdef NO_THREADS
	exit(0);
#else
	if(use_thread)
		thread_exit();
	else
		exit(0);
#endif
return(0);
}

/************************************************************************/
/* Thread_reverse_perf_test				        	*/
/* Reverse read test							*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void *
thread_reverse_read_test(void *x)
#else
void *
thread_reverse_read_test(x)
#endif
{
	long long xx;
	char *nbuff;
	struct child_stats *child_stat;
	long long tt;
	int fd;
	long long flags = 0;
	double walltime, cputime;
	double thread_qtime_stop,thread_qtime_start;
	double starttime2 = 0;
	double delay = 0;
	double temp_time;
	double compute_val = (double)0;
	long long recs_per_buffer;
	off64_t i,t_offset,here;
	off64_t zoffset=0;
	off64_t current_position=0;
	off64_t written_so_far, reverse_read, re_written_so_far,
		re_read_so_far,read_so_far;
	char *dummyfile[MAXSTREAMS];           /* name of dummy file     */
	char *maddr;
	char *wmaddr;
	volatile char *buffer1;
	int anwser,bind_cpu;
	off64_t traj_offset;
	char tmpname[256];
	FILE *thread_revqfd;
#ifdef ASYNC_IO
	struct cache *gc=0;
#else
	long long *gc=0;
#endif
	/*****************/
	/* Children only */
	/*****************/
	written_so_far=read_so_far=reverse_read=re_read_so_far=0;
	recs_per_buffer = cache_size/reclen ;
#ifdef NO_THREADS
	xx=chid;
#else
	if(use_thread)
		xx = (long long)((long)x);
	else
	{
		xx=chid;
	}
#endif
#ifndef NO_THREADS
#ifdef _HPUX_SOURCE
	if(ioz_processor_bind)
	{
		 bind_cpu=(begin_proc+(int)xx)%num_processors;
		 pthread_processor_bind_np(PTHREAD_BIND_FORCED_NP,
                         (pthread_spu_t *)&anwser, (pthread_spu_t)bind_cpu, pthread_self());
		my_nap(40);	/* Switch to new cpu */
	}
#endif
#endif
	if(use_thread)
		nbuff=barray[xx];
	else
		nbuff=buffer;
	if(debug1)
	{
	   if(use_thread)
#ifdef NO_PRINT_LLD
		printf("\nStarting child %ld\n",xx);
#else
		printf("\nStarting child %lld\n",xx);
#endif
	   else
#ifdef NO_PRINT_LLD
		printf("\nStarting process %d slot %ld\n",getpid(),xx);
#else
		printf("\nStarting process %d slot %lld\n",getpid(),xx);
#endif
		
	}
	dummyfile[xx]=(char *)malloc((size_t)MAXNAMESIZE);
#ifdef NO_PRINT_LLD
	sprintf(dummyfile[xx],"%s.DUMMY.%ld",filearray[xx],xx);
#else
	sprintf(dummyfile[xx],"%s.DUMMY.%lld",filearray[xx],xx);
#endif
	if(oflag)
		flags=O_RDONLY|O_SYNC;
	else
		flags=O_RDONLY;
#if defined(linux)
	if(direct_flag)
		flags |=O_DIRECT;
#endif
#ifdef _LARGEFILE64_SOURCE
	if((fd = open64(dummyfile[xx], (int)flags))<0)
	{
		perror(dummyfile[xx]);
		exit(140);
	}
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
	if(async_flag)
		async_init(&gc,fd,direct_flag);
#else
	if(async_flag)
		async_init(&gc,fd,0);
#endif
#endif
#ifdef VXFS
	if(direct_flag)
		ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#else
	if((fd = open(dummyfile[xx], (int)flags))<0)
	{
		perror(dummyfile[xx]);
		exit(141);
	}
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
	if(async_flag)
		async_init(&gc,fd,direct_flag);
#else
	if(async_flag)
		async_init(&gc,fd,0);
#endif
#endif
#ifdef VXFS
	if(direct_flag)
		ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif

#endif
	if(mmapflag)
	{
		maddr=(char *)initfile(fd,(numrecs64*reclen),0,PROT_READ);
	}
	if(fetchon)
		fetchit(nbuff,reclen);
        if(Q_flag)
        {
                sprintf(tmpname,"Child_%d_revol.dat",(int)xx);
                thread_revqfd=fopen(tmpname,"a");
                if(thread_revqfd==0)
                {
                        printf("Unable to open %s\n",tmpname);
                        exit(40);
                }
		fprintf(thread_revqfd,"Offset in Kbytes   Latency in microseconds\n");
        }
	child_stat = (struct child_stats *)&shmaddr[xx];
	child_stat->throughput = 0;
	child_stat->actual = 0;
	child_stat->flag = CHILD_STATE_READY;
        if(distributed && client_iozone)
        {
                tell_master_ready(chid);
                wait_for_master_go(chid);
        }
        else
        {
                while(child_stat->flag!=CHILD_STATE_BEGIN)   /* Wait for signal from parent */
                        Poll((long long)1);
        }
	starttime2 = time_so_far();
	child_stat->start_time = starttime2;
	if(cpuutilflag)
	{
		walltime = starttime2;
		cputime = cputime_so_far();
	}

#ifdef _LARGEFILE64_SOURCE
	t_offset = (off64_t)reclen;
	if (!(h_flag || k_flag || mmapflag))
	{
	  if((lseek64( fd, -t_offset, SEEK_END ))<0)
	  {
		perror("lseek64");
		exit(142);
	  };
	}
#else
	if (!(h_flag || k_flag || mmapflag))
	{
	  if((lseek( fd, (off_t)-reclen, SEEK_END ))<0)
	  {
		perror("lseek");
		exit(143);
	  };
	}
#endif
	current_position=(reclen*numrecs64)-reclen;
	if(file_lock)
		if(mylockf((int) fd, (int) 1, (int)1)!=0)
			printf("File lock for read failed. %d\n",errno);
	for(i=0; i<numrecs64; i++) 
	{
		if(disrupt_flag && ((i%DISRUPT)==0))
		{
			disrupt(fd);
		}
		if(compute_flag)
			compute_val+=do_compute(delay);
		if(Q_flag)
		{
#ifdef _LARGEFILE64_SOURCE
			traj_offset=lseek64(fd,(off64_t)0,SEEK_CUR);
#else
			traj_offset=lseek(fd,(off_t)0,SEEK_CUR);
#endif
		}
		if(*stop_flag)
		{
			if(debug1)
				printf("\n(%ld) Stopped by another 3\n", (long)xx);
			break;
		}
		if(purge)
			purgeit(nbuff,reclen);
                if(Q_flag)
                {
                        thread_qtime_start=time_so_far();
                }
		if(mmapflag)
		{
			wmaddr = &maddr[current_position];
			fill_area((long long*)wmaddr,(long long*)nbuff,(long long)reclen);
		}
		else
		{
			  if(async_flag)
			  {
			    if(no_copy_flag)
			      async_read_no_copy(gc, (long long)fd, &buffer1, (current_position),
			      	reclen, -1LL,(numrecs64*reclen),depth);
			    else
			      async_read(gc, (long long)fd, nbuff, (current_position),reclen,
			    	-1LL,(numrecs64*reclen),depth);
			  }
			  else
			  {
			      if(read((int)fd, (void*)nbuff, (size_t) reclen) != reclen)
			      {
				if(*stop_flag)
				{
					if(debug1)
						printf("\n(%ld) Stopped by another 4\n", (long)xx);
					break;
				}
#ifdef NO_PRINT_LLD
				printf("\nError reading block %ld\n", i); 
#else
				printf("\nError reading block %lld\n", i); 
#endif
				perror("read");
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = CHILD_STATE_HOLD;
				exit(144);
			      }
			  }
		}
		current_position+=reclen;
		if(verify){
		   if(async_flag && no_copy_flag)
		   {
			if(verify_buffer(buffer1,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = CHILD_STATE_HOLD;
				exit(145);
			}
		   }
		   else
		   {
			if(verify_buffer(nbuff,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = CHILD_STATE_HOLD;
				exit(146);
			}
		   }
		}
		if(async_flag && no_copy_flag)
			async_release(gc);
#ifdef _LARGEFILE64_SOURCE
		t_offset = (off64_t)reclen*2;
		if (!(h_flag || k_flag || mmapflag))
		{
		  lseek64( fd, -t_offset, SEEK_CUR );
		}
#else
		if (!(h_flag || k_flag || mmapflag))
		{
		  lseek( fd, (off_t)(-2*reclen), SEEK_CUR );
		}
#endif
		current_position-=(2 *reclen);
		reverse_read +=reclen/1024;
		if(*stop_flag)
		{
			reverse_read -=reclen/1024;
		}
                if(Q_flag)
                {
                        thread_qtime_stop=time_so_far();
#ifdef NO_PRINT_LLD
                        fprintf(thread_revqfd,"%10.1ld %10.0f\n",(traj_offset)/1024,((thread_qtime_stop-thread_qtime_start-time_res))*1000000);
#else
                        fprintf(thread_revqfd,"%10.1lld %10.0f\n",(traj_offset)/1024,((thread_qtime_stop-thread_qtime_start-time_res))*1000000);
#endif
                }
	}
	if(file_lock)
		if(mylockf((int) fd,(int)0, (int)1))
			printf("Read unlock failed %d\n",errno);
#ifdef ASYNC_IO
	if(async_flag)
	{
		end_async(gc);
		gc=0;
	}
#endif
	if(include_flush)
	{
		if(mmapflag)
		{
			msync(maddr,(size_t)(numrecs64*reclen),MS_SYNC);
		}else
			fsync(fd);
	}
	if(include_close)
	{	
		if(mmapflag)
		{
			mmap_end(maddr,(unsigned long long)numrecs64*reclen);
		}
		close(fd);
	}
	temp_time = time_so_far();
	child_stat->stop_time = temp_time;
	child_stat->throughput = ((temp_time - starttime2)-time_res)
		-compute_val;
	if(child_stat->throughput < (double).000001) 
	{
		child_stat->throughput= time_res;
		if(rec_prob < reclen)
			rec_prob = reclen;
		res_prob=1;
	}
	if(OPS_flag){
	   reverse_read=(reverse_read*1024)/reclen;
	}
	child_stat->throughput = reverse_read/child_stat->throughput;
	child_stat->actual = reverse_read;
	if(!xflag)
	{
		*stop_flag=1;
		if(distributed && client_iozone)
			send_stop();
	}
        if(distributed && client_iozone)
                tell_master_stats(THREAD_REVERSE_READ_TEST, chid, child_stat->throughput,
                        child_stat->actual, child_stat->stop_time,
                        child_stat->start_time,child_stat->fini_time,(char)*stop_flag,
			(long long)CHILD_STATE_HOLD);
	if(cpuutilflag)
	{
		child_stat->fini_time = time_so_far();
		cputime = cputime_so_far() - cputime;
		if (cputime < cputime_res)
			cputime = 0.0;
		child_stat->cputime = cputime;
		walltime = time_so_far() - walltime;
		if (walltime < cputime_res)
			walltime = 0.0;
		child_stat->walltime = walltime;
	}
	child_stat->flag = CHILD_STATE_HOLD;	/* Tell parent I'm done */
	if(!include_close)
	{
		if(mmapflag)
		{
			msync(maddr,(size_t)(numrecs64*reclen),MS_SYNC);
			mmap_end(maddr,(unsigned long long)numrecs64*reclen);
		}else
			fsync(fd);
		close(fd);
	}
	free(dummyfile[xx]);
        if(Q_flag && (thread_revqfd !=0) )
                fclose(thread_revqfd);
	if(debug1)
#ifdef NO_PRINT_LLD
		printf("\nChild finished %ld\n",xx);
#else
		printf("\nChild finished %lld\n",xx);
#endif
#ifdef NO_THREADS
	exit(0);
#else
	if(use_thread)
		thread_exit();
	else
		exit(0);
#endif
return(0);
}
/************************************************************************/
/* Thread_stride_read_test				        	*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void *
thread_stride_read_test(void *x)
#else
void *
thread_stride_read_test(x)
#endif
{
	long long xx;
	off64_t xx64;
	char *nbuff;
	struct child_stats *child_stat;
	double walltime, cputime;
	long long tt;
	int fd;
	long long flags = 0;
	double thread_qtime_stop,thread_qtime_start;
	double starttime2 = 0;
	double delay = 0;
	double compute_val = (double)0;
	double temp_time;
	long long recs_per_buffer;
	off64_t i;
	off64_t savepos64=0;
	off64_t written_so_far, stride_read, re_written_so_far,
		re_read_so_far,read_so_far;
	off64_t internal_offset = 0;
	off64_t stripewrap = 0;
	off64_t current_position = 0;
	char *dummyfile[MAXSTREAMS];           /* name of dummy file     */
	char *maddr;
	char *wmaddr;
	volatile char *buffer1;
	int anwser,bind_cpu;
	off64_t traj_offset;
	char tmpname[256];
	FILE *thread_strqfd;
#ifdef ASYNC_IO
	struct cache *gc=0;
#else
	long long *gc=0;
#endif
	/*****************/
	/* Children only */
	/*****************/
	written_so_far=read_so_far=stride_read=re_read_so_far=0;
	recs_per_buffer = cache_size/reclen ;
#ifdef NO_THREADS
	xx=chid;
#else
	if(use_thread)
		xx = (long long)((long)x);
	else
	{
		xx=chid;
	}
#endif
#ifndef NO_THREADS
#ifdef _HPUX_SOURCE
	if(ioz_processor_bind)
	{
		 bind_cpu=(begin_proc+(int)xx)%num_processors;
		 pthread_processor_bind_np(PTHREAD_BIND_FORCED_NP,
                         (pthread_spu_t *)&anwser, (pthread_spu_t)bind_cpu, pthread_self());
		my_nap(40);	/* Switch to new cpu */
	}
#endif
#endif
	if(use_thread)
		nbuff=barray[xx];
	else
		nbuff=buffer;
	if(debug1)
	{
	   if(use_thread)
#ifdef NO_PRINT_LLD
		printf("\nStarting child %ld\n",xx);
#else
		printf("\nStarting child %lld\n",xx);
#endif
	   else
#ifdef NO_PRINT_LLD
		printf("\nStarting process %d slot %ld\n",getpid(),xx);
#else
		printf("\nStarting process %d slot %lld\n",getpid(),xx);
#endif
		
	}
	dummyfile[xx]=(char *)malloc((size_t)MAXNAMESIZE);
#ifdef NO_PRINT_LLD
	sprintf(dummyfile[xx],"%s.DUMMY.%ld",filearray[xx],xx);
#else
	sprintf(dummyfile[xx],"%s.DUMMY.%lld",filearray[xx],xx);
#endif
	if(oflag)
		flags=O_RDONLY|O_SYNC;
	else
		flags=O_RDONLY;
#if defined(linux)
	if(direct_flag)
		flags |=O_DIRECT;
#endif
#ifdef _LARGEFILE64_SOURCE
	if((fd = open64(dummyfile[xx], (int)flags))<0)
	{
		perror(dummyfile[xx]);
		exit(147);
	}
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
	if(async_flag)
		async_init(&gc,fd,direct_flag);
#else
	if(async_flag)
		async_init(&gc,fd,0);
#endif
#endif
#ifdef VXFS
	if(direct_flag)
		ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif

#else
	if((fd = open(dummyfile[xx], (int)flags))<0)
	{
		perror(dummyfile[xx]);
		exit(148);
	}
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
	if(async_flag)
	async_init(&gc,fd,direct_flag);
#else
	if(async_flag)
		async_init(&gc,fd,0);
#endif
#endif
#ifdef VXFS
	if(direct_flag)
		ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif

#endif
	if(mmapflag)
	{
		maddr=(char *)initfile(fd,(numrecs64*reclen),0,PROT_READ);
	}
	if(fetchon)
		fetchit(nbuff,reclen);
        if(Q_flag)
        {
                sprintf(tmpname,"Child_%d_strol.dat",(int)xx);
                thread_strqfd=fopen(tmpname,"a");
                if(thread_strqfd==0)
                {
                        printf("Unable to open %s\n",tmpname);
                        exit(40);
                }
		fprintf(thread_strqfd,"Offset in Kbytes   Latency in microseconds\n");
        }
	child_stat = (struct child_stats *)&shmaddr[xx];
	child_stat->throughput = 0;
	child_stat->actual = 0;
	child_stat->flag = CHILD_STATE_READY;
        if(distributed && client_iozone)
        {
                tell_master_ready(chid);
                wait_for_master_go(chid);
        }
        else

                while(child_stat->flag==CHILD_STATE_READY)      /* wait for parent to say go */
                        Poll((long long)1);
	if(file_lock)
		if(mylockf((int) fd, (int) 1,  (int)1)!=0)
			printf("File lock for write failed. %d\n",errno);
	starttime2 = time_so_far();
	child_stat->start_time = starttime2;
	if(cpuutilflag)
	{
		walltime = starttime2;
		cputime = cputime_so_far();
	}
	for(i=0; i<numrecs64; i++){
		if(disrupt_flag && ((i%DISRUPT)==0))
		{
			disrupt(fd);
		}
		if(compute_flag)
			compute_val+=do_compute(delay);
		if(Q_flag)
		{
#ifdef _LARGEFILE64_SOURCE
			traj_offset=lseek64(fd,(off64_t)0,SEEK_CUR);
#else
			traj_offset=lseek(fd,(off_t)0,SEEK_CUR);
#endif
		}
		if(*stop_flag)
		{
			if(debug1)
				printf("\n(%ld) Stopped by another 3\n", (long)xx);
			break;
		}
		if(purge)
			purgeit(nbuff,reclen);
                if(Q_flag)
                {
                        thread_qtime_start=time_so_far();
                }
		if(verify)
			savepos64=current_position/(off64_t)reclen;
		if(mmapflag)
		{
			wmaddr = &maddr[current_position];
			fill_area((long long*)wmaddr,(long long*)nbuff,(long long)reclen);
		}
		else
		{
			if(async_flag)
			{
			    if(no_copy_flag)
			      async_read_no_copy(gc, (long long)fd, &buffer1, (current_position),
			      	reclen, stride,(numrecs64*reclen),depth);
			    else
			      async_read(gc, (long long)fd, nbuff, (current_position),reclen,
			    	stride,(numrecs64*reclen),depth);
			}
			else
			{
			  if(read((int)fd, (void*)nbuff, (size_t) reclen) != reclen)
			  {
				if(*stop_flag)
				{
					if(debug1)
						printf("\n(%ld) Stopped by another 4\n", (long)xx);
					break;
				}
#ifdef NO_PRINT_LLD
		    		printf("\nError reading block %ld, fd= %d\n", i, fd);
#else
		    		printf("\nError reading block %lld, fd= %d\n", i, fd);
#endif
				perror("read");
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = CHILD_STATE_HOLD;
		    		exit(149);
			  }
			}
		}
		current_position+=reclen;
		if(verify){
		   if(async_flag && no_copy_flag)
		   {
			if(verify_buffer(buffer1,reclen,(off64_t)savepos64,reclen,(long long)pattern,sverify)){
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = CHILD_STATE_HOLD;
				exit(150);
			}
		   }
		   else
		   {
			if(verify_buffer(nbuff,reclen,(off64_t)savepos64,reclen,(long long)pattern,sverify)){
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = CHILD_STATE_HOLD;
				exit(151);
			}
		   }
		}
		if(async_flag && no_copy_flag)
			async_release(gc);
		if(current_position + (stride * reclen) >= (numrecs64 * reclen)-reclen) 
		{
			current_position=0;

			stripewrap++;
			  
			if(numrecs64 <= stride)
			{
				current_position=0;
			}
			else
			{
#ifdef _LARGEFILE64_SOURCE
				current_position = (off64_t)((stripewrap)%numrecs64)*reclen;
#else
				current_position = (long long)((stripewrap)%numrecs64)*reclen;
#endif
			}
			if (!(h_flag || k_flag || mmapflag))
			{
#ifdef _LARGEFILE64_SOURCE
			  if(lseek64(fd,current_position,SEEK_SET)<0)
			  {
				perror("lseek64");
				exit(152);
			  }
#else
			  if(lseek(fd,(off_t)current_position,SEEK_SET)<0)
			  {
				perror("lseek");
				exit(153);
			  }
#endif
			}
		}
		else			
		{
			current_position+=(stride*reclen)-reclen;
			if (!(h_flag || k_flag || mmapflag))
			{
#ifdef _LARGEFILE64_SOURCE
			  if(lseek64(fd,(off64_t)current_position,SEEK_SET)<0)
			  {
				perror("lseek64");
				exit(154);
			  };
#else
			  if(lseek(fd,(off_t)current_position,SEEK_SET)<0)
			  {
				perror("lseek");
				exit(155);
			  };
#endif
			}
		}
		stride_read +=reclen/1024;
		if(*stop_flag)
		{
			stride_read -=reclen/1024;
		}
                if(Q_flag)
                {
                        thread_qtime_stop=time_so_far();
#ifdef NO_PRINT_LLD
                        fprintf(thread_strqfd,"%10.1ld %10.0f\n",(traj_offset)/1024,((thread_qtime_stop-thread_qtime_start-time_res))*1000000);
#else
                        fprintf(thread_strqfd,"%10.1lld %10.0f\n",(traj_offset)/1024,((thread_qtime_stop-thread_qtime_start-time_res))*1000000);
#endif
                }
	}
	if(file_lock)
		if(mylockf((int) fd,(int)0,(int)1))
			printf("Read unlock failed %d\n",errno);
#ifdef ASYNC_IO
	if(async_flag)
	{
		end_async(gc);
		gc=0;
	}
#endif
	if(include_flush)
	{
		if(mmapflag)
		{
			msync(maddr,(size_t)(numrecs64*reclen),MS_SYNC);
		}else
			fsync(fd);
	}
	if(include_close)
	{	
		if(mmapflag)
		{
			mmap_end(maddr,(unsigned long long)numrecs64*reclen);
		}
		close(fd);
	}
	temp_time = time_so_far();
	child_stat->stop_time = temp_time;
	child_stat->throughput = ((temp_time - starttime2)-time_res)
		-compute_val;
	if(child_stat->throughput < (double).000001) 
	{
		child_stat->throughput= time_res;
		if(rec_prob < reclen)
			rec_prob = reclen;
		res_prob=1;
	}
	if(OPS_flag){
	   stride_read=(stride_read*1024)/reclen;
	}
	child_stat->throughput = stride_read/child_stat->throughput;
	child_stat->actual = stride_read;
	if(!xflag)
	{
		*stop_flag=1;
		if(distributed && client_iozone)
			send_stop();
	}
        if(distributed && client_iozone)
        {
                tell_master_stats(THREAD_STRIDE_TEST,chid, child_stat->throughput,
                        child_stat->actual, child_stat->stop_time,
                        child_stat->start_time,child_stat->fini_time,(char)*stop_flag,
                        (long long)CHILD_STATE_HOLD);
        }
	if(cpuutilflag)
	{
		child_stat->fini_time = time_so_far();
		cputime = cputime_so_far() - cputime;
		if (cputime < cputime_res)
			cputime = 0.0;
		child_stat->cputime = cputime;
		walltime = time_so_far() - walltime;
		if (walltime < cputime_res)
			walltime = 0.0;
		child_stat->walltime = walltime;
	}
	child_stat->flag = CHILD_STATE_HOLD;	/* Tell parent I'm done */
	if(!include_close)
	{
		if(mmapflag)
		{
			msync(maddr,(size_t)(numrecs64*reclen),MS_SYNC);
			mmap_end(maddr,(unsigned long long)numrecs64*reclen);
		}else
			fsync(fd);
		close(fd);
	}
        if(Q_flag && (thread_strqfd !=0) )
                fclose(thread_strqfd);
	free(dummyfile[xx]);
	if(debug1)
#ifdef NO_PRINT_LLD
		printf("\nChild finished %ld\n",xx);
#else
		printf("\nChild finished %lld\n",xx);
#endif
#ifdef NO_THREADS
	exit(0);
#else
	if(use_thread)
		thread_exit();
	else
		exit(0);
#endif
return(0);
}

/************************************************************************/
/* Thread random read test				        	*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void *
thread_ranread_test(void *x)
#else
void *
thread_ranread_test(x)
#endif
{
	long long xx;
	struct child_stats *child_stat;
	long long tt;
	double walltime, cputime;
	int fd;
	long long flags = 0;
	double thread_qtime_stop,thread_qtime_start;
	double starttime1 = 0;
	double delay = 0;
	double temp_time;
	double compute_val = (double)0;
	off64_t written_so_far, ranread_so_far, re_written_so_far,re_read_so_far;
	long long recs_per_buffer;
	off64_t current_offset=0;
	off64_t i;
	char *dummyfile[MAXSTREAMS];           /* name of dummy file     */
	char *nbuff;
	char *maddr;
	char *wmaddr;
	volatile char *buffer1;
	int anwser,bind_cpu;
	off64_t traj_offset;
	char tmpname[256];
	FILE *thread_randrfd;
#ifdef ASYNC_IO
	struct cache *gc=0;
#else
	long long *gc=0;
#endif

	written_so_far=ranread_so_far=re_written_so_far=re_read_so_far=0;
	recs_per_buffer = cache_size/reclen ;
#ifdef NO_THREADS
	xx=chid;
#else
	if(use_thread)
		xx = (long long)((long)x);
	else
	{
		xx=chid;
	}
#endif
#ifndef NO_THREADS
#ifdef _HPUX_SOURCE
	if(ioz_processor_bind)
	{
		 bind_cpu=(begin_proc+(int)xx)%num_processors;
		 pthread_processor_bind_np(PTHREAD_BIND_FORCED_NP,
                         (pthread_spu_t *)&anwser, (pthread_spu_t)bind_cpu, pthread_self());
		my_nap(40);	/* Switch to new cpu */
	}
#endif
#endif
	if(use_thread)
		nbuff=barray[xx];
	else
		nbuff=buffer;
	dummyfile[xx]=(char *)malloc((size_t)MAXNAMESIZE);
#ifdef NO_PRINT_LLD
	sprintf(dummyfile[xx],"%s.DUMMY.%ld",filearray[xx],xx);
#else
	sprintf(dummyfile[xx],"%s.DUMMY.%lld",filearray[xx],xx);
#endif
	if(oflag)
		flags=O_RDONLY|O_SYNC;
	else
		flags=O_RDONLY;
#if defined(linux)
	if(direct_flag)
		flags |=O_DIRECT;
#endif
#ifdef _LARGEFILE64_SOURCE
	if((fd = open64(dummyfile[xx], (int)flags))<0)
	{
		perror(dummyfile[xx]);
		exit(156);
	}
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
	if(async_flag)
		async_init(&gc,fd,direct_flag);
#else
	if(async_flag)
		async_init(&gc,fd,0);
#endif
#endif

#ifdef VXFS
	if(direct_flag)
		ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif

#else
	if((fd = open(dummyfile[xx], (int)flags))<0)
	{
		perror(dummyfile[xx]);
		exit(157);
	}
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
	if(async_flag)
		async_init(&gc,fd,direct_flag);
#else
	if(async_flag)
		async_init(&gc,fd,0);
#endif
#endif
#ifdef VXFS
	if(direct_flag)
		ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif

#endif
	if(mmapflag)
	{
		maddr=(char *)initfile(fd,(numrecs64*reclen),0,PROT_READ);
	}
	child_stat = (struct child_stats *)&shmaddr[xx];
	child_stat->throughput = 0;
	child_stat->actual = 0;
	if(debug1)
	{
	   if(use_thread)
#ifdef NO_PRINT_LLD
		printf("\nStarting child %ld\n",xx);
#else
		printf("\nStarting child %lld\n",xx);
#endif
	   else
#ifdef NO_PRINT_LLD
		printf("\nStarting process %d slot %ld\n",getpid(),xx);
#else
		printf("\nStarting process %d slot %lld\n",getpid(),xx);
#endif
		
	}
	/*****************/
	/* Children only */
	/*****************/
	if(fetchon)
		fetchit(nbuff,reclen);
        if(Q_flag)
        {
                sprintf(tmpname,"Child_%d_randrol.dat",(int)xx);
                thread_randrfd=fopen(tmpname,"a");
                if(thread_randrfd==0)
                {
                        printf("Unable to open %s\n",tmpname);
                        exit(40);
                }
		fprintf(thread_randrfd,"Offset in Kbytes   Latency in microseconds\n");
        }
	child_stat=(struct child_stats *)&shmaddr[xx];
	child_stat->flag = CHILD_STATE_READY;
        if(distributed && client_iozone)
        {
                tell_master_ready(chid);
                wait_for_master_go(chid);
        }
        else
        {
                while(child_stat->flag!=CHILD_STATE_BEGIN)   /* Wait for signal from parent */
                        Poll((long long)1);
        }
	starttime1 = time_so_far();
	child_stat->start_time = starttime1;
	if(cpuutilflag)
	{
		walltime = starttime1;
		cputime = cputime_so_far();
	}

#ifdef bsd4_2
        srand();
#else
#ifdef Windows
        srand();
#else
	srand48(0);
#endif
#endif
	if(file_lock)
		if(mylockf((int) fd, (int) 1, (int)1)!=0)
			printf("File lock for read failed. %d\n",errno);
	for(i=0; i<numrecs64; i++) {
		if(compute_flag)
			compute_val+=do_compute(delay);
		if(*stop_flag)
		{
			if(debug1)
				printf("\n(%ld) Stopped by another 2\n", (long)xx);
			break;
		}
		if(purge)
			purgeit(nbuff,reclen);
#ifdef bsd4_2
		current_offset = (off64_t)reclen * (rand()%numrecs64);
#else
#ifdef Windows
                current_offset = (off64_t)reclen * (rand()%numrecs64);
#else
		current_offset = reclen * (lrand48()%numrecs64);
#endif
#endif

		if (!(h_flag || k_flag || mmapflag))
		{
#ifdef _LARGEFILE64_SOURCE
		  if(lseek64( fd, current_offset, SEEK_SET )<0)
		  {
			perror("lseek64");
			exit(158);
		  };
#else
		  if(lseek( fd, (off_t)current_offset, SEEK_SET )<0)
		  {
			perror("lseek");
			exit(159);
		  };
#endif
		}
		if(Q_flag)
		{
#ifdef _LARGEFILE64_SOURCE
			traj_offset=lseek64(fd,(off64_t)0,SEEK_CUR);
#else
			traj_offset=lseek(fd,(off_t)0,SEEK_CUR);
#endif
                        thread_qtime_start=time_so_far();
		}
		if(mmapflag)
		{
			wmaddr = &maddr[current_offset];
			fill_area((long long*)wmaddr,(long long*)nbuff,(long long)reclen);
		}
		else
		{
			if(async_flag)
			{
			    if(no_copy_flag)
			      async_read_no_copy(gc, (long long)fd, &buffer1, (current_offset),
			      	 reclen, 0LL,(numrecs64*reclen),depth);
			    else
			      async_read(gc, (long long)fd, nbuff, (current_offset), reclen,
			    	0LL,(numrecs64*reclen),depth);
			}
			else
			{
	  		  if(read((int)fd, (void*)nbuff, (size_t)reclen) != reclen)
	  		  {
				if(*stop_flag)
				{
					if(debug1)
						printf("\n(%ld) Stopped by another 2\n", (long)xx);
					break;
				}
#ifdef NO_PRINT_LLD
				printf("\nError reading block at %ld\n",
					 offset); 
#else
				printf("\nError reading block at %lld\n",
					 offset); 
#endif
				perror("ranread");
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = CHILD_STATE_HOLD;
				exit(160);
	 		  }
			}
		}
		current_offset+=reclen;
		if(verify){
		   if(async_flag && no_copy_flag)
		   {
			if(verify_buffer(buffer1,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = CHILD_STATE_HOLD;
				exit(161);
			}
		   }
		   else
		   {
			if(verify_buffer(nbuff,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = CHILD_STATE_HOLD;
				exit(162);
			}
		   }
		}
		if(async_flag && no_copy_flag)
			async_release(gc);
		ranread_so_far+=reclen/1024;
		if(*stop_flag)
		{
			ranread_so_far-=reclen/1024;
		}
                if(Q_flag)
                {
                        thread_qtime_stop=time_so_far();
#ifdef NO_PRINT_LLD
                        fprintf(thread_randrfd,"%10.1ld %10.0f\n",(traj_offset)/1024,((thread_qtime_stop-thread_qtime_start-time_res))*1000000);
#else
                        fprintf(thread_randrfd,"%10.1lld %10.0f\n",(traj_offset)/1024,((thread_qtime_stop-thread_qtime_start-time_res))*1000000);
#endif
                }
	}
	if(file_lock)
		if(mylockf((int) fd,(int)0,(int)1))
			printf("Read unlock failed %d\n",errno);
#ifdef ASYNC_IO
	if(async_flag)
	{
		end_async(gc);
		gc=0;
	}
#endif
	if(include_flush)
	{
		if(mmapflag)
		{
			msync(maddr,(size_t)(numrecs64*reclen),MS_SYNC);
		}else
			fsync(fd);
	}
	if(include_close)
	{	
		if(mmapflag)
		{
			mmap_end(maddr,(unsigned long long)numrecs64*reclen);
		}
		close(fd);
	}
	temp_time = time_so_far();
	child_stat->stop_time = temp_time;
	child_stat=(struct child_stats *)&shmaddr[xx];
	child_stat->throughput = ((temp_time - starttime1)-time_res)
		-compute_val;
	if(child_stat->throughput < (double).000001) 
	{
		child_stat->throughput= time_res;
		if(rec_prob < reclen)
			rec_prob = reclen;
		res_prob=1;
	}
	if(OPS_flag){
	   ranread_so_far=(ranread_so_far*1024)/reclen;
	}
	child_stat->throughput = ranread_so_far/child_stat->throughput;
	child_stat->actual = ranread_so_far;
	if(!xflag)
	{
		*stop_flag=1;
		if(distributed && client_iozone)
			send_stop();
	}
        if(cdebug)
                fprintf(newstdout,"Child: throughput %f actual %f \n",child_stat->throughput,
                        child_stat->actual);
        if(distributed && client_iozone)
                tell_master_stats(THREAD_RANDOM_READ_TEST, chid, child_stat->throughput,
                        child_stat->actual, child_stat->stop_time,
                        child_stat->start_time,child_stat->fini_time,(char)*stop_flag,
                        (long long)CHILD_STATE_HOLD);
	if(cpuutilflag)
	{
		child_stat->fini_time = time_so_far();
		cputime = cputime_so_far() - cputime;
		if (cputime < cputime_res)
			cputime = 0.0;
		child_stat->cputime = cputime;
		walltime = time_so_far() - walltime;
		if (walltime < cputime_res)
			walltime = 0.0;
		child_stat->walltime = walltime;
	}
	child_stat->flag = CHILD_STATE_HOLD; 	/* Tell parent I'm done */
	if(!include_close)
	{
		if(mmapflag)
		{
			msync(maddr,(size_t)(numrecs64*reclen),MS_SYNC);
			mmap_end(maddr,(unsigned long long)numrecs64*reclen);
		}else
			fsync(fd);
		close(fd);
	}
        if(Q_flag && (thread_randrfd !=0) )
                fclose(thread_randrfd);
	free(dummyfile[xx]);
	if(debug1)
#ifdef NO_PRINT_LLD
		printf("\nChild finished %ld\n",xx);
#else
		printf("\nChild finished %lld\n",xx);
#endif
#ifdef NO_THREADS
	exit(0);
#else
	if(use_thread)
		thread_exit();
	else
		exit(0);
#endif
return(0);
}

/************************************************************************/
/* Thread random write test			        		*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void *
thread_ranwrite_test(void *x)
#else
void *
thread_ranwrite_test( x)
#endif
{

	struct child_stats *child_stat;
	double starttime1 = 0;
	double temp_time;
	double walltime, cputime;
	double compute_val = (double)0;
	double delay = (double)0;
	double thread_qtime_stop,thread_qtime_start;
	off64_t traj_offset;
	off64_t current_offset=0;
	long long tt,flags,traj_size;
	long long w_traj_bytes_completed;
	long long w_traj_ops_completed;
	FILE *w_traj_fd;
	int fd;
	long long recs_per_buffer;
	long long stopped,i;
	off64_t written_so_far, read_so_far, re_written_so_far,re_read_so_far;
	long long xx;
	char *dummyfile [MAXSTREAMS];           /* name of dummy file     */
	char *nbuff;
	char *maddr;
	char *wmaddr,*free_addr;
	int anwser,bind_cpu,wval;
	off64_t filebytes64;
	char tmpname[256];
	FILE *thread_randwqfd;

#ifdef ASYNC_IO
	struct cache *gc=0;

#else
	long long *gc=0;
#endif

	filebytes64 = numrecs64*reclen;
	written_so_far=read_so_far=re_written_so_far=re_read_so_far=0;
	w_traj_bytes_completed=w_traj_ops_completed=0;
	recs_per_buffer = cache_size/reclen ;
#ifdef NO_THREADS
	xx=chid;
#else
	if(use_thread)
	{
		xx = (long long)((long)x);
	}
	else
	{
		xx=chid;
	}
#endif
#ifndef NO_THREADS
#ifdef _HPUX_SOURCE
	if(ioz_processor_bind)
	{
		 bind_cpu=(begin_proc+(int)xx)%num_processors;
		 pthread_processor_bind_np(PTHREAD_BIND_FORCED_NP,
                         (pthread_spu_t *)&anwser, (pthread_spu_t)bind_cpu, pthread_self());
		my_nap(40);	/* Switch to new cpu */
	}
#endif
#endif
	if(use_thread)
		nbuff=barray[xx];
	else
		nbuff=buffer;
	if(debug1 )
	{
	   if(use_thread)
#ifdef NO_PRINT_LLD
		printf("\nStarting child %ld\n",xx);
#else
		printf("\nStarting child %lld\n",xx);
#endif
	   else
#ifdef NO_PRINT_LLD
		printf("\nStarting process %d slot %ld\n",getpid(),xx);
#else
		printf("\nStarting process %d slot %lld\n",getpid(),xx);
#endif
		
	}
	dummyfile[xx]=(char *)malloc((size_t)MAXNAMESIZE);
#ifdef NO_PRINT_LLD
	sprintf(dummyfile[xx],"%s.DUMMY.%ld",filearray[xx],xx);
#else
	sprintf(dummyfile[xx],"%s.DUMMY.%lld",filearray[xx],xx);
#endif
	/*****************/
	/* Children only */
	/*******************************************************************/
	/* Random write throughput performance test. **********************/
	/*******************************************************************/
#ifdef foobar
#ifdef _LARGEFILE64_SOURCE
	if((fd = creat64(dummyfile[xx], 0640))<0)
	{
		perror(dummyfile[xx]);
		exit(123);
	}
#else
	if((fd = creat(dummyfile[xx], 0640))<0)
	{
		perror(dummyfile[xx]);
		exit(124);
	}
#endif
	close(fd);
#endif
	if(oflag)
		flags=O_RDWR|O_SYNC;
	else
		flags=O_RDWR;
#if defined(linux)
	if(direct_flag)
		flags |=O_DIRECT;
#endif
#ifdef _LARGEFILE64_SOURCE
	if((fd = open64(dummyfile[xx], (int)flags))<0)
	{
		printf("\nCan not open temp file: %s\n", 
			filename);
		perror("open");
		exit(125);
	}
#else
	if((fd = open(dummyfile[xx], (int)flags))<0)
	{
		printf("\nCan not open temp file: %s\n", 
			filename);
		perror("open");
		exit(126);
	}
#endif
#ifdef VXFS
	if(direct_flag)
		ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#ifdef ASYNC_IO
#ifdef _HPUX_SOURCE
	if(async_flag)
		async_init(&gc,fd,direct_flag);
#else
	if(async_flag)
		async_init(&gc,fd,0);
#endif
#endif
	if(mmapflag)
	{
		maddr=(char *)initfile(fd,(filebytes64),1,PROT_READ|PROT_WRITE);
	}
	if(reclen < cache_size )
	{
		recs_per_buffer = cache_size/reclen ;
		nbuff=&nbuff[(xx%recs_per_buffer)*reclen];
	}
	if(fetchon)			/* Prefetch into processor cache */
		fetchit(nbuff,reclen);
	if(verify)
		fill_buffer(nbuff,reclen,(long long)pattern,sverify);

	child_stat = (struct child_stats *)&shmaddr[xx];	
	child_stat->throughput = 0;
	child_stat->actual = 0;
	child_stat->flag=CHILD_STATE_READY; /* Tell parent child is ready to go */
	if(distributed && client_iozone)
	{
		tell_master_ready(chid);
		wait_for_master_go(chid);
	}
	else
	{
		while(child_stat->flag!=CHILD_STATE_BEGIN)   /* Wait for signal from parent */
			Poll((long long)1);
	}
	written_so_far=0;
	child_stat = (struct child_stats *)&shmaddr[xx];
	child_stat->actual = 0;
	child_stat->throughput = 0;
	stopped=0;
	if(file_lock)
		if(mylockf((int) fd, (int) 1, (int)0) != 0)
			printf("File lock for write failed. %d\n",errno);
	if(Q_flag)
	{
		sprintf(tmpname,"Child_%d_randwol.dat",(int)xx);
		thread_randwqfd=fopen(tmpname,"a");
		if(thread_randwqfd==0)
		{
			printf("Unable to open %s\n",tmpname);
			exit(40);
		}
		fprintf(thread_randwqfd,"Offset in Kbytes   Latency in microseconds\n");
	}
	starttime1 = time_so_far();
	child_stat->start_time = starttime1;
	if(cpuutilflag)
	{
		walltime = starttime1;
		cputime = cputime_so_far();
	}
	for(i=0; i<numrecs64; i++){
		if(compute_flag)
			compute_val+=do_compute(delay);
#ifdef bsd4_2
		current_offset = (off64_t)reclen * (rand()%numrecs64);
#else
#ifdef Windows
                current_offset = (off64_t)reclen * (rand()%numrecs64);
#else
		current_offset = reclen * (lrand48()%numrecs64);
#endif
#endif

		if (!(h_flag || k_flag || mmapflag))
		{
#ifdef _LARGEFILE64_SOURCE
		  if(lseek64( fd, current_offset, SEEK_SET )<0)
		  {
			perror("lseek64");
			exit(158);
		  };
#else
		  if(lseek( fd, (off_t)current_offset, SEEK_SET )<0)
		  {
			perror("lseek");
			exit(159);
		  };
#endif
		}
		if(Q_flag)
		{
#ifdef _LARGEFILE64_SOURCE
			traj_offset=lseek64(fd,(off64_t)0,SEEK_CUR);
#else
			traj_offset=lseek(fd,(off_t)0,SEEK_CUR);
#endif
                        thread_qtime_start=time_so_far();
		}
		if(*stop_flag && !stopped){
			if(include_flush)
			{
				if(mmapflag)
					msync(maddr,(size_t)filebytes64,MS_SYNC);
				else
					fsync(fd);
			}
			child_stat->throughput = 
				(time_so_far() - starttime1)-time_res;
			if(child_stat->throughput < (double).000001) 
			{
				child_stat->throughput = time_res;
				if(rec_prob < reclen)
					rec_prob = reclen;
				res_prob=1;
			}

			if(OPS_flag){
			   /*written_so_far=(written_so_far*1024)/reclen;*/
			   written_so_far=w_traj_ops_completed;
			}
			child_stat->throughput = 
			  (double)written_so_far/child_stat->throughput;
			child_stat->actual = (double)written_so_far;
			if(debug1)
			{
				printf("\n(%ld) Stopped by another\n", (long)xx);
			}
			stopped=1;
		}
		if(purge)
			purgeit(nbuff,reclen);
		if(Q_flag)
		{
			thread_qtime_start=time_so_far();
		}
again:		
		if(mmapflag)
		{
			wmaddr = &maddr[current_offset];
			fill_area((long long*)nbuff,(long long*)wmaddr,(long long)reclen);
			if(!mmapnsflag)
			{
			  if(mmapasflag)
			    msync(wmaddr,(size_t)reclen,MS_ASYNC);
			  if(mmapssflag)
			    msync(wmaddr,(size_t)reclen,MS_SYNC);
			}
		}
		else
		{
		   if(async_flag)
		   {
			     if(no_copy_flag)
			     {
				free_addr=nbuff=(char *)malloc((size_t)reclen+page_size);
				nbuff=(char *)(((long)nbuff+(long)page_size) & (long)(~page_size-1));
				if(verify)
					fill_buffer(nbuff,reclen,(long long)pattern,sverify);
			        async_write_no_copy(gc, (long long)fd, nbuff, reclen, (current_offset), depth,free_addr);
			     }
			     else
				async_write(gc, (long long)fd, nbuff, reclen, (current_offset), depth);
		   }
		   else
		   {
		      wval = write(fd, nbuff, (size_t) reclen);
		      if(wval != reclen)
		      {
			if(*stop_flag && !stopped){
				if(include_flush)
				{
					if(mmapflag)
						msync(maddr,(size_t)filebytes64,MS_SYNC);
					else
						fsync(fd);
				}
				temp_time = time_so_far();
				child_stat->stop_time = temp_time;
				child_stat->throughput = 
					(temp_time - starttime1)-time_res;
				if(child_stat->throughput < (double).000001) 
				{
					child_stat->throughput= time_res;
					if(rec_prob < reclen)
						rec_prob = reclen;
					res_prob=1;
				}

				if(OPS_flag){
				   /*written_so_far=(written_so_far*1024)/reclen;*/
				   written_so_far=w_traj_ops_completed;
				}
				child_stat->throughput = 
				  (double)written_so_far/child_stat->throughput;
				child_stat->actual = (double)written_so_far;
				if(debug1)
				{
					printf("\n(%ld) Stopped by another\n", (long)xx);
				}
				stopped=1;
				goto again;
			}
			/* Note: Writer must finish even though told
			   to stop. Otherwise the readers will fail.
			   The code will capture bytes transfered
			   before told to stop but let the writer
			   complete.
			*/
#ifdef NO_PRINT_LLD
		    	printf("\nError writing block %ld, fd= %d\n", i,
				 fd);
#else
		    	printf("\nError writing block %lld, fd= %d\n", i,
				 fd);
#endif
			if(wval==-1)
				perror("write");
			if (!no_unlink)
				unlink(dummyfile[xx]);
			child_stat->flag = CHILD_STATE_HOLD;
		    	exit(127);
		      }
		    }
		}
		if(Q_flag)
		{
			thread_qtime_stop=time_so_far();
#ifdef NO_PRINT_LLD
			fprintf(thread_randwqfd,"%10.1ld %10.0f\n",(traj_offset)/1024,((thread_qtime_stop-thread_qtime_start-time_res))*1000000);
#else
			fprintf(thread_randwqfd,"%10.1lld %10.0f\n",(traj_offset)/1024,((thread_qtime_stop-thread_qtime_start-time_res))*1000000);
#endif
		}
		w_traj_ops_completed++;
		w_traj_bytes_completed+=reclen;
		written_so_far+=reclen/1024;
		if(*stop_flag)
		{
			written_so_far-=reclen/1024;
			w_traj_bytes_completed-=reclen;
		}
	}
	

	if(file_lock)
		if(mylockf((int) fd, (int) 0, (int)0))
			printf("Write unlock failed. %d\n",errno);
	
#ifdef ASYNC_IO
	if(async_flag)
	{
		end_async(gc);
		gc=0;
	}
#endif
	if(!xflag)
	{
		*stop_flag=1;
		if(distributed && client_iozone)
			send_stop();
	}
	
	if(include_flush)
	{
		if(mmapflag)
			msync(maddr,(size_t)filebytes64,MS_SYNC);
		else
			fsync(fd);
	}
	if(include_close)
	{
		if(mmapflag)
			mmap_end(maddr,(unsigned long long)filebytes64);
		close(fd);
	}
	if(!stopped){
		temp_time = time_so_far();
		child_stat->throughput = ((temp_time - starttime1)-time_res)
			-compute_val;
		if(child_stat->throughput < (double).000001) 
		{
			child_stat->throughput= time_res;
			if(rec_prob < reclen)
				rec_prob = reclen;
			res_prob=1;
		}

		if(OPS_flag){
		   /*written_so_far=(written_so_far*1024)/reclen;*/
		   written_so_far=w_traj_ops_completed;
		}
		child_stat->throughput =
			(double)written_so_far/child_stat->throughput;
		child_stat->actual = (double)written_so_far;
		child_stat->stop_time = temp_time;
	}
	child_stat->flag = CHILD_STATE_HOLD; /* Tell parent I'm done */
        if(cdebug)
                fprintf(newstdout,"Child: throughput %f actual %f \n",child_stat->throughput,
                        child_stat->actual);
        if(distributed && client_iozone)
                tell_master_stats(THREAD_RANDOM_WRITE_TEST, chid, child_stat->throughput,
                        child_stat->actual, child_stat->stop_time,
                        child_stat->start_time,child_stat->fini_time,(char)*stop_flag,
                        (long long)CHILD_STATE_HOLD);
	if(cpuutilflag)
	{
		child_stat->fini_time = time_so_far();
		cputime = cputime_so_far() - cputime;
		if (cputime < cputime_res)
			cputime = 0.0;
		child_stat->cputime = cputime;
		walltime = time_so_far() - walltime;
		if (walltime < cputime_res)
			walltime = 0.0;
		child_stat->walltime = walltime;
	}
	stopped=0;
	/*******************************************************************/
	/* End random write performance test. ******************************/
	/*******************************************************************/
	if(debug1)
#ifdef NO_PRINT_LLD
		printf("\nChild finished %ld\n",xx);
#else
		printf("\nChild finished %lld\n",xx);
#endif
	if(!include_close)
	{
		if(mmapflag)
		{
			msync(maddr,(size_t)numrecs64*reclen,MS_SYNC); /*Clean up before read starts running*/ 
			mmap_end(maddr,(unsigned long long)numrecs64*reclen);
		}else
			fsync(fd);
			
		close(fd);
	}
	if(Q_flag && (thread_randwqfd !=0) )
		fclose(thread_randwqfd);
	free(dummyfile[xx]);
#ifdef NO_THREADS
	exit(0);
#else
	if(use_thread)
		thread_exit();
	else
		exit(0);
#endif
return(0);
}

/************************************************************************/
/* Thread cleanup test				        		*/
/* This is not a measurement. It is a mechanism to cleanup all of the   */
/* temporary files that were being used. This becomes very important    */
/* when testing multiple clients over a network :-)                     */
/************************************************************************/
#ifdef HAVE_ANSIC_C
void *
thread_cleanup_test(void *x)
#else
void *
thread_cleanup_test(x)
#endif
{
	long long xx;
	struct child_stats *child_stat;
	off64_t i;
	char *dummyfile[MAXSTREAMS];           /* name of dummy file     */

	
#ifdef NO_THREADS
	xx=chid;
#else
	if(use_thread)
		xx = (long long)((long)x);
	else
	{
		xx=chid;
	}
#endif
	dummyfile[xx]=(char *)malloc((size_t)MAXNAMESIZE);
#ifdef NO_PRINT_LLD
	sprintf(dummyfile[xx],"%s.DUMMY.%ld",filearray[xx],xx);
#else
	sprintf(dummyfile[xx],"%s.DUMMY.%lld",filearray[xx],xx);
#endif
	unlink(dummyfile[xx]);

	child_stat = (struct child_stats *)&shmaddr[xx];
	/*****************/
	/* Children only */
	/*****************/
	child_stat=(struct child_stats *)&shmaddr[xx];
	child_stat->flag = CHILD_STATE_READY;
	if(distributed && client_iozone)
        {
		tell_master_ready(chid);
                wait_for_master_go(chid);
        }
        else
        {
                while(child_stat->flag!=CHILD_STATE_BEGIN)   /* Wait for signal from parent */
                        Poll((long long)1);
        }

	*stop_flag=1;
	if(distributed && client_iozone)
		send_stop();
        if(distributed && client_iozone)
                tell_master_stats(THREAD_CLEANUP_TEST, chid, child_stat->throughput,
                        child_stat->actual, child_stat->stop_time,
                        child_stat->start_time,child_stat->fini_time,(char)*stop_flag,
                        (long long)CHILD_STATE_HOLD);
	child_stat->flag = CHILD_STATE_HOLD; 	/* Tell parent I'm done */
	free(dummyfile[xx]);
#ifdef NO_THREADS
	exit(0);
#else
	if(use_thread)
		thread_exit();
	else
		exit(0);
#endif
return(0);
}


/************************************************************************/
/* mythread_create() Internal routine that calls pthread_create()	*/
/************************************************************************/
#ifndef NO_THREADS
#ifdef HAVE_ANSIC_C
long long 
mythread_create( void *(*func)(void *),void *x)
#else
long long 
mythread_create( func,x)
#endif
{
	pthread_t ts;
	pthread_attr_t attr;
	int xx;
	int *yy;
#ifdef _64BIT_ARCH_
	long long meme;
	meme = (long long)x;
#else
	long meme;
	meme = (long)x;
#endif
	yy=(int *)x;


#ifdef OSFV3
	
	xx=(int )pthread_create(&ts, pthread_attr_default,
		func, (void *)yy);

#else
	pthread_attr_init(&attr);
	xx=(int )pthread_create((pthread_t *)&ts, (pthread_attr_t *) &attr,
		func, (void *)yy);
#endif
	bcopy(&ts,&p_childids[meme],sizeof(pthread_t));
	if(xx < (int)0)
		printf("Thread create failed. Returned %d Errno = %d\n",xx,errno);
	if(debug1 )
	{
		printf("\nthread created has an id of %d\n",ts);
		printf("meme %d\n",meme);
	}
	return((long long)meme);
}
#else
long long 
mythread_create( func,x)
{
	printf("This version does not support threads\n");
}
#endif

/************************************************************************/
/* thread_exit() Internal routine that calls pthread_exit()		*/
/************************************************************************/
#ifndef NO_THREADS
#ifdef HAVE_ANSIC_C
thread_exit(void)
#else
thread_exit()
#endif
{
	pthread_exit((void *)NULL);
return(0);
}
#else
thread_exit()
{
	printf("This version does not support threads\n");
}
#endif

/************************************************************************/
/* mythread_self() Internal function that calls pthread_self()		*/
/************************************************************************/
#ifndef NO_THREADS
pthread_t
mythread_self()
{
	pthread_t xx;
	xx = pthread_self();
	return(xx);
}
#else
mythread_self()
{
	printf("This version does not support threads\n");
}
#endif

/************************************************************************/
/* Internal thread_join routine... calls pthread_join			*/
/************************************************************************/
#ifndef NO_THREADS
#ifdef HAVE_ANSIC_C
void *
thread_join( long long tid, void *status) 
#else
void *
thread_join( tid, status) 
long long tid;
void *status;
#endif
{
	int xx;
	pthread_t eek;
	pthread_attr_t foo;

	bcopy(&p_childids[tid],&eek,sizeof(pthread_t));
	xx=pthread_join(eek,(void **)&foo);
	if(xx<0)
		printf("Thread join returned error %d\n",errno);
	return(0);
}
#else
void *
thread_join( tid, status) 
long long tid;
void *status;
{
	printf("This version does not support threads\n");
}
#endif


/************************************************************************/
/* Dump the CPU utilization data.					*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void
dump_throughput_cpu(void)
#else
void
dump_throughput_cpu()
#endif
{
	long long x,y,i,j;
	char *port;
	char *label;
	char print_str[300];
	x=max_x;
	y=max_y;

	port = use_thread ? "threads" : "processes";
	printf("\n\"CPU utilization report Y-axis is type of test X-axis is number of %s\"\n",port);
	if (bif_flag)
	{
		sprintf(print_str, "CPU utilization report Y-axis is type of test X-axis is number of %s", port);
		do_label(bif_fd, print_str, bif_row++, bif_column);
	}
	label = OPS_flag ?  "ops/sec" :
		MS_flag ? "microseconds/op" : "Kbytes/sec";
#ifdef NO_PRINT_LLD
	if(!silent) printf("\"Record size = %ld Kbytes \"\n", reclen/1024);
#else
	if(!silent) printf("\"Record size = %lld Kbytes \"\n", reclen/1024);
#endif
	if(!silent) printf("\"Output is in CPU%%\"\n\n");
	if (bif_flag)
	{
#ifdef NO_PRINT_LLD
		sprintf(print_str, "Record size = %ld Kbytes", reclen/1024);
#else
		sprintf(print_str, "Record size = %lld Kbytes", reclen/1024);
#endif
		do_label(bif_fd, print_str, bif_row++, bif_column);
		sprintf(print_str, "Output is in CPU%%");
		do_label(bif_fd, print_str, bif_row++, bif_column);
	}
	for (i = 0; i < x; i++)
	{
		if(!silent) printf("\"%15s \"", throughput_tests[i]);
		if (bif_flag)
		{
			sprintf(print_str, "%15s ", throughput_tests[i]);
			do_label(bif_fd, print_str, bif_row, bif_column++);
			bif_column++;
		}
		for (j = 0; j <= y; j++)
		{
			if (bif_flag)
				do_float(bif_fd, runtimes[i][j].cpuutil, bif_row, bif_column++);
			if(!silent) printf("%10.2f ", runtimes[i][j].cpuutil);
		}
		if(!silent) printf("\n\n");
		if (bif_flag)
		{
			bif_column=0;
			bif_row++;
		}
	}
}


/************************************************************************/
/* Dump the throughput graphs 						*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void
dump_throughput(void)
#else
void
dump_throughput()
#endif
{
	long long x,y,i,j;
	char *port;
	char *label;
	char print_str[300];
	x=max_x;
	y=max_y;

	if(use_thread)
		port="threads";
	else
		port="processes";
	if(!silent) printf("\n\"Throughput report Y-axis is type of test X-axis is number of %s\"\n",port);
	if(bif_flag)
	{
		bif_fd=create_xls(bif_filename);
		do_label(bif_fd,command_line,bif_row++,bif_column);
		sprintf(print_str,"Throughput report Y-axis is type of test X-axis is number of %s",port);
		do_label(bif_fd,print_str,bif_row++,bif_column);
	}
	if(OPS_flag)
		label="ops/sec";
	else
	if(MS_flag)
		label="microseconds/op";
	else
		label="Kbytes/sec";
#ifdef NO_PRINT_LLD
	if(!silent) printf("\"Record size = %ld Kbytes \"\n",reclen/1024);
#else
	if(!silent) printf("\"Record size = %lld Kbytes \"\n",reclen/1024);
#endif
	if(!silent) printf("\"Output is in %s\"\n\n",label);
	if(bif_flag)
	{
#ifdef NO_PRINT_LLD
		sprintf(print_str,"Record size = %ld Kbytes",reclen/1024);
#else
		sprintf(print_str,"Record size = %lld Kbytes",reclen/1024);
#endif
		do_label(bif_fd,print_str,bif_row++,bif_column);
		sprintf(print_str,"Output is in %s",label);
		do_label(bif_fd,print_str,bif_row++,bif_column);
	}
	for(i=0;i<=toutputindex;i++)
	{
		   if(!silent) printf("\"%15s \"",toutput[i]);
		   if(bif_flag)
		   {
			   sprintf(print_str,"%15s ",toutput[i]);
			   do_label(bif_fd,print_str,bif_row,bif_column++);
			   bif_column++;
		   }
		   for(j=0;j<=y;j++)
		   {
			   if(bif_flag)
			   {
				   do_float(bif_fd,(double)report_darray[i][j],bif_row,bif_column++);
			   }
			   if(!silent) printf("%10.2f ",report_darray[i][j]);
		   }
		   if(!silent) printf("\n\n");
		   if(bif_flag)
		   {
			   bif_column=0;
			   bif_row++;
		   }
	}
	if (cpuutilflag)
		dump_throughput_cpu();
	if(bif_flag)
		close_xls(bif_fd);
}

/************************************************************************/
/* store_dvalue()							*/
/* Stores a value in an in memory array. Used by the report function    */
/* to re-organize the output for Excel					*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void 
store_dvalue(double value)
#else
void
store_dvalue(value)
double value;
#endif
{
	report_darray[current_x][current_y]=value;
	current_x++;
	if(current_x > max_x)
		max_x=current_x;
	if(current_y > max_y)
		max_y=current_y;
	if(max_x >= MAX_X)
	{
		printf("\nMAX_X too small\n");
		exit(163);
	}
	if(max_y >= MAXSTREAMS)
	{
		printf("\nMAXSTREAMS too small\n");
		exit(164);
	}
}

/************************************************************************/
/* Initialize a file that will be used by mmap.				*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
char *
initfile(int fd, off64_t filebytes,int flag,int prot)
#else
char *
initfile(fd, filebytes,flag, prot)
int fd;
off64_t filebytes;
int flag, prot;
#endif
{
	 char *pa;
	 int mflags=0;
	 size_t tfilebytes;
	 long long x;
	 if(flag)
	 {

#ifdef _LARGEFILE64_SOURCE 
	 	lseek64(fd,filebytes-1,SEEK_SET);
#else
	 	lseek(fd,(off_t)filebytes-1,SEEK_SET);
#endif
		x=write(fd,"b",1);
		if(x < 1)
		{
			printf("Unable to write file\n");
		}
#ifdef _LARGEFILE64_SOURCE 
	 	lseek64(fd,0,SEEK_SET);
#else
	 	lseek(fd,0,SEEK_SET);
#endif
	 }

#ifdef IRIX64
	if((prot & PROT_WRITE)==PROT_WRITE)
		mflags=MAP_SHARED;
	else
		mflags=MAP_PRIVATE;
#else
#ifdef IRIX
	if((prot & PROT_WRITE)==PROT_WRITE)
		mflags=MAP_SHARED;
	else
		mflags=MAP_PRIVATE;
#else
	if((prot & PROT_WRITE)==PROT_WRITE)
		mflags=MAP_FILE|MAP_SHARED;
	else
		mflags=MAP_FILE|MAP_PRIVATE;
#endif
#endif

#ifdef bsd4_2
#ifdef _LARGEFILE64_SOURCE 
	 pa = (char *)mmap64( 0,&filebytes, (int)prot, 
	 		(int)mflags, (int)fd, 0);
#else
	 tfilebytes=(size_t)filebytes;
	 pa = (char *)mmap( 0,&tfilebytes, (int)prot, 
	 		(int)mflags, (int)fd, 0);
#endif
#else
#ifdef _LARGEFILE64_SOURCE 
	 pa = (char *)mmap64( (char *)0,(off64_t)filebytes, (int)prot, 
	 		(int)mflags, (int)fd, (off64_t)0);
#else
	 tfilebytes=(size_t)filebytes;
	 pa = (char *)mmap( (int)0,tfilebytes, (int)prot, 
	 		(int)mflags, (int)fd, (off_t)0);
#endif
#endif
#ifdef __convex_spp
	if(pa == (char *)-1)
	{
		printf("\nMmap failed, errno %d Flags %x\n",errno,(int)mflags);
		exit(165);
	}
#else
#ifdef linux
	if(pa == (char *)-1)
	{
		printf("Mapping failed, errno %d\n",errno);
		exit(166);
	}
#else
#ifdef bsd4_2
	if(pa == (char *)-1)
	{
		printf("Mapping failed, errno %d\n",errno);
		exit(167);
	}
#else
	if(pa == (char *)MAP_FAILED)
	{
		printf("\nMapping failed, errno %d Flags = %x\n",errno,mflags);
		exit(168);
	}
#endif
#endif
#endif
	return(pa);

}


/************************************************************************/
/* Release the mmap area.						*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void
mmap_end( char *buffer, long long size)
#else
void
mmap_end( buffer, size)
char *buffer;
long long size;
#endif
{
	if(munmap(buffer,(size_t)size)<0)
#ifdef NO_PRINT_LLD
		printf("munmap buffer %llx, size %ld failed.\n",buffer,size);
#else
		printf("munmap buffer %llx, size %lld failed.\n",buffer,size);
#endif
	
}

/************************************************************************/
/*	This is an interesting function. How much data to 		*/
/*	copy is a very good question. Here we are using mmap to		*/
/*	perform I/O. If the benchmark touches every byte then		*/
/*	this will include a bcopy of the mmap area into the		*/
/*	users buffer. This is represenative of an application		*/
/*	that reads and touches every byte that it read. If		*/
/*	the benchmark reduces the work to touching only			*/
/*	a long per page then the numbers go up but it			*/
/*	does not reflect the application to well. For now		*/
/*	the best assumption is to believe that the application		*/
/*	will indeed touch every byte.					*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void 
fill_area(long long *src_buffer, long long *dest_buffer, long long length)
#else
fill_area( src_buffer, dest_buffer, length)
long long *src_buffer;
long long *dest_buffer;
long long length;
#endif
{
	bcopy((void *)src_buffer,(void *)dest_buffer,(size_t)length);
}

#ifndef ASYNC_IO
async_read()
{
	printf("Your system does not support async I/O\n");
	exit(169);
}
async_write_no_copy()
{
	printf("Your system does not support async I/O\n");
	exit(170);
}
async_write()
{
	printf("Your system does not support async I/O\n");
	exit(171);
}
async_read_no_copy()
{
	printf("Your system does not support async I/O\n");
	exit(172);
}
async_release()
{
	printf("Your system does not support async I/O\n");
	exit(173);
}
#endif

/************************************************************************/
/* Nap in microseconds.							*/
/************************************************************************/
my_nap( ntime )
int ntime;
{
	struct timeval nap_time;
	int seconds, microsecs;
	seconds = ntime/1000; /* Now in seconds */
	microsecs = (ntime*1000)%1000000;  /* Remaining microsecs */
	nap_time.tv_sec=seconds;
        nap_time.tv_usec=microsecs;
        select(0,0,0,0,&nap_time);
}

/************************************************************************/
/* Function that establishes the resolution 				*/
/* of the gettimeofday() function.					*/
/************************************************************************/

#ifdef HAVE_ANSIC_C
void
get_resolution(void)
#else
void
get_resolution()
#endif
{
	double starttime, finishtime;
	long  j;

again:
	finishtime=time_so_far1(); /* Warm up the instruction cache */
	starttime=time_so_far1();  /* Warm up the instruction cache */
	delay=j=0;		   /* Warm up the data cache */
	while(1)
	{
		starttime=time_so_far1();
		for(j=0;j< delay;j++)
			;
		finishtime=time_so_far1();
		if(starttime==finishtime)
			
			delay++;
		else
			break;
	}
	time_res = (finishtime-starttime)/1000000.0;	
}
/************************************************************************/
/* Function that establishes the resolution 				*/
/* of the getrusage() function.						*/
/************************************************************************/

#ifdef HAVE_ANSIC_C
void
get_rusage_resolution(void)
#else
void
get_rusage_resolution()
#endif
{
	double starttime, finishtime;
	long  j;

again:
	finishtime=cputime_so_far(); /* Warm up the instruction cache */
	starttime=cputime_so_far();  /* Warm up the instruction cache */
	delay=j=0;		   /* Warm up the data cache */
	while(1)
	{
		starttime=cputime_so_far();
		for(j=0;j< delay;j++)
			;
		finishtime=cputime_so_far();
		if(starttime==finishtime)
			
			delay++;
		else
			break;
	}
	cputime_res = (finishtime-starttime);	 /* in seconds */
}
/************************************************************************/
/* Time measurement routines.						*/
/* Return time in microseconds						*/
/************************************************************************/

#ifdef HAVE_ANSIC_C
static double
time_so_far1(void)
#else
static double
time_so_far1()
#endif
{
	/* For Windows the time_of_day() is useless. It increments in 55 milli second   */
	/* increments. By using the Win32api one can get access to the high performance */
	/* measurement interfaces. With this one can get back into the 8 to 9  		*/
	/* microsecond resolution.							*/
#ifdef Windows
	LARGE_INTEGER freq,counter;
	double wintime;
	double bigcounter;

       	QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&counter);
        bigcounter=(double)counter.HighPart *(double)0xffffffff +
                (double)counter.LowPart;
        wintime = (double)(bigcounter/(double)freq.LowPart);
        return((double)wintime*1000000.0);
#else
#if defined (OSFV4) || defined(OSFV3) || defined(OSFV5)
  struct timespec gp;

  if (getclock(TIMEOFDAY, (struct timespec *) &gp) == -1)
    perror("getclock");
  return (( (double) (gp.tv_sec)*1000000.0) +
    ( ((float)(gp.tv_nsec)) * 0.001 ));
#else
  struct timeval tp;

  if (gettimeofday(&tp, (struct timezone *) NULL) == -1)
    perror("gettimeofday");
  return ((double) (tp.tv_sec)*1000000.0) +
    (((double) tp.tv_usec) );
#endif
#endif
}

/************************************************************************/
/* Return the clocks per tick for the times() call.			*/
/************************************************************************/
#ifdef unix
static double
clk_tck()		/* Get the clocks per tick for times */
{
	return((double)sysconf(_SC_CLK_TCK));
}

/************************************************************************/
/* Return the user time in tics as a double.				*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
static double
utime_so_far(void)	 /* Return user time in ticks as double */
#else
static double
utime_so_far()
#endif
{
  	struct tms tp;

  	times(&tp);
  	return ((double) (tp.tms_utime));
}

/************************************************************************/
/* Return the system time in tics as a double.				*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
static double
stime_so_far(void)  	/* Return system time in ticks as double */
#else
static double
stime_so_far()
#endif
{
  	struct tms tp;

  	times(&tp);
  	return ((double) (tp.tms_stime));
}

/************************************************************************/
/* Return the CPU (user + system) time in seconds as a double.		*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
static double
cputime_so_far(void)  	/* Return CPU time in seconds as double */
#else
static double
cputime_so_far()
#endif
{
#if 0
  	struct tms tp;

  	times(&tp);
  	return ((double) (tp.tms_utime + tp.tms_stime) / sc_clk_tck);
#else
	struct rusage ru;

	if (getrusage (RUSAGE_SELF, &ru))
		perror ("getrusage");
	return ((double)(ru.ru_utime.tv_sec + ru.ru_stime.tv_sec) +
		.000001 *(ru.ru_utime.tv_usec + ru.ru_stime.tv_usec));
#endif
}
#endif

/************************************************************************/
/* Return the CPU utilization ((user + system) / walltime) as a percentage. */
/************************************************************************/
#ifdef HAVE_ANSIC_C
static double
cpu_util(double cputime, double walltime)
#else
static double
cpu_util(cputime, walltime)
double cputime, walltime;
#endif
{
	double cpu;

	if (walltime <= (double)0.0)
	{
		cpu = (double)0.0;
		return cpu;
	}
	if (cputime <= (double)0.0)
		cputime = 0.0;
	if (walltime <= (double)0.0)
		cpu = (double)100.0;
	else {
		cpu = (((double)100.0 * cputime) / walltime);
		if (cpu > (double)100.0)
			cpu = (double)99.99;
	}
	return cpu;
}

/************************************************************************/
/* This is a locking function that permits the writes and 		*/
/* reads during the test to hold a file lock. Since each		*/
/* tmp file that Iozone creates is a private file, this seems		*/
/* like a no-op but it turns out that when using Iozone			*/
/* over NFS,  life is very, very different. Some vendors		*/
/* read and write performance goes to zip when locks are held		*/
/* even if there is only one process using the file and having		*/
/* it locked. Some implementations of NFS transition from async		*/
/* to fully sync reads and writes if any locks are used. Euck...	*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
int
mylockf(int fd, int op, int rdwr)
#else
int
mylockf(fd, op, rdwr)
int fd, op, rdwr;
#endif
{
	struct flock myflock;
	int ret;
	if(op==0) /* Generic unlock the whole file */
	{
		myflock.l_type=F_UNLCK;
		myflock.l_whence=SEEK_SET;
		myflock.l_start=0;
		myflock.l_len=0; /* The whole file */
		myflock.l_pid=getpid();
		ret=fcntl(fd,F_SETLKW, &myflock);
	}
	else
		  /* Generic lock the whole file */
	{
		if(rdwr==0)
			myflock.l_type=F_WRLCK; /* Apply write lock */
		else
			myflock.l_type=F_RDLCK; /* Apply read lock */
		myflock.l_whence=SEEK_SET;
		myflock.l_start=0;
		myflock.l_len=0; /* The whole file */
		myflock.l_pid=getpid();
		ret=fcntl(fd,F_SETLKW, &myflock);
	}
	return(ret);
}

/************************************************************************/
/* This function is used to simulate compute time that does		*/
/* not involve the I/O subsystem.					*/
/************************************************************************/

#ifdef HAVE_ANSIC_C
double
do_compute(double delay)
#else
double
do_compute(delay)
double delay;
#endif
{
	double starttime,tmptime;
	if(delay == (double)0)
		return(0);
	starttime=time_so_far();
	while(1)
	{	
		tmptime=time_so_far()-starttime;
		if(tmptime >= delay)
			return(tmptime);
	}
}

/************************************************************************/
/* This function is intended to cause an interruption			*/
/* in the read pattern. It will make a reader have			*/
/* jitter in its access behavior.					*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
void
disrupt(int fd)
#else
disrupt(fd)
int fd;
#endif
{
	char buf[1024];
#ifdef _LARGEFILE64_SOURCE
	off64_t current;
#else
	off_t current;
#endif
	/* Save current position */
#ifdef _LARGEFILE64_SOURCE
	current = lseek64(fd,(off64_t)0,SEEK_CUR);
#else
	current = lseek(fd,0,SEEK_CUR);
#endif

	/* Move to beginning of file */
#ifdef _LARGEFILE64_SOURCE
	lseek64(fd,(off64_t)0,SEEK_SET);
#else
	lseek(fd,0,SEEK_SET);
#endif
	/* Read a little of the file */
	read(fd,buf,1);

	/* Skip to 3k into the file */
#ifdef _LARGEFILE64_SOURCE
	lseek64(fd,(off64_t)3096,SEEK_SET);
#else
	lseek(fd,3096,SEEK_SET);
#endif
	/* Read a little of the file */
	read(fd,buf,1);

	/* Restore current position in file, before disruption */
#ifdef _LARGEFILE64_SOURCE
	lseek64(fd,(off64_t)current,SEEK_SET);
#else
	lseek(fd,current,SEEK_SET);
#endif
	
}

/************************************************************************/
/* Read a telemetry file and return the the offset			*/
/* for the next operaton. Also, set the size				*/
/* in the variable given in the param list.				*/
/* which == 0 ... reader calling					*/
/* which == 1 ... writer calling					*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
long long
get_traj(FILE *traj_fd, long long *traj_size, double *delay, long which)
#else
long long
get_traj(traj_fd, traj_size, delay, which)
FILE *traj_fd;
long long *traj_size;
double *delay;
long which;
#endif
{
	long long traj_offset = 0;
	int tmp = 0;
	int tokens;
	int ret;
	char *ret1,*where;
	char buf[200];
	char sbuf[200];
	int got_line;
	
	got_line=0;

	while(got_line==0)
	{
		tokens=0;
		ret1=fgets(buf,200,traj_fd);
		if(ret1==(char *)0)
		{
			printf("\n\n\tEarly end of telemetry file. Results not accurate.\n");
			signal_handler();
		}
		where=(char *)&buf[0];
		strcpy(sbuf,buf);
		if((*where=='#') || (*where=='\n'))
			continue;
		tokens++;
		strtok(where," ");
		while( (char *)(strtok( (char *)0," ")) != (char *)0)
		{
			tokens++;
		}
		got_line=1;
	}
	if(tokens == 3)
	{
#ifdef NO_PRINT_LLD
		ret=sscanf(sbuf,"%ld %ld %d\n",&traj_offset,traj_size,&tmp);
#else
		ret=sscanf(sbuf,"%lld %lld %d\n",&traj_offset,traj_size,&tmp);
#endif
	/*printf("\nReading %s trajectory with %d items\n",which?"write":"read",tokens);*/
		*delay= ((double)tmp/1000);
	}
	if(tokens == 2)
	{ 
#ifdef NO_PRINT_LLD
		ret=sscanf(sbuf,"%ld %ld\n",&traj_offset,traj_size);
#else
		ret=sscanf(sbuf,"%lld %lld\n",&traj_offset,traj_size);
#endif
		*delay=compute_time;
	/*printf("\nReading %s trajectory with %d items\n",which?"write":"read",tokens);*/
	}
	if((tokens != 2) && (tokens !=3))
	{
		printf("\n\tInvalid entry in telemetry file. > %s <\n",sbuf);
		exit(178);
	}
	if(ret==EOF)
	{
		printf("\n\n\tEarly end of telemetry file. Results not accurate.\n");
		signal_handler();
	}
#ifdef DEBUG
#ifdef NO_PRINT_LLD
	if(!silent) printf("\nOffset %lld  Size %ld Compute delay %f\n",traj_offset, *traj_size,*delay);
#else
	if(!silent) printf("\nOffset %lld  Size %lld Compute delay %f\n",traj_offset, *traj_size,*delay);
#endif
#endif
	return(traj_offset);
}

/************************************************************************/
/* Open the read telemetry file and return file pointer. 		*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
FILE *
open_r_traj(void)
#else
FILE *
open_r_traj()
#endif
{
	FILE *fd;
	fd=fopen(read_traj_filename,"r");
	if(fd == (FILE *)0)
	{
		printf("Unable to open read telemetry file \"%s\"\n",
			read_traj_filename);
		exit(174);
	}
	return(fd);
}

/************************************************************************/
/* Open the write telemetry file and return file pointer.		*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
FILE *
open_w_traj(void)
#else
FILE *
open_w_traj()
#endif
{
	FILE *fd;
	fd=fopen(write_traj_filename,"r");
	if(fd == (FILE *)0)
	{
		printf("Unable to open write telemetry file \"%s\"\n",
			write_traj_filename);
		exit(175);
	}
	return(fd);
}

/************************************************************************/
/* r_traj_size(void)							*/
/* This function scans the read telemetry file 				*/
/* and establishes the number of entries				*/
/* and the maximum file offset.						*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
long long
r_traj_size(void)
#else
long long
r_traj_size()
#endif
{
	FILE *fd;
	int ret;
	long long traj_offset = 0;
	long long traj_size = 0;
	long long max_offset = 0;
	int tokens;
	int dummy;
	int lines;
	char buf[200];
	char sbuf[200];
	char *ret1,*where;

	lines=0;
	fd=fopen(read_traj_filename,"r");
	if(fd == (FILE *)0)
	{
		printf("Unable to open read telemetry file \"%s\"\n",
			read_traj_filename);
		exit(174);
	}
	while(1)
	{
		tokens=0;
		ret1=fgets(buf,200,fd);
		if(ret1==(char *)0)
			break;
		where=(char *)&buf[0];
		strcpy(sbuf,buf);
		lines++;
		if((*where=='#') || (*where=='\n'))
			continue;
		tokens++;
		strtok(where," ");
		while( (char *)(strtok( (char *)0," ")) != (char *)0)
		{
			tokens++;
		}
		if(tokens==1)
		{
			printf("\n\tInvalid read telemetry file entry. Line %d",
				lines);
			signal_handler();
		}
#ifdef DEBUG
	printf("Tokens = %d\n",tokens);
#endif
		if(tokens==3)
		{
#ifdef NO_PRINT_LLD
			ret=sscanf(sbuf,"%ld %ld %d\n",&traj_offset,&traj_size,&dummy);
#else
			ret=sscanf(sbuf,"%lld %lld %d\n",&traj_offset,&traj_size,&dummy);
#endif
		}
		if(tokens==2)
		{
#ifdef NO_PRINT_LLD
			ret=sscanf(sbuf,"%ld %ld\n",&traj_offset,&traj_size);
#else
			ret=sscanf(sbuf,"%lld %lld\n",&traj_offset,&traj_size);
#endif
		}
		if((tokens != 2) && (tokens !=3))
		{
			printf("\n\tInvalid read telemetry file. Line %d\n",lines);
			exit(178);
		}
		if(traj_offset + traj_size > max_offset)
			max_offset=traj_offset + traj_size;
		
		r_traj_ops++;
	}	
	r_traj_fsize=max_offset;
#ifdef DEBUG
	printf("File size of read %lld Item count %lld\n",r_traj_fsize,r_traj_ops);
#endif
	fclose(fd);
}

/************************************************************************/
/* w_traj_size(void)							*/
/* This function scans the write telemetry file 			*/
/* and establishes the number of entries				*/
/* and the maximum file offset.						*/
/************************************************************************/
#ifdef HAVE_ANSIC_C
long long
w_traj_size(void)
#else
long long
w_traj_size()
#endif
{
	FILE *fd;
	int ret;
	long long traj_offset = 0;
	long long traj_size = 0;
	long long max_offset = 0;
	int dummy;
	int tokens,lines;
	char *ret1;
	char buf[200];
	char sbuf[200];
	char *where;

	lines=0;

	fd=fopen(write_traj_filename,"r");
	if(fd == (FILE *)0)
	{
		printf("Unable to open write telemetry file \"%s\"\n",
			write_traj_filename);
		exit(174);
	}
	while(1)
	{
		tokens=0;
		ret1=fgets(buf,200,fd);
		if(ret1==(char *)0)
			break;
		lines++;
		where=(char *)&buf[0];
		strcpy(sbuf,buf);
		if((*where=='#') || (*where=='\n'))
			continue;
		tokens++;
		strtok(where," ");
		while( (char *)(strtok( (char *)0," ")) != (char *)0)
		{
			tokens++;
		}
		if(tokens==1)
		{
			printf("\n\tInvalid write telemetry file entry. Line %d\n",
				lines);
			signal_handler();
		}
		if(tokens==3)
		{
#ifdef NO_PRINT_LLD
			ret=sscanf(sbuf,"%ld %ld %d\n",&traj_offset,&traj_size,&dummy);
#else
			ret=sscanf(sbuf,"%lld %lld %d",&traj_offset,&traj_size,&dummy);
#endif
		}
		if(tokens==2)
		{
#ifdef NO_PRINT_LLD
			ret=sscanf(sbuf,"%ld %ld\n",&traj_offset,&traj_size);
#else
			ret=sscanf(sbuf,"%lld %lld\n",&traj_offset,&traj_size);
#endif
		}
		if(tokens > 3)
		{
			printf("\n\tInvalid write telemetry file entry. Line %d\n",
				lines);
			exit(174);
		}
		if(traj_offset + traj_size > max_offset)
			max_offset=traj_offset + traj_size;
		
		w_traj_ops++;
	}	
	w_traj_fsize=max_offset;
#ifdef DEBUG
	printf("File size of write %lld Item count %lld\n",w_traj_fsize,w_traj_ops);
#endif
	fclose(fd);
	return(max_offset);
}

/************************************************************************/
/* Find which version of the telemetry file format is in use.		*/
/************************************************************************/

#ifdef HAVE_ANSIC_C
void
traj_vers(void)
#else
void
traj_vers()
#endif
{
	FILE *fd;
	char *where;
	char buf[200];
	int things;
	char *ret1;
	
	if(r_traj_flag)
	{
		things=0;
		fd=fopen(read_traj_filename,"r");
		if(fd == (FILE *)0)
		{	
			printf("Unable to open read telemetry file \"%s\"\n", read_traj_filename);
			exit(174);
		}
loop1:
		ret1=fgets(buf,200,fd);
		if(ret1==(char *)0)
		{
			fclose(fd);
			return;
		}
		where=(char *)&buf[0];
		if((*where=='#') || (*where=='\n'))
			goto loop1;
		things++;
		strtok(where," ");
		while( (char *)(strtok( (char *)0," ")) != (char *)0)
		{
			things++;
		}
		r_traj_items=things;
#ifdef DEBUG
		printf("Found %d items in the read telemetry file\n",things);
#endif
	}
	if(w_traj_flag)
	{
		things=0;
		fd=fopen(write_traj_filename,"r");
		if(fd == (FILE *)0)
		{	
			printf("Unable to open write telemetry file \"%s\"\n", write_traj_filename);
			exit(174);
		}
loop2:
		ret1=fgets(buf,200,fd);
		if(ret1==(char *)0)
		{
			fclose(fd);
			return;
		}
		where=(char *)&buf[0];
		if((*where=='#') || (*where=='\n'))
			goto loop2;
		things++;
		strtok(where," ");
		while( (char *)(strtok( (char *)0," ")) != (char *)0)
		{
			things++;
		}
		fclose(fd);
		w_traj_items=things;
#ifdef DEBUG
		printf("Found %d items in the write telemetry file\n",things);
#endif
	}
}

/********************************************************************/
/*							            */
/* Today this initializes the default set of file sizes for Iozone. */
/* in the future it may take input from the command line or	    */
/* from a file.							    */
/*								    */
/********************************************************************/
#ifdef HAVE_ANSIC_C
void
init_file_sizes( off64_t min_f_size,  off64_t max_f_size)
#else
void
init_file_sizes(min_f_size, max_f_size)
off64_t min_f_size;
off64_t max_f_size;
#endif
{
    	off64_t kilosi;
        for(kilosi=min_f_size;kilosi<=max_f_size;kilosi*=MULTIPLIER)
	{
		add_file_size((off64_t)kilosi);
	}
}

/********************************************************************/
/* Used to constuct the list of file sizes to test.		    */
/********************************************************************/
#ifdef HAVE_ANSIC_C
void
add_file_size(off64_t size)
#else
void
add_file_size(size)
off64_t size;
#endif
{
	struct size_entry *size_listp;
	struct size_entry *nsize_list;
	
	size_listp=size_list;
	
	if(size_list)
	{
		if(size_listp->next)
			while(size_listp->next!=0)
				size_listp=size_listp->next;
	}
	nsize_list=(struct size_entry *)malloc(sizeof(struct size_entry));
	if(nsize_list==0)
	{
		printf("Malloc failed in add_file_size\n");
		exit(180);
	}	
	nsize_list->next=0;
	nsize_list->size=size;
	if(size_list == 0)
		size_list=nsize_list;
	else
		size_listp->next=nsize_list;
	size_listp=size_list;
}

/********************************************************************/
/* Return the next file size to test.				    */
/********************************************************************/
#ifdef HAVE_ANSIC_C
off64_t
get_next_file_size(off64_t size)
#else
off64_t
get_next_file_size(size)
off64_t size;
#endif
{
	struct size_entry *size_listp;
	struct size_entry *nsize_list;
	
	size_listp=size_list;
	
	for( ; size_listp ; size_listp=size_listp->next )
	{
		if(size_listp->size > size)
			return(size_listp->size);
	}
	return((off64_t)0);
}


/**********************************************************************/
/*							              */
/* Today this initializes the default set of record sizes for Iozone. */
/* in the future it may take input from the command line or	      */
/* from a file.							      */
/*								      */
/**********************************************************************/
#ifdef HAVE_ANSIC_C
void
init_record_sizes( off64_t min_r_size,  off64_t max_r_size)
#else
void
init_record_sizes(min_r_size, max_r_size)
off64_t min_r_size;
off64_t max_r_size;
#endif
{
    	off64_t size;
        for(size=min_r_size;size<=max_r_size;size*=MULTIPLIER)
	{
		add_record_size((off64_t)size);
	}
}

#ifdef HAVE_ANSIC_C
void
del_record_sizes(void)
#else
void
del_record_sizes()
#endif
{
        struct size_entry *size_listp;
        struct size_entry *save_item;

        size_listp=rec_size_list;
        if(rec_size_list)
        {
                while(size_listp!=0)
                {
                        save_item=size_listp->next;
                        free(size_listp);
                        size_listp=save_item;
                }
        }
        rec_size_list=0;
}

/********************************************************************/
/* Used to constuct the list of record sizes to test.		    */
/********************************************************************/
#ifdef HAVE_ANSIC_C
void
add_record_size(off64_t size)
#else
void
add_record_size(size)
off64_t size;
#endif
{
	struct size_entry *size_listp;
	struct size_entry *nsize_list;
	
	size_listp=rec_size_list;
	
	if(rec_size_list)
	{
		if(size_listp->next)
			while(size_listp->next!=0)
				size_listp=size_listp->next;
	}
	nsize_list=(struct size_entry *)malloc(sizeof(struct size_entry));
	if(nsize_list==0)
	{
		printf("Malloc failed in add_file_size\n");
		exit(180);
	}	
	nsize_list->next=0;
	nsize_list->size=size;
	if(rec_size_list == 0)
		rec_size_list=nsize_list;
	else
		size_listp->next=nsize_list;
	size_listp=rec_size_list;
}

/********************************************************************/
/* Return the next record size to test.				    */
/********************************************************************/
#ifdef HAVE_ANSIC_C
off64_t
get_next_record_size(off64_t size)
#else
off64_t
get_next_record_size(size)
off64_t size;
#endif
{
	struct size_entry *size_listp;
	struct size_entry *nsize_list;
	
	size_listp=rec_size_list;
	
	for( ; size_listp ; size_listp=size_listp->next )
	{
		if(size_listp->size > size)
			return(size_listp->size);
	}
	return((off64_t)0);
}


/*
 * Socket based communication mechanism.
 * It's intended use is to be the communication mechanism
 * that will be used to get Iozone to run across
 * multiple clients. 1/11/2002  Don Capps
 * The communication model permits a master to send and receive
 * messages to and from clients, and for clients to be able to 
 * send and receive messages to and from the master.
 */
/* 
 * Interfaces are:
	Master:
	  int start_master_listen(void)
		Called to create masters listening port.

	  void master_listen(int sock, int size_of_message)
		Call when master wants to block and read
		a message.

	  int start_master_send(char *child_host_name, int port)
		Call to start a send channel to a client.

	  void master_send(int child_socket_val, char *host_name, 
		    char *send_buffer, int send_size)
		Call to send message to a client.

	  void stop_master_listen(int master_socket_val)
		Call to release the masters listening port.

	  void stop_master_send(int child_socket_val)
		Call to release the masters send port to a client.

	Clients:
	  int start_child_listen(int size_of_message)
		Called to create clients listening port.

	  void child_listen(int sock, int size_of_message)
		Call when client wants to block and read
		a message from the master.

	  int start_child_send(char *controlling_host_name)
		Call to start a send channel to the master.

	  void child_send(int child_socket_val, char *controlling_host_name, 
		   char *send_buffer, int send_size)
		Call to send message to the master.

	  void stop_child_listen(int child_socket_val)
		Call to release the clients listening port.

	  void stop_child_send(int child_socket_val)
		Call to release the clients send port to the master.


	Messages are sent in command blocks. The structure is
	client_command for messages from the master to the 
	client, and master_command for messages sent from
	a client to the master.
*/


/*
 * Allocate the master listening port that 
 * all children will use to send messages to the master.
 */
#ifdef HAVE_ANSIC_C
int
start_master_listen(void)
#else
int
start_master_listen()
#endif
{
	int tsize;
	int rcvd;
	int s;
	int rc;
	struct sockaddr_in addr, raddr;

        s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s < 0)
        {
                perror("socket failed:");
                exit(19);
        }
        bzero(&addr, sizeof(struct sockaddr_in));
        addr.sin_port = HOST_LIST_PORT;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        rc = -1;
        while (rc < 0)
        {
                rc = bind(s, (struct sockaddr *)&addr,
                                sizeof(struct sockaddr));
		if(rc < 0)
		{
                	addr.sin_port++;
			continue;
		}
		master_listen_port = addr.sin_port;
        }
	if(rc < 0)
	{
		perror("bind failed\n");
		exit(20);
	}
	return(s);
}

/*
 * Master listens for messages and blocks until
 * something arrives.
 */
#ifdef HAVE_ANSIC_C
void
master_listen(int sock, int size_of_message)
#else
void
master_listen(sock, size_of_message)
int sock, size_of_message;
#endif
{
	int tsize;
	int rcvd;
	int s;
	int rc;
	struct sockaddr_in addr, raddr;

	tsize = size_of_message;
	s = sock;
	rcvd = 0;
	while(rcvd < tsize)
	{
		if(mdebug ==1)
		{
			printf("Master: In recieve \n");
			fflush(stdout);
		}
		rc=recv(s,master_rcv_buf,size_of_message,0);
		if(rc < 0)
		{
			perror("Read failed\n");
			exit(21);
		}
		if(mdebug >=1)
		{
			printf("Master got %d bytes\n",rc);
			fflush(stdout);
		}
		rcvd+=rc;
	}
	if(mdebug >=1)
	{
		printf("Master returning from got %d bytes\n",rc);
		fflush(stdout);
	}
}

/*
 * Child sends message to master.
 */

#ifdef HAVE_ANSIC_C
void
child_send(int child_socket_val, char *controlling_host_name, struct master_command *send_buffer, int send_size)
#else
void
child_send(child_socket_val, controlling_host_name, send_buffer, send_size)
int child_socket_val; 
char *controlling_host_name; 
struct master_command *send_buffer; 
int send_size;
#endif
{
	int rc,tsize;
	struct sockaddr_in addr,raddr;
	struct hostent *he;
	int port;
	struct in_addr *ip;

	if(cdebug>=1)
	{
		printf("Child sending message to %s\n",controlling_host_name);
		fflush(newstdout);
	}
        rc = send(child_socket_val, send_buffer, send_size, 0);
        if (rc < 0)
        {
                perror("write failed\n");
                exit(26);
        }
}

/*
 * Master sending message to a child
 * There should be a unique child_socket_val for each
 * child.
 */
#ifdef HAVE_ANSIC_C
void
master_send(int child_socket_val, char *host_name, struct client_command *send_buffer, int send_size)
#else
void
master_send(child_socket_val, host_name, send_buffer, send_size)
void
int child_socket_val;
char *host_name; 
struct client_command *send_buffer; 
int send_size;
#endif
{
	int rc,tsize;
	struct sockaddr_in addr,raddr;
	struct hostent *he;
	int port;
	struct in_addr *ip;

	if(mdebug >= 1)
		printf("Master sending message to %s \n",host_name);
        rc = send(child_socket_val, send_buffer, send_size, 0);
        if (rc < 0)
        {
                perror("write failed\n");
                exit(26);
        }
}

/*
 * Client setting up the channel for sending messages to the master.
 */
#ifdef HAVE_ANSIC_C
int
start_child_send(char *controlling_host_name)
#else
int
start_child_send(controlling_host_name)
char *controlling_host_name;
#endif
{
	int rc,child_socket_val,tsize;
	struct sockaddr_in addr,raddr;
	struct hostent *he;
	int port;
	struct in_addr *ip;

        he = gethostbyname(controlling_host_name);
        if (he == NULL)
        {
		if(cdebug)
		{
                   fprintf(newstdout,"Child: Bad server host %s\n",controlling_host_name);
		   fflush(newstdout);
		}
                exit(22);
        }
	if(cdebug ==1)
	{
	        fprintf(newstdout,"Child: start child send to hostname: %s\n", he->h_name);
		fflush(newstdout);
	}
        ip = (struct in_addr *)he->h_addr_list[0];
	if(cdebug ==1)
	{
        	fprintf(newstdout,"Child: server host: %s\n", (char *)inet_ntoa(ip->s_addr));
		fflush(newstdout);
	}


        raddr.sin_family = AF_INET;
        raddr.sin_port = HOST_LIST_PORT;
        raddr.sin_addr.s_addr = ip->s_addr;
        child_socket_val = socket(AF_INET, SOCK_DGRAM, 0);
        if (child_socket_val < 0)
        {
                perror("Child: socket failed:");
                exit(23);
        }
        bzero(&addr, sizeof(struct sockaddr_in));
        addr.sin_port = CHILD_ESEND_PORT;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        rc = -1;
        while (rc < 0)
        {
                rc = bind(child_socket_val, (struct sockaddr *)&addr,
                                                sizeof(struct sockaddr_in));
		if(rc < 0)
		{
                	addr.sin_port++;
			continue;
		}
        }
	if(cdebug ==1)
	{
		fprintf(newstdout,"Child: Bound to host port %d\n",addr.sin_port);
		fflush(newstdout);
	}
        if (rc < 0)
        {
                perror("Child: bind failed\n");
                exit(24);
        }
        rc = 
		connect(child_socket_val, (struct sockaddr *)&raddr, 
			sizeof(struct sockaddr_in));
	        if (rc < 0)
        {
                perror("Child: connect failed\n");
                exit(25);
        }
	if(cdebug ==1)
	{
		fprintf(newstdout,"Child Connected\n");
		fflush(newstdout);
	}
	return (child_socket_val);
}

/*
 * Close the childs listening port for messages from the master.
 */
#ifdef HAVE_ANSIC_C
void
stop_child_listen(int child_socket_val)
#else
void
stop_child_listen(child_socket_val)
int child_socket_val;
#endif
{
	close(child_socket_val);
}

/*
 * Close the childs channel for sending messages to the master.
 */
#ifdef HAVE_ANSIC_C
void
stop_child_send(int child_socket_val)
#else
void
stop_child_send(child_socket_val)
int child_socket_val;
#endif
{
	close(child_socket_val);
}

/*
 * Close the masters listening channel for all clients messages.
 */
#ifdef HAVE_ANSIC_C
void
stop_master_listen(int master_socket_val)
#else
void
stop_master_listen(master_socket_val)
int master_socket_val;
#endif
{
	close(master_socket_val);
}

/*
 * Close the masters send channel a particular child.
 */
#ifdef HAVE_ANSIC_C
void
stop_master_send(int child_socket_val)
#else
void
stop_master_send(child_socket_val)
int child_socket_val;
#endif
{
	close(child_socket_val);
}

/*
 * Start the childs listening service for messages from the master.
 */
#ifdef HAVE_ANSIC_C
int
start_child_listen(int size_of_message)
#else
int
start_child_listen(size_of_message)
int size_of_message;
#endif
{
	int tsize;
	int rcvd;
	int s;
	int rc;
	int xx;
	struct sockaddr_in addr, raddr;
	xx = 0;
	tsize=size_of_message; /* Number of messages to receive */
        s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s < 0)
        {
                perror("socket failed:");
                exit(19);
        }
        bzero(&addr, sizeof(struct sockaddr_in));
        addr.sin_port = CHILD_LIST_PORT;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        rc = -1;
        while (rc < 0)
        {
                rc = bind(s, (struct sockaddr *)&addr,
                                sizeof(struct sockaddr));
		if(rc < 0)
		{
                	addr.sin_port++;
			continue;
		}
        }
	child_port = addr.sin_port;
	if(cdebug ==1)
	{
		fprintf(newstdout,"Child: Listen: Bound at port %d\n",addr.sin_port);
		fflush(newstdout);
	}
	if(rc < 0)
	{
		perror("bind failed\n");
		exit(20);
	}
	return(s);
}

/*
 * The clients use this to block waiting for a message from
 * the master.
 */
#ifdef HAVE_ANSIC_C
void
child_listen(int sock, int size_of_message)
#else
void
child_listen(sock, size_of_message)
int sock, size_of_message;
#endif
{
	int tsize;
	int rcvd;
	int s;
	int rc;
	struct sockaddr_in addr, raddr;
	s = sock;
	tsize=size_of_message; /* Number of messages to receive */
	rcvd = 0;
	while(rcvd < tsize)
	{
		if(cdebug ==1)
		{
			fprintf(newstdout,"Child In recieve \n");
			fflush(newstdout);
		}
		rc=recv(s,child_rcv_buf,size_of_message,0);
		if(rc < 0)
		{
			perror("Read failed\n");
			exit(21);
		}
		if(cdebug >= 1)
		{
			fprintf(newstdout,"Child: Got %d bytes\n",rc);
			fflush(newstdout);
		}
		rcvd+=rc;
	}
	if(cdebug >= 1)
	{
		fprintf(newstdout,"Child: return from listen\n");
		fflush(newstdout);
	}
}
/*
 * Start the childs async listening service for messages from the master.
 */
#ifdef HAVE_ANSIC_C
int
start_child_listen_async(int size_of_message)
#else
int
start_child_listen_async(size_of_message)
int size_of_message;
#endif
{
	int tsize;
	int rcvd;
	int s;
	int rc;
	int xx;
	struct sockaddr_in addr, raddr;
	xx = 0;
	tsize=size_of_message; /* Number of messages to receive */
        s = socket(AF_INET, SOCK_DGRAM, 0);
        if (s < 0)
        {
                perror("socket failed:");
                exit(19);
        }
        bzero(&addr, sizeof(struct sockaddr_in));
        addr.sin_port = CHILD_ALIST_PORT;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        rc = -1;
        while (rc < 0)
        {
                rc = bind(s, (struct sockaddr *)&addr,
                                sizeof(struct sockaddr));
		if(rc < 0)
		{
                	addr.sin_port++;
			continue;
		}
        }
	child_async_port = addr.sin_port;
	if(cdebug ==1)
	{
		fprintf(newstdout,"Child: Async Listen: Bound at port %d\n",addr.sin_port);
		fflush(newstdout);
	}
	if(rc < 0)
	{
		perror("bind failed\n");
		exit(20);
	}
	return(s);
}
/*
 * The clients use this to block waiting for an async message from
 * the master.
 */
#ifdef HAVE_ANSIC_C
void
child_listen_async(int sock, int size_of_message)
#else
void
child_listen_async(sock, size_of_message)
int sock, size_of_message;
#endif
{
	int tsize;
	int rcvd;
	int s;
	int rc;
	struct sockaddr_in addr, raddr;
	s = sock;
	tsize=size_of_message; /* Number of messages to receive */
	rcvd = 0;
	while(rcvd < tsize)
	{
		if(cdebug ==1)
		{
			fprintf(newstdout,"Child In async recieve \n");
			fflush(newstdout);
		}
		rc=recv(s,child_async_rcv_buf,size_of_message,0);
		if(rc < 0)
		{
			perror("Read failed\n");
			exit(21);
		}
		if(cdebug >= 1)
		{
			fprintf(newstdout,"Child: Got %d bytes (async) \n",rc);
			fflush(newstdout);
		}
		rcvd+=rc;
	}
	if(cdebug >= 1)
	{
		fprintf(newstdout,"Child: return from async listen\n");
		fflush(newstdout);
	}
}

/*
 * Start the channel for the master to send a message to 
 * a particular child on a particular port that the child
 * has created for the parent to use to communicate.
 */
#ifdef HAVE_ANSIC_C
int
start_master_send(char *child_host_name, int child_port, in_addr_t *my_s_addr)
#else
int
start_master_send(child_host_name, child_port, my_s_addr)
char *child_host_name; 
int child_port;
in_addr_t *my_s_addr;
#endif
{
	int rc,master_socket_val,tsize;
	struct sockaddr_in addr,raddr;
	struct hostent *he;
	int port;
	struct in_addr *ip;

        he = gethostbyname(child_host_name);
        if (he == NULL)
        {
                printf("Master: Bad hostname >%s<\n",child_host_name);
		fflush(stdout);
                exit(22);
        }
	if(mdebug ==1)
	{
	        printf("Master: start master send: %s\n", he->h_name);
		fflush(stdout);
	}
        ip = (struct in_addr *)he->h_addr_list[0];
	if(mdebug ==1)
	{
        	printf("Master: child name: %s\n", (char *)inet_ntoa(ip->s_addr));
        	printf("Master: child Port: %d\n", child_port);
		fflush(stdout);
	}

	port=child_port;
	*my_s_addr = ip->s_addr;
	/*port=CHILD_LIST_PORT;*/

        raddr.sin_family = AF_INET;
        raddr.sin_port = port;
        raddr.sin_addr.s_addr = ip->s_addr;
        master_socket_val = socket(AF_INET, SOCK_DGRAM, 0);
        if (master_socket_val < 0)
        {
                perror("Master: socket failed:");
                exit(23);
        }
        bzero(&addr, sizeof(struct sockaddr_in));
        addr.sin_port = HOST_ESEND_PORT;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        rc = -1;
        while (rc < 0)
        {
                rc = bind(master_socket_val, (struct sockaddr *)&addr,
                                                sizeof(struct sockaddr_in));
		if(rc < 0)
		{
                	addr.sin_port++;
			continue;
		}
        }
	if(mdebug ==1)
	{
		printf("Master: Bound port\n");
		fflush(stdout);
	}
        if (rc < 0)
        {
                perror("Master: bind failed\n");
                exit(24);
        }
        rc = connect(master_socket_val, (struct sockaddr *)&raddr, 
			sizeof(struct sockaddr_in));
	if (rc < 0)
        {
                perror("Master: connect failed\n");
                exit(25);
        }
	if(mdebug ==1)
	{
		printf("Master Connected\n");
		fflush(stdout);
	}
	return (master_socket_val);
}
/*
 * Start the channel for the master to send a message to 
 * a particular child on a particular port that the child
 * has created for the parent to use to communicate.
 */
#ifdef HAVE_ANSIC_C
int
start_master_send_async(char *child_host_name, int child_port, in_addr_t my_s_addr)
#else
int
start_master_send_async(child_host_name, child_port, my_s_addr)
char *child_host_name; 
int child_port;
in_addr_t my_s_addr;
#endif
{
	int rc,master_socket_val,tsize;
	struct sockaddr_in addr,raddr;
	int dummy;
	int port;
	struct in_addr *ip;
	int dummy1;
	struct hostent *ho;
	int dummy2;

	port=child_port;

        raddr.sin_family = AF_INET;
        raddr.sin_port = port;
        raddr.sin_addr.s_addr = my_s_addr;
        master_socket_val = socket(AF_INET, SOCK_DGRAM, 0);
        if (master_socket_val < 0)
        {
                perror("Master: async socket failed:");
                exit(23);
        }
        bzero(&addr, sizeof(struct sockaddr_in));
        addr.sin_port = HOST_ESEND_PORT;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        rc = -1;
        while (rc < 0)
        {
                rc = bind(master_socket_val, (struct sockaddr *)&addr,
                                                sizeof(struct sockaddr_in));
		if(rc < 0)
		{
                	addr.sin_port++;
			continue;
		}
        }
	if(mdebug ==1)
	{
		printf("Master: Bound async port\n");
		fflush(stdout);
	}
        if (rc < 0)
        {
                perror("Master: bind async failed\n");
                exit(24);
        }
        rc = connect(master_socket_val, (struct sockaddr *)&raddr, 
			sizeof(struct sockaddr_in));
	if (rc < 0)
        {
                perror("Master: async connect failed\n");
                exit(25);
        }
	if(mdebug ==1)
	{
		printf("Master async Connected\n");
		fflush(stdout);
	}
	return (master_socket_val);
}

/*
 * If not "distributed" then call fork. The "distributed"
 * will start iozone on a remote node.
 */
#ifdef HAVE_ANSIC_C
long long
start_child_proc(int testnum,long long numrecs64, long long reclen)
#else
long long
start_child_proc(testnum, numrecs64, reclen)
int testnum;
long long numrecs64, reclen;
#endif
{
	long long x;
	if(distributed && master_iozone)
	{
		x=(long long)pick_client(testnum,numrecs64, reclen);
	}
	else
	{
		x=(long long)fork();
	}
	if(mdebug)
		printf("Starting proc %d\n",(int)x);	
	return(x);
}	

/*
 * This function picks a client from the list of clients and
 * starts it running on the remote machine. It also waits for
 * the remote process to join and then sends the client
 * the state information it needs to begin to run the 
 * test. The client will initialize its state space, 
 * begin the test and block as the barrier waiting
 * for the master to say go.
 */
#ifdef HAVE_ANSIC_C
int
pick_client(int testnum,long long numrecs64, long long reclen)
#else
int
pick_client(testnum, numrecs64, reclen)
int testnum;
long long numrecs64, reclen;
#endif
{
	int x;
	int c_command,child_index;
	struct client_command cc;
	struct master_command *mc;
	char command[512];
	in_addr_t my_s_addr;

	for(x=0;x<512;x++)
		command[x]=0;

	current_client_number++; /* Need to start with 1 */
	x=current_client_number;

	child_idents[x-1].state = C_STATE_ZERO;
	/* Step 1. Now start client going on remote node.	*/

	sprintf(command,"%s ",REMOTE_SHELL);
	strcat(command,child_idents[x-1].child_name);
	strcat(command," '");
	strcat(command,child_idents[x-1].execute_path);
	strcat(command," -+s -t 1 -r 4 -s 4 -+c ");
	strcat(command,controlling_host_name);
	strcat(command," '");
	system(command);
/*
	system("remsh rsnperf '/home/capps/niozone/iozone -+s -t 1 -r 4 -s 8 -+c rsnperf'");

*/
	if(mdebug)
		printf("%s",command);
	/* Format example: 					*/
	/*							*/
	/* system("remsh rsnperf '/home/capps/niozone/iozone 	*/
	/*       -+s -t 1 -r 4 -s 8 -+c rsnperf'");		*/
	/*							*/

	/* Step 2. Wait for join from new client.		*/

	child_idents[x-1].state = C_STATE_WAIT_WHO;

	if(mdebug>=1)
		printf("Master listening for child to send join message.\n");
	master_listen(master_listen_socket,sizeof(struct master_command));
	mc = (struct master_command *)&master_rcv_buf;
	c_port = mc->m_child_port; 
	a_port = mc->m_child_async_port; 
	c_command = mc->m_command;
	if(mdebug>=1)
	{
		printf("Master back from listen child Joined.\n");
		printf("Master: Command %d\n",c_command);
	}
	/* Step 3. Then start_master_send() for this client.	*/
	
	if(mdebug>=1)
		printf("Starting master send channel\n");
        master_send_sockets[x-1]= start_master_send(child_idents[x-1].child_name,c_port,
		&my_s_addr); 
	if(mdebug>=1)
		printf("Starting master send async channel\n");
        master_send_async_sockets[x-1]= start_master_send_async(child_idents[x-1].child_name,a_port,
		my_s_addr); 

	child_idents[x-1].master_socket_num = master_send_sockets[x-1];
	child_idents[x-1].master_async_socket_num = master_send_async_sockets[x-1];
	child_idents[x-1].child_number = x-1;
	child_idents[x-1].child_port = c_port;
	child_idents[x-1].child_async_port = a_port;

	/* 								*/
	/* Step 4. Send message to client telling him his name, number, */
	/*             rsize, fsize, and test to run.			*/
	strcpy(cc.c_host_name ,controlling_host_name);
	strcpy(cc.c_client_name ,child_idents[x-1].child_name);
	strcpy(cc.c_working_dir ,child_idents[x-1].workdir);
	cc.c_command = R_JOIN_ACK;
	cc.c_client_number = x-1;
	cc.c_testnum = testnum;
	cc.c_numrecs64 = numrecs64;
	cc.c_reclen = reclen;
	cc.c_oflag = oflag;
	cc.c_jflag = jflag;
	cc.c_direct_flag = direct_flag;
	cc.c_async_flag = async_flag;
	cc.c_mmapflag = mmapflag;
	cc.c_fetchon = fetchon;
	cc.c_verify = verify;
	cc.c_file_lock = file_lock;
	cc.c_Q_flag = Q_flag;
	cc.c_xflag = xflag;
	cc.c_include_flush = include_flush;
	cc.c_OPS_flag = OPS_flag;
	cc.c_purge = purge;
	cc.c_mmapnsflag = mmapnsflag;
	cc.c_mmapssflag = mmapssflag;
	cc.c_no_copy_flag = no_copy_flag;
	cc.c_no_unlink = no_unlink;
	cc.c_include_close = include_close;
	cc.c_disrupt_flag = disrupt_flag;
	cc.c_compute_flag = compute_flag;
	cc.c_delay = delay;


	if(mdebug)
		printf("Master sending client who he is\n");
	master_send(master_send_sockets[x-1],cc.c_client_name, &cc,sizeof(struct client_command));

	child_idents[x-1].state = C_STATE_WAIT_BARRIER;
	
	/* 								*/
	/* Step 5. Wait until you receive message that the chile is at  */
	/*             the barrier.  					*/
	if(mdebug>=1)
	   printf("Master listening for child to send at barrier message.\n");
	master_listen(master_listen_socket,sizeof(struct master_command));
	mc = (struct master_command *)&master_rcv_buf;
	child_index = mc->m_client_number;
	child_stat = (struct child_stats *)&shmaddr[child_index];	
	child_stat->flag = (long long)(mc->m_child_flag);
	if(mdebug>=1)
	   printf("Master sees child %d at barrier message.\n",child_index);

	return(x); /* Tell code above that it is the parent returning */
}

/****************************************************************************************/
/* This is the code that the client will use when it 					*/
/* gets started via remote shell. It is activated by the -+c controller_name option.	*/
/*											*/
/* The steps to this process are:							*/
/* 1. Start client receive channel 							*/
/* 2. Start client send channel 							*/
/* 3. Send message to controller saying I'm joining. 					*/
/* 4. Go into a loop and get all instructions from 					*/
/* 5. Get state information from the master 						*/
/* 6. Change to the working directory 							*/
/* 7. Run the test 									*/
/* 8. Release the listen and send sockets to the master 				*/
/* 											*/
/****************************************************************************************/
#ifdef HAVE_ANSIC_C
void
become_client(void)
#else
void
become_client()
#endif
{
	int x,testnum;
	struct master_command mc;
	struct client_command *cc;
	char client_name[256];
	char *workdir;

	x=fork(); /* Become a daemon so that remote shell will return. */
	if(x != 0)
		exit(0);
	/*
 	 * I am the child 
	 */
	(void)gethostname(client_name,256);

	fclose(stdin);
	fflush(stdout);
	fclose(stdout);
	fclose(stderr);
	if(cdebug)
	{
		newstdin=fopen("/tmp/don_in","r+");
		newstdout=fopen("/tmp/don_out","a+");
		newerrout=fopen("/tmp/don_err","a+");
	}
	if(cdebug>=1)
		fprintf(newstdout,"My name = %s, Controller's name = %s\n",client_name, controlling_host_name);

	/* 1. Start client receive channel 					*/

	l_sock = start_child_listen(sizeof(struct client_command));
	l_async_sock = start_child_listen_async(sizeof(struct client_command));

	/* 2. Start client send channel 					*/

	s_sock = start_child_send(controlling_host_name);

	/* 3. Send message to controller saying I'm joining. 			*/

	strcpy(mc.m_host_name,controlling_host_name);
	strcpy(mc.m_client_name,client_name);
	mc.m_child_port = child_port;
	mc.m_child_async_port = child_async_port;
	mc.m_command = R_CHILD_JOIN;
	if(cdebug)
	{
		fprintf(newstdout,"Child sends JOIN to master %s My port %d\n",
			controlling_host_name,child_port);
		fflush(newstdout);
	}
	child_send(s_sock, controlling_host_name,(struct master_command *)&mc, sizeof(struct master_command));

	/* 4. Go into a loop and get all instructions from 			*/
        /*    the controlling process. 						*/

	if(cdebug>=1)
	{
		fprintf(newstdout,"Child waiting for who am I\n");
		fflush(newstdout);
	}
	child_listen(l_sock,sizeof(struct client_command));
	cc = (struct client_command *)&child_rcv_buf;
	if(cc->c_command == R_TERMINATE)
	{
		if(cdebug)
		{
			fprintf(newstdout,"Child received terminate on sync channel !!\n");
			fflush(newstdout);
		}
		exit(1);
	}
	
	if(cdebug)
	{
		fprintf(newstdout,"Child sees: \n Client name %s \n Client_num # %d \n Host_name %s\n",
		cc->c_client_name,cc->c_client_number,cc->c_host_name);
		fflush(newstdout);
	}

	/*
	 * Now import all of the values of the flags that the child on this machine needs
	 * to be able to run the test requested.
	 */

	/* 5. Get state information from the master */

	numrecs64 = cc->c_numrecs64;
	reclen = cc->c_reclen;
	testnum = cc->c_testnum;
	chid = cc->c_client_number;
	workdir=cc->c_working_dir;
	oflag = cc->c_oflag;
	jflag = cc->c_jflag;
	direct_flag = cc->c_direct_flag;
	async_flag = cc->c_async_flag;
	mmapflag = cc->c_mmapflag;
	fetchon = cc->c_fetchon;
	verify = cc->c_verify;
	file_lock = cc->c_file_lock;
	Q_flag = cc->c_Q_flag;
	xflag = cc->c_xflag;
	include_flush = cc->c_include_flush;
	OPS_flag = cc->c_OPS_flag;
	purge = cc->c_purge;
	mmapnsflag = cc->c_mmapnsflag;
	mmapssflag = cc->c_mmapssflag; 
	no_copy_flag = cc->c_no_copy_flag;
	no_unlink = cc->c_no_unlink;
	include_close = cc->c_include_close;
	disrupt_flag = cc->c_disrupt_flag;
	compute_flag = cc->c_compute_flag;
	delay = cc->c_delay;
	if(cdebug)
	{
		fprintf(newstdout,"Child change directory to %s\n",workdir);
		fflush(newstdout);
	}

	/* 6. Change to the working directory */

	chdir(workdir);
	start_child_listen_loop(); /* The async channel listener */

	/* Need to start this after getting into the correct directory */

	/* 7. Run the test */
	switch(testnum) {

	case THREAD_WRITE_TEST : 
		if(cdebug>=1)
		{
			fprintf(newstdout,"Child running thread_write_test\n");
			fflush(newstdout);
		}
		thread_write_test((long)0);
		break;
	case THREAD_REWRITE_TEST : 
		if(cdebug>=1)
		{
			fprintf(newstdout,"Child running thread_rewrite_test\n");
			fflush(newstdout);
		}
		thread_rwrite_test((long)0);
		break;
	case THREAD_READ_TEST : 
		if(cdebug>=1)
		{
			fprintf(newstdout,"Child running thread_read_test\n");
			fflush(newstdout);
		}
		thread_read_test((long)0);
		break;
	case THREAD_REREAD_TEST : 
		if(cdebug>=1)
		{
			fprintf(newstdout,"Child running thread_reread_test\n");
			fflush(newstdout);
		}
		thread_rread_test((long)0);
		break;
	case THREAD_STRIDE_TEST : 
		if(cdebug>=1)
		{
			fprintf(newstdout,"Child running thread_stride_read_test\n");
			fflush(newstdout);
		}
		thread_stride_read_test((long)0);
		break;
	case THREAD_RANDOM_READ_TEST : 
		if(cdebug>=1)
		{
			fprintf(newstdout,"Child running random read test\n");
			fflush(newstdout);
		}
		thread_ranread_test((long)0);
		break;
	case THREAD_RANDOM_WRITE_TEST : 
		if(cdebug>=1)
		{
			fprintf(newstdout,"Child running random write test\n");
			fflush(newstdout);
		}
		thread_ranwrite_test((long)0);
		break;
	case THREAD_REVERSE_READ_TEST : 
		if(cdebug>=1)
		{
			fprintf(newstdout,"Child running reverse read test\n");
			fflush(newstdout);
		}
		thread_reverse_read_test((long)0);
		break;
	case THREAD_CLEANUP_TEST : 
		if(cdebug>=1)
		{
			fprintf(newstdout,"Child running cleanup\n");
			fflush(newstdout);
		}
		thread_cleanup_test((long)0);
		break;
	};
	if(cdebug>=1)
	{
		fprintf(newstdout,"Child finished running test.\n");
		fflush(newstdout);
	}
	
	/* 8. Release the listen and send sockets to the master */
	stop_child_listen(l_sock);
	stop_child_send(s_sock);

	exit(0);
}

/*
 * Clients tell the master their statistics, set the stopped flag, and set shared memory
 * child_flag to tell the master they are finished. Also each client report all statistics.
 */
#ifdef HAVE_ANSIC_C
void
tell_master_stats(testnum , chid, throughput, actual, stop_time, start_time,
		 fini_time, stop_flag, child_flag)
int testnum; 
long long chid; 
double throughput, actual, stop_time, start_time, fini_time;
char stop_flag;
long long child_flag;
/*
void
tell_master_stats(int testnum , long long chid, double tthroughput, 
		double actual, double stop_time, double start_time,
		double fini_time, char stop_flag, long long child_flag)
*/
#else
void
tell_master_stats(testnum , chid, throughput, actual, stop_time, start_time,
		 fini_time, stop_flag, child_flag)
int testnum; 
long long chid; 
double throughput, actual, stop_time, start_time, fini_time;
char stop_flag;
long long child_flag;
#endif
{
	struct master_command mc;
	mc.m_client_number = (int) chid;
	mc.m_throughput= throughput;
	mc.m_testnum = testnum;
	mc.m_actual = actual;
	mc.m_stop_time = stop_time;
	mc.m_start_time = start_time;
	mc.m_fini_time = fini_time;
	mc.m_stop_flag = stop_flag;
	mc.m_child_flag = child_flag;
	mc.m_command = R_STAT_DATA;
	if(cdebug>=1)
	{
		fprintf(newstdout, "Child: Tell master stats and terminate\n");
		fflush(newstdout);
	}
	child_send(s_sock, controlling_host_name,(struct master_command *)&mc, sizeof(struct master_command));
}
	
/*
 * Stop the master listener loop service.
 * Currently this is not used. The master_join_count
 * variable is used to terminate the loop service.
 */
#ifdef HAVE_ANSIC_C
void
stop_master_listen_loop(void)
#else
void
stop_master_listen_loop()
#endif
{
	if(mdebug>=1)
		printf("Stopping Master listen loop");
	kill(master_listen_pid,SIGKILL);
}


/*
 * Clients tell the master that I am at the barrier and ready
 * for the message to start work.
 */
#ifdef HAVE_ANSIC_C
void
tell_master_ready(long long chid)
#else
void
tell_master_ready()
long long chid;
#endif
{
	struct master_command mc;
	if(cdebug>=1)
	{
		fprintf(newstdout,"Child: Tell master to go\n");
		fflush(newstdout);
	}
	mc.m_command = R_FLAG_DATA;
	mc.m_child_flag = CHILD_STATE_READY; 
	mc.m_client_number = (int)chid; 
	child_send(s_sock, controlling_host_name,(struct master_command *)&mc, sizeof(struct master_command));
}

/*
 * Clients wait at a barrier for the master to tell them
 * to begin work. This is the function where they wait.
 */
#ifdef HAVE_ANSIC_C
void
wait_for_master_go(long long chid)
#else
wait_for_master_go()
long long chid;
#endif
{
	struct client_command *cc;
	child_listen(l_sock,sizeof(struct client_command));
	cc = (struct client_command *)child_rcv_buf;
	if(cc->c_command == R_TERMINATE)
	{
		if(cdebug)
		{
			fprintf(newstdout,"Child received terminate on sync channel at barrier !!\n");
			fflush(newstdout);
		}
		exit(1);
	}
	if(cdebug>=1)
		fprintf(newstdout,"Return from wait_for_master_go\n");
}

/*
 * Create a master listener for receiving data from the
 * many children. As the children finish they will send
 * their statistics and terminate. When the master_join_count 
 * goes to zero then it is time to stop this service.
 * When this service exits then the parent will know
 * that all of the children are done.
 */
#ifdef HAVE_ANSIC_C
void
start_master_listen_loop(int num)
#else
void
start_master_listen_loop(num)
int num;
#endif
{
	int i;
	struct child_stats *child_stat;
	struct master_command *mc;

	master_join_count=num;
	master_listen_pid=fork();
	if(master_listen_pid!=0)
		return;
	if(mdebug>=1)
		printf("Starting Master listen loop m %d c %d count %d\n",master_iozone, 
			client_iozone,num);

	while(master_join_count)
	{
		master_listen(master_listen_socket,sizeof(struct master_command));
		mc=(struct master_command *)&master_rcv_buf;
		switch(mc->m_command) {
		case R_STAT_DATA:
			i = mc->m_client_number;
			if(mdebug)
				printf("loop: R_STAT_DATA for client %d\n",i);
			child_stat = (struct child_stats *)&shmaddr[i];	
			child_stat->flag = mc->m_child_flag;
			child_stat->actual = mc->m_actual;
			child_stat->throughput = mc->m_throughput;
			child_stat->start_time = mc->m_start_time;
			child_stat->stop_time = mc->m_stop_time;
			*stop_flag = mc->m_stop_flag;
			master_join_count--;
			break;
		case R_FLAG_DATA:
			if(mdebug)
				printf("loop: R_FLAG_DATA: Client %d flag %d \n",
				  (int)mc->m_client_number,
				  (int)mc->m_child_flag);
			i = mc->m_client_number;
			child_stat = (struct child_stats *)&shmaddr[i];	
			child_stat->flag = (long long)(mc->m_child_flag);
			break;
		case R_STOP_FLAG:
			if(mdebug)
			  printf("Master loop: R_STOP_FLAG: Client %d STOP_FLAG \n",
				  (int)mc->m_client_number);
			*stop_flag=1;
			distribute_stop();
			break;
		}
			
	}
	exit(0);
}
/*
 * Create a client listener for receiving async data from the
 * the master. 
 */
#ifdef HAVE_ANSIC_C
void
start_child_listen_loop(void)
#else
void
start_child_listen_loop()
#endif
{
	int i;
	struct child_stats *child_stat;
	struct client_command *cc;

	client_listen_pid=fork();
	if(client_listen_pid!=0)
		return;
	if(mdebug>=1)
		printf("Starting client listen loop\n");

	while(1)
	{
		child_listen_async(l_async_sock,sizeof(struct client_command));
		cc=(struct client_command *)&child_async_rcv_buf;
		switch(cc->c_command) {
		case R_STOP_FLAG:
			i = cc->c_client_number;
			if(cdebug)
				fprintf(newstdout,"child loop: R_STOP_FLAG for client %d\n",i);
			child_stat = (struct child_stats *)&shmaddr[i];	
			*stop_flag = cc->c_stop_flag; /* In shared memory with other copy */
			break;
		case R_TERMINATE:
			if(cdebug)
			{
				fprintf(newstdout,"Child loop: R_TERMINATE: Client %d \n",
				  (int)cc->c_client_number);
				fflush(newstdout);
			}
			i = cc->c_client_number;
			child_remove_files(i);
			exit(0);
		}
			
	}
}

/*
 * The controlling process "master" tells the children to begin.
 */

#ifdef HAVE_ANSIC_C
void
tell_children_begin(long long childnum)
#else
void
tell_children_begin(childnum)
long long childnum;
#endif
{
	struct client_command cc;
	int x;
	x = (int) childnum;
	if(mdebug>=1)
		printf("Master: Tell child %d to begin\n",x);
	cc.c_command = R_FLAG_DATA;
	cc.c_child_flag = CHILD_STATE_BEGIN; 
	cc.c_client_number = (int)childnum; 
	master_send(master_send_sockets[x],child_idents[x].child_name, &cc,sizeof(struct client_command));
}

/*
 * The master waits here for all of the the children to terminate.
 * When the children are done the the master_join_count will be at zero
 * and the master_listen_loop will exit. This function waits for this to happen.
 */
#ifdef HAVE_ANSIC_C
void
wait_dist_join(void)
#else
void
wait_dist_join()
#endif
{
	wait(0);
	if(mdebug)
		printf("Master: All children have finished. Sending terminate\n");
	terminate_child_async(); /* All children are done, so terminate their async channel */
	current_client_number=0; /* start again */
}


/* 
 * This function reads a file that contains client information. 
 * The information is:
 * 	client name (DNS usable name)
 *	client working directory (where to run the test)
 * 	client directory that contains the Iozone executable.
 *
 * If the first character in a line is a # then it is a comment.
 * The maximum number of clients is MAXSTREAMS.
 */
#ifdef HAVE_ANSIC_C
int
get_client_info(void)
#else
int
get_client_info()
#endif
{
	FILE *fd;
	char *ret1;
	int count;
	char buffer[200];
	count=0;
	fd=fopen(client_filename,"r");
	if(fd == (FILE *)0)
	{
		printf("Unable to open client file \"%s\"\n",
			client_filename);
		exit(176);
	}
	while(1)
	{
		ret1=fgets(buffer,200,fd);
		if(ret1==(char *)0)
			break;
		count+=parse_client_line(buffer,count);
	}
	return(count);
}


/*
 * This function parses a line from the client file. It is
 * looking for:
 *	Client name  (DNS usable)
 *	Client working directory (where to run the test )
 *	Client path to Iozone executable.
 *
 * Lines that start with # are comments.
 */

#ifdef HAVE_ANSIC_C
int 
parse_client_line(char *buffer,int line_num)
#else
int
parse_client_line(buffer, line_num)
char *buffer;
int line_num;
#endif
{
	/* Format is clientname, workdir, execute_path */
	/* If column #1 contains a # symbol then skip this line */

	if(buffer[0]=='#')
		return(0);
	sscanf(buffer,"%s %s %s\n",
		child_idents[line_num].child_name,
		child_idents[line_num].workdir,
		child_idents[line_num].execute_path);
	if(mdebug)
	{
		printf("Client: %s  Workdir %s  Execute_path %s\n",
		child_idents[line_num].child_name,
		child_idents[line_num].workdir,
		child_idents[line_num].execute_path);
	}
	return(1);
}

/* 
 * This is a mechanism that the child uses to remove all
 * of its temporary files. Only used at terminate time.
 */
#ifdef HAVE_ANSIC_C
void
child_remove_files(int i)
#else
void
child_remove_files(i)
int i;
#endif
{

	char *dummyfile[MAXSTREAMS];           /* name of dummy file     */
	dummyfile[i]=(char *)malloc((size_t)MAXNAMESIZE);
#ifdef NO_PRINT_LLD
	sprintf(dummyfile[i],"%s.DUMMY.%ld",filearray[i],i);
#else
	sprintf(dummyfile[i],"%s.DUMMY.%lld",filearray[i],i);
#endif
	unlink(dummyfile[i]);
}

	
/*
 * The master tells the child async listener that it is time
 * to terminate its services.
 */
#ifdef HAVE_ANSIC_C
void
terminate_child_async(void)
#else
void
terminate_child_async()
#endif
{
	int i;
	struct client_command cc;
	cc.c_command = R_TERMINATE;
	for(i=0;i<num_child;i++)
	{
		child_idents[i].state = C_STATE_ZERO;
		cc.c_client_number = (int)i; 
		if(mdebug)
			printf("Master terminating async channels to children.\n");
		master_send(master_send_async_sockets[i],child_idents[i].child_name, &cc,sizeof(struct client_command));
	}
}

/*
 * The master has received an update to the stop flag and is
 * now distributing this to all of the clients.
 */
#ifdef HAVE_ANSIC_C
void
distribute_stop(void)
#else
void
distribute_stop()
#endif
{
	int i;
	struct client_command cc;
	cc.c_command = R_STOP_FLAG;
	for(i=0;i<num_child;i++)
	{
		cc.c_client_number = (int)i; 
		if(mdebug)
			printf("Master distributing stop flag to child %d\n",i);
		master_send(master_send_async_sockets[i],child_idents[i].child_name, &cc,sizeof(struct client_command));
	}
}

/*
 * Child is sending its stop flag to the master.
 */
#ifdef HAVE_ANSIC_C
void
send_stop(void)
#else
void
send_stop()
#endif
{
	struct master_command mc;

	mc.m_command = R_STOP_FLAG,
	mc.m_client_number = chid;
	if(cdebug)
	{
		fprintf(newstdout,"Child sending stop flag to master\n");
		fflush(newstdout);
	}
        child_send(s_sock, controlling_host_name,(struct master_command *)&mc, sizeof(struct master_command));
}

/*
 * This is very tricky stuff. There are points in time when 
 * someone can hit control-c and cause the master to want to die.
 * Ok..now how does the master contact all the clients and tell
 * them to stop ?  The clients may be in 3 different states.
 * Not started yet, Joined and waiting for the WHO information, 
 * or at the barrier.  If the client is not started... cool.
 * ignore it. If the client has joined and is waiting at WHO
 * then the client does not have an async listener yet. So 
 * the master only needs to tell the client (sync) channel
 * to terminate. If the client is at the barrier then the 
 * client has two processes. One at the barrier and another
 * that is providing the async services. So... the master
 * needs to terminate both of these processes.
 */
#ifdef HAVE_ANSIC_C
void
cleanup_children(void)
#else
void
cleanup_children()
#endif
{
	int i;
	struct client_command cc;
	cc.c_command = R_TERMINATE;
	for(i=0;i<num_child;i++)
	{
		cc.c_client_number = (int)i; 
		/* Child not started yet */
		if(child_idents[i].state == C_STATE_ZERO)
			;
		/* Child is waiting for who info */
		if(child_idents[i].state == C_STATE_WAIT_WHO)
		{
			if(mdebug)
				printf("Master sending signaled death to child !!\n");
			master_send(master_send_sockets[i],child_idents[i].child_name, &cc,sizeof(struct client_command));
		}
		/* Child is waiting at the barrier */
		if(child_idents[i].state == C_STATE_WAIT_BARRIER)
		{
			if(mdebug)
				printf("Master sending signaled death to child !!\n");
			master_send(master_send_sockets[i],child_idents[i].child_name, &cc,sizeof(struct client_command));
			if(mdebug)
				printf("Master sending signaled death to child async !!\n");
			master_send(master_send_async_sockets[i],child_idents[i].child_name, &cc,sizeof(struct client_command));
		}
			
	}
}

/*
 * This closes the file descriptors that were created for the master send and async send
 * at the end of each phase of the throughput testing.
 */
#ifdef HAVE_ANSIC_C
void
cleanup_comm(void)
#else
void
cleanup_comm()
#endif
{
	int i;
	for(i=0;i<num_child;i++)
	{
        	close(master_send_sockets[i]);
        	close(master_send_async_sockets[i]);
	}
}
