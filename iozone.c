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
/*    	Copyright 1991, 1992, 1994, 1998  William D. Norcott		*/
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
#define THISVERSION "        Version $Revision: 3.28 $"

/* Include for Cygnus development environment for Windows */
#ifdef Windows
#include <Windows.h>
int errno;
#else
extern  int errno;
#endif


#if defined (__LP64__) || defined(OSF_64)
#define MODE "\tCompiled for 64 bit mode."
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
long wait();
int fsync();
void srand48();
long lrand48();
void create_list();
void Poll();
void print_header();
void Kill();
#ifndef Windows
long long min();
long long max();
#endif
long long mythread_create();
#endif


char *help[] = {
"    Usage: iozone [-s kilobytes] [-r record_length in kb ] [-f [path]filename] ",
"                  [-i test] [-E] [-p] [-a] [-A] [-m] [-M] [-t children] [-h] [-o] ",
"                  [-l min_number_procs] [-u] max_number_procs] [-v] [-R] [-x]",
"                  [-d microseconds] [-F path1 path2...] [-V pattern ] [-j stride ]",
"                  [-T ] [-C] [-B] [-D] [-G] [-I] [-H depth] [-k depth] [-U mount_point]",
"                  [-S cache_size] [-O] [-L line_size] ",
"                  [-N] [-Q] [-P start_cpu] [-e] [-c] [-b Excel.xls]",
" ",
"           -s Kilobytes in file.",
"              or -s #k .. size in Kb",
"              or -s #m .. size in Mb",
"           -r record size in kb",
"              or -r #k .. size in Kb",
"              or -r #m .. size in Mb",
"           -f filename to use",
"           -i # Test to run. (0=write/rewrite, 1=read/re-read, 2=random-read/write",
"                 3=Read-backwards, 4=Re-write-record, 5=stride-read, 6=fwrite/re-fwrite",
"                 7=fread/Re-fread, 8=pwrite/Re-pwrite, 9=pread/Re-pread, 10=pwritev/Re-pwritev",
"                 11=preadv/Re-preadv)",
"           -E run extension tests",
"           -p purge on",
"           -a auto mode",
"           -A auto2 mode",
"           -m multi buffer",
"           -M uname -a output",
"           -W Lock file when reading or writing",
"           -t # throughput test",
"           -h help",
"           -o Writes are synch (O_SYNC)",
"           -l Lower limit on number of processes to run",
"           -u Upper limit on number of processes to run",
"           -v version info",
"           -R Generate Excel report",
"           -x Turn off stone-walling",
"           -d Microsecond delay out of barrier",
"           -F number_procs filenames for throughput test",
"           -V Verify data pattern write/read",
"           -j # Set stride of file accesses to (# * record size)",
"           -T Use threads for throughput tests ",
"           -C Show bytes transfered by each child in throughput testing.",
"           -B Use mmap() files",
"           -D Use msync(MS_ASYNC) on mmap files",
"           -G Use msync(MS_SYNC) on mmap files",
"           -I Use VxFS VX_DIRECT for all file operations",
"           -H Use POSIX async I/O with # async operations",
"           -k Use POSIX async I/O. (no bcopy) with # async operations",
"           -U Mount point to remount between tests",
"           -S Set processor cache size to value (in Kbytes).",
"           -O Give results in ops/sec.",
"           -L Set processor cache line size to value (in bytes).",
"           -N Report results in microseconds per operation.",
"           -Q Create offset/latency files.",
"           -P start_cpu. Bind processes/threads to processors, starting with this cpu",
"           -e Include flush (fsync,fflush) in the timing calculations",
"           -c Include close in the timing calculations",
"           -b Filename. Create Excel worksheet file",
"           -w Do not unlink temporary file",
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
typedef long long off64_t;
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
#endif

#ifdef unix
#include <sys/file.h>
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

/* for systems with System V-style time, define SysVtime */
#ifdef M_SYSV
#define SysVtime
#endif

#ifdef SysVtime
#include <sys/times.h>
#include <sys/param.h>
#ifndef CLK_TCK
#define CLK_TCK HZ
#endif
#endif
/* for systems with BSD style time, define BSDtime */
#ifdef bsd4_2
#define BSDtime
#endif
#ifdef bsd4_4
#define BSDtime
#endif
#ifdef BSDtime
#include <sys/time.h>
#else
#include <sys/time.h>
#endif

#ifdef SHARED_MEM
#include <sys/shm.h>
#endif

#ifdef Windows
long long page_size = 4096;
#define GOT_PAGESIZE 1
#endif

#ifdef netbsd
#define MAP_ANONYMOUS MAP_ANON
long long page_size = 4096;
#define GOT_PAGESIZE 1
#endif

#ifdef bsd4_2
long long page_size = 4096;
#define GOT_PAGESIZE 1
#define MS_SYNC 0
#define MS_ASYNC 0
#endif

#ifdef __bsdi__
#define MAP_ANONYMOUS MAP_ANON
#endif

#ifdef bsd4_4
#ifndef GOT_PAGESIZE
long long page_size = 4096;
#define GOT_PAGESIZE 1
#endif
#endif 

#ifdef SCO
long long page_size = 4096;
#define GOT_PAGESIZE 1
#define AMAP_FILE (0)
#endif

#ifdef solaris
long long page_size = 4096;
#define GOT_PAGESIZE 1
#define MAP_FILE (0)
#endif

#ifdef linux
#ifndef GOT_PAGESIZE
#include <asm/page.h>
long long page_size = PAGE_SIZE;
#define GOT_PAGESIZE 1
#endif
#endif

#ifdef IRIX64
long long page_size = 4096;
#define GOT_PAGESIZE 1
#endif

#ifdef IRIX
long long page_size = 4096;
#define GOT_PAGESIZE 1
#endif

#ifdef NBPG
#ifndef GOT_PAGESIZE
long long page_size = NBPG;
#define GOT_PAGESIZE 1
#endif
#endif

#ifndef GOT_PAGESIZE
long long page_size = 4096; /* Used when all else fails */
#endif

#ifdef HAVE_PREAD
#include <sys/puio.h>
#define PVECMAX 16
struct piovec piov[PVECMAX];
#endif

struct child_stats {
	long long flag;	/* control space */
	long long flag1;	/* pad */
	long long flag2;	/* pad */
	long long flag3;	/* pad */
	double throughput; /* Throughput in either kb/sec or ops/sec */
	double actual;	   /* Either actual kb read or # of ops performed */
	double start_time;	/* Actual start time */
	double stop_time;	/* Actual end time */
} VOLATILE *child_stat;

#ifdef __convex_spp
#include <sys/cnx_ail.h>
#endif

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

#define LARGE_REC 65536
#define KILOBYTES 512 			/* number of kilobytes in file */
#define RECLEN 1024			/* number of bytes in a record */
#define FILESIZE (KILOBYTES*1024)	/*size of file in bytes*/
#define NUMRECS FILESIZE/RECLEN		/* number of records */

#ifdef __bsdi__
#define CROSSOVER (8*1024)		/* At 16 Meg switch to large records */
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
#define MAX_X 100
#define MAX_Y 200
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
#endif
static double time_so_far1();	/* time since start of program    */
void get_resolution();
void signal_handler();		/* clean up if user interrupts us */
void begin();			/* The main worker in the app     */
void fetchit();			/* Prime on chip cache		  */
void purgeit();			/* Purge on chip cache		  */
void throughput_test();		/* Multi process throughput 	  */
void multi_throughput_test();	/* Multi process throughput 	  */
void prepage();			/* Pre-fault user buffer	  */

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
void purge_buffer_cache(void);
char *alloc_mem(long long);
void *(thread_rwrite_test)(void *);
void *(thread_write_test)(void *);
void *(thread_read_test)(void*);
void *(thread_ranread_test)(void *);
void *(thread_rread_test)(void *);
void *(thread_reverse_read_test)(void *);
void *(thread_stride_read_test)(void *);
void *(thread_set_base)(void *);
void *(thread_join)(long long, void *);
#else
void fill_buffer();
char *alloc_mem();
void *(thread_rwrite_test)();
void *(thread_write_test)();
void *(thread_read_test)();
void *(thread_ranread_test)();
void *(thread_rread_test)();
void *(thread_reverse_read_test)();
void *(thread_stride_read_test)();
void *(thread_set_base)();
void *(thread_join)();
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
double time_res;
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
char Q_flag;
int bif_fd;
int bif_row,bif_column;
char aflag, mflag, pflag, Eflag, hflag, oflag, Rflag, rflag, sflag;
char no_copy_flag,h_flag,k_flag,include_close,include_flush,bif_flag;
int direct_flag;
char async_flag;
char trflag; 
long long mint, maxt,depth; 
char fflag, Uflag,uflag,lflag,OPS_flag,include_tflag; 
char MS_flag;
char mmapflag,mmapasflag,mmapssflag,mmapnsflag,mmap_mix;
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

char bif_filename [MAXNAMESIZE];           /* name of biff file      */
char filename [MAXNAMESIZE];               /* name of temporary file */
char mountname [MAXNAMESIZE];              /* name of device         */
char dummyfile [MAXSTREAMS][MAXNAMESIZE];  /* name of dummy file     */
char dummyfile1 [MAXNAMESIZE];             /* name of dummy file     */
char *filearray[MAXSTREAMS];		   /* array of file names    */
char tfile[] = "iozone";
char *buffer, *mbuffer,*mainbuffer;
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
char *throughput_tests[] = {"Initial write","Rewrite","Read","Re-read",
	"Reverse Read","Stride read","Random read"};

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
	FILE *pi;
	char reply[99];
	unsigned char inp_pat;
	time_t time_run;
	char *port,*pl,*m;
	int num_child1;
	int cret,test_foo,test_fd;
	int anwser,bind_cpu;

	setvbuf( stdout, NULL, _IONBF, (size_t) NULL );
	setvbuf( stderr, NULL, _IONBF, (size_t) NULL );
	
#if defined (solaris) || defined (_HPUX_SOURCE) || defined (linux) || defined(IRIX) || defined (IRIX64)
#ifndef __convex_spp
	page_size=getpagesize();
#endif
#endif
	for(ind=0;ind<MAXSTREAMS;ind++)
		filearray[ind]=(char *)tfile;

	myid=(long long)getpid();
	get_resolution();
	time_run = time(0);
    	printf("\tIozone: Performance Test of File I/O\n");
    	printf("\t%s\n\t%s\n\n", THISVERSION,MODE);
    	printf("\tContributors:William Norcott, Don Capps, Isom Crawford, Kirby Collins\n");
	printf("\t             Al Slater, Scott Rhine, Mike Wisner, Ken Goss\n");
    	printf("\t             Steve Landherr, Brad Smith.\n\n");
	printf("\tRun began: %s\n",ctime(&time_run));

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

#ifdef __LP64__ 
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
	while((cret = getopt(argc,argv,"ZQNIBDGCTOMREWovAxamphcej:k:V:r:t:s:f:F:d:l:u:U:S:L:H:P:i:b:w ")) != EOF){
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
			printf("\tPOSIX Async I/O (no bcopy). Depth %ld \n",depth);
#else
			printf("\tPOSIX Async I/O (no bcopy). Depth %lld \n",depth);
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
			printf("\tPOSIX async I/O (with bcopy). Depth %ld\n",depth);
#else
			printf("\tPOSIX async I/O (with bcopy). Depth %lld\n",depth);
#endif
			async_flag++;
			break;
		case 'I':	/* Use vxfs direct advisory */
#ifdef VXFS
			direct_flag++;
			test_fd=open("vxfstest",O_CREAT|O_RDWR,0660);
			ioctl(test_fd,VX_SETCACHE,VX_DIRECT);
			ioctl(test_fd,VX_GETCACHE,&test_foo);
			unlink("vxfstest");
			if(test_foo == 0)
			{
#endif
				printf("\tVxFS advanced setcache feature not available\n");
				exit(3);
#ifdef VXFS
			}
			printf("\tVxFS advanced feature SET_CACHE, VX_DIRECT enabled\n");
#endif
			break;
		case 'B':	/* Use mmap file for test file */
			printf("\tUsing mmap files\n");
			mmapflag++;
			mmapnsflag++;
			break;
		case 'D':	/* Use async msync mmap file */
			printf("\tUsing msync(MS_ASYNC) on mmap files\n");
			mmapflag++;
			mmapasflag++;
			mmapnsflag=0;
			break;
		case 'G':	/* Use msync sync for mmap file */
			printf("\tUsing msync(MS_SYNC) on mmap files\n");
			mmapssflag++;
			mmapnsflag=0;
			break;
		case 'C':	/* show children xfer counts */
			Cflag++;
			break;
		case 'Q':	/* Output offset/latency files */
	    		printf("\tOffset/latency files enabled.\n");
			Q_flag++;
			break;
		case 'x':	/* disable stone_wall */
	    		printf("\tStonewall disabled\n");
			xflag++;
			break;

		case 'a':	/* auto mode */
			fetchon=1;
			purge=0;
			multi_buffer=0;
	    		auto_mode = 1;
			aflag++;
	    		printf("\tAuto Mode\n");
			break;
		case 'c':	/* Include close in timing */
			include_close++;
	    		printf("\tInclude close in write timing\n");
			break;
		case 'e':	/* Include fsync in timing */
			include_flush++;
	    		printf("\tInclude fsync in write timing\n");
			break;
		case 'A':	/* auto2 mode - kcollins 8-21-96*/
			fetchon=1;
			purge=0;
			multi_buffer=0;
	    		auto_mode = 1;
			aflag++;
	    		printf("\tAuto Mode\n");
			RWONLYflag++;
			NOCROSSflag++;
			break;
		case 's': 	/* set file size */
#ifdef NO_PRINT_LLD
			sscanf(optarg,"%ld",&kilobytes64);
#else
			sscanf(optarg,"%lld",&kilobytes64);
#endif
			if(optarg[strlen(optarg)-1]=='k' ||
				optarg[strlen(optarg)-1]=='K'){
#ifdef NO_PRINT_LLD
				sscanf(optarg,"%ld",&kilobytes64);
#else
				sscanf(optarg,"%lld",&kilobytes64);
#endif
			}
			if(optarg[strlen(optarg)-1]=='m' ||
				optarg[strlen(optarg)-1]=='M'){
#ifdef NO_PRINT_LLD
				sscanf(optarg,"%ld",&kilobytes64);
#else
				sscanf(optarg,"%lld",&kilobytes64);
#endif
				kilobytes64 *= 1024;
			}
			if(kilobytes64 <= 0)
				kilobytes64=512;
#ifdef NO_PRINT_LLD
	    		printf("\tFile size set to %ld KB\n",kilobytes64);
#else
	    		printf("\tFile size set to %lld KB\n",kilobytes64);
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
	    		printf("\tMulti_buffer. Work area %d bytes\n",
				MAXBUFFERSIZE);
			break;
                case 'M':       /* Report machine name and OS */
                        pi=popen("uname -a", "r");
                        fread(reply,99,1,pi);
			m=reply;
                        while(*m!='\n') /* Strip after new line */
                                m++;
                        *m=0;
                        printf("\n\tMachine = %s\n",reply);
                        pclose(pi);
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
                        printf("\tBinding of processors beginning with %d \n",begin_proc);
			ioz_processor_bind++;
#else
			printf("\tProcessor binding not available in this version\n");
#endif
#endif
                        break;
		case 'p':	/* purge the processor cache */
	    		printf("\tPurge Mode On\n");
			fetchon=0;
			pflag++;
			purge=1;
     			pbuffer = (char *) alloc_mem((long long)(3 * cache_size));
			if(pbuffer == 0) {
                        	perror("Memory allocation failed:");
                          	exit(9);
			}
#ifdef __LP64__ 
		     	pbuffer = (char *) 
				(((unsigned long long)pbuffer + cache_size ) 
					& ~(cache_size-1));
#else
		     	pbuffer = (char *) 
				(((long)pbuffer + (long)cache_size ) 
					& ~((long)cache_size-1));
#endif
			break;
		case 'h':	/* show help */
			hflag++;
	    		show_help();
			exit(0);
			break;
		case 'E':	/* show help */
			Eflag++;
			break;
		case 'R':	/* Generate Report */
			Rflag++;
	    		printf("\tExcel chart generation enabled\n");
			break;
		case 'o':	/* Open OSYNC */
	    		printf("\tSYNC Mode. \n");
			oflag++;
			break;
		case 'O':	/* Report in Ops/sec instead of KB/sec */
	    		printf("\tOPS Mode. Output is in operations per second.\n");
			OPS_flag++;
			break;
		case 'N':	/* Report in usec/op  */
	    		printf("\tMicroseconds/op Mode. Output is in microseconds per operation.\n");
			MS_flag++;
			break;
		case 'V':	/* Turn on Verify */
			sverify=0;
			inp_pat = (char)(atoi(optarg));
			if(inp_pat == 0)
				inp_pat = PATTERN;
			pattern = ((inp_pat << 24) | (inp_pat << 16) | (inp_pat << 8) 
				| inp_pat);
			verify++;
	    		printf("\tVerify Mode. Pattern %x\n",pattern);
    			printf("\tPerformance measurements are invalid in this mode.\n");
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
			if(reclen <= 0)
				reclen=(long long)4096;

#ifdef NO_PRINT_LLD
	    		printf("\tRecord Size %ld KB\n",reclen/1024);
#else
	    		printf("\tRecord Size %lld KB\n",reclen/1024);
#endif
			break;
		case 'j':	/* Specify the stride in records */
			stride = (long long)(atoi(optarg));
			if(stride < 0)
				stride=0;
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
				printf("\tSelected test not available on the version.\n");
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
			printf("\tSetting no_unlink\n");
			no_unlink = 1;
			break;
		case 'Z':	/* Turn on the mmap and file I/O mixing */
			printf("\tEnable mmap & file I/O mixing.\n");
			mmap_mix = 1;
			break;
		case 'W':	/* Read/Write with file locked */
			file_lock=1;
			printf("\tLock file when reading/writing.\n");
			break;
		}
	}

	if(trflag && MS_flag)
	{
		printf("\n\tMicrosecond mode not supported in throughput mode.\n\n");
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
		
    	printf("\tTime Resolution = %1.6f seconds.\n",time_res);
#ifdef NO_PRINT_LLD
    	printf("\tProcessor cache size set to %ld Kbytes.\n",cache_size/1024);
    	printf("\tProcessor cache line size set to %ld bytes.\n",cache_line_size);
	printf("\tFile stride size set to %ld * record size.\n",stride);
#else
    	printf("\tProcessor cache size set to %ld Kbytes.\n",cache_size/1024);
    	printf("\tProcessor cache line size set to %ld bytes.\n",cache_line_size);
	printf("\tFile stride size set to %lld * record size.\n",stride);
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
		printf("\tMin %s = %ld \n",port,mint);
		printf("\tMax %s = %ld \n",port,maxt);
#else
		printf("\tMin %s = %lld \n",port,mint);
		printf("\tMax %s = %lld \n",port,maxt);
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
		printf("\tThroughput test with %ld %s\n", num_child,port);
#else
		printf("\tThroughput test with %lld %s\n", num_child,port);
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
		fill_buffer((char *)buffer,min(reclen,cache_size),(long long)pattern,(char)sverify);
		if(pflag)
			fill_buffer((char *)pbuffer,min(reclen,cache_size),(long long)pattern,(char)sverify);
		if(mflag)
			fill_buffer((char *)mbuffer,min(reclen,cache_size),(long long)pattern,(char)sverify);
	}
	else
	{
		bzero(buffer,(size_t)min(reclen,cache_size));
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
	if (!no_unlink)
	      	unlink(dummyfile[0]);	/* delete the file */
	printf("\niozone test complete.\n");
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
	store_value((off64_t)(reclen/1024));

#ifdef NO_PRINT_LLD
	printf("%16ld",kilobytes64);
	printf("%8ld",reclen/1024);
#else
	printf("%16lld",kilobytes64);
	printf("%8lld",reclen/1024);
#endif
	if(include_tflag)
	{
		for(i=0;i<num_tests;i++)
		{
			if(include_mask & (1<<i))
			   func[i](kilobytes64,reclen,&data1[i],&data2[i]);
			else
			{
			       	printf("%s",test_output[i]); 
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
	printf("\n");
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
    	printf("iozone: help mode\n\n");
    	for(i=0; strlen(help[i]); i++)
    	{
		printf("%s\n", help[i]);
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
	if((long long)getpid()==myid)
	{
    		printf("\niozone: interrupted\n\n");
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

	    	printf("exiting iozone\n\n");
		if(res_prob)
		{
			printf("Timer resolution is poor. Some small transfers may have \n");
			printf("reported inaccurate results. Sizes %d Kbytes and below.\n",
				(long)rec_prob/1024);
		}
		if(trflag && !use_thread)
		   for(i=0;i<num_child;i++)
			kill((pid_t)childids[i],SIGTERM);
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
	off64_t min_file_size = KILOBYTES_START;
        off64_t max_file_size = KILOBYTES_END;
        long long min_rec_size = RECLEN_START;
        long long max_rec_size = RECLEN_END;
	long long xover = CROSSOVER;
	long long mult,tmp,tmp2,tmp3,save_y=0;
	long long xx;

	tmp2=0;
	for(tmp=RECLEN_START;tmp<RECLEN_END;tmp=tmp<<1)
		tmp2++;
	tmp3=tmp2;
	/****************************************************************/
	/* Start with file size of 1 megabyte and repeat the test 	*/
	/* KILOBYTES_ITER_LIMIT  					*/
	/* times.  Each time we run, the file size is doubled		*/
	/****************************************************************/
        if(sflag) {
          min_file_size = kilobytes64;
          max_file_size = kilobytes64;
        }
        if(rflag) {
          min_rec_size = reclen;
          max_rec_size = reclen;
        }
	if(NOCROSSflag) xover = max_file_size;

        for(kilosi=min_file_size;kilosi<=max_file_size;kilosi*=MULTIPLIER)
        {
	/****************************************************************/
	/* Start with record size of min_rec_size bytes and repeat the 	*/
	/* test, multiplying the record size by MULTIPLIER each time, 	*/
	/* until we reach max_rec_size.	 At the CROSSOVER we stop doing */
	/* small buffers as it takes forever and becomes very 		*/
	/* un-interesting.						*/
	/****************************************************************/
             if(!rflag)
                     if(!sflag)
                             if(kilosi > xover){
                                     min_rec_size = LARGE_REC;
				     mult = RECLEN_START/1024;
				     /************************************/
				     /* Generate dummy entries in the    */
				     /* Excel buffer for skipped         */
				     /* record sizes			 */
				     /************************************/
				     for(count1=min_rec_size;
				     	(count1 != RECLEN_START) && (
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

             for (recszi=min_rec_size;recszi<=max_rec_size;recszi*=MULTIPLIER)
             {
                     if(recszi > (kilosi*1024)) break;
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
/* 	char flag; 		Used to barrier			*/
/* 	double throughput;	Childs throughput		*/
/* 	double actual;		Childs actual read/written	*/
/* } 								*/
/*								*/
/* There is an array of child_stat structures layed out in 	*/
/* shared memory.						*/
/*								*/
/****************************************************************/

#ifdef HAVE_ANSIC_C
void throughput_test(void)
#else
void throughput_test()
#endif
{
	char *unit;
	char *stackp;
	double starttime1 = 0;
	double jstarttime = 0;
	double jtime = 0;
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
	double min_xfer = 0; 


	ptotal=written_so_far=read_so_far=re_written_so_far=re_read_so_far=0 ;

	if(OPS_flag)
		unit="ops";
	else
		unit="KB";

	if(!haveshm)
	{
		shmaddr=(struct child_stats *)alloc_mem((long long)SHMSIZE);
#ifdef __LP64__ 
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
	for(xyz=0;xyz<num_child;xyz++){ /* all children to state 0 */
		child_stat = (struct child_stats *)&shmaddr[xyz];
		child_stat->flag=0;
		child_stat->actual=0;
		child_stat->throughput=0;
		child_stat->start_time=0;
		child_stat->stop_time=0;
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
#ifdef NO_PRINT_LLD
	printf("\tEach %s writes a %ld Kbyte file in %ld Kbyte records\n",
		port,kilobytes64,reclen/1024);
#else
	printf("\tEach %s writes a %lld Kbyte file in %lld Kbyte records\n",
		port,kilobytes64,reclen/1024);
#endif

	if(fflag)		/* Each child has a file name to write */
	  for(xx=0;xx<num_child;xx++)
	    filearray[xx] = filename;
	myid = (long long)getpid();

	/* rags: skip writer test */
	if(include_tflag)
		if(!(include_mask & 1))
			goto next0;

	if(!use_thread)
	{
	   for(xx = 0; xx< num_child ; xx++){	/* Create the children */
		chid=xx;
		childids[xx] = fork();
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
#ifdef __LP64__
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

#ifdef __LP64__
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
		for(i=0;i<num_child; i++){
			child_stat = (struct child_stats *)&shmaddr[i];	
			while(child_stat->flag==0)
				Poll((long long)1);
		}
		for(i=0;i<num_child; i++)	/* Start all children going */
		{
			if(delay_start!=0)
				Poll((long long)delay_start);
						/* State "go" */
			child_stat = (struct child_stats *)&shmaddr[i];	
			child_stat->flag=2;
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
			if(use_thread)
			{
				thread_join(childids[i],(void *)&pstatus);
			}
			else
			{
		   		wait(0);
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
	printf("\nJoin time %10.2f\n",jtime);
#endif
	
	total_kilos=0;
	ptotal=0;
	printf("\n");
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
	}

	for(xyz=0;xyz<num_child;xyz++){
		child_stat = (struct child_stats *) &shmaddr[xyz];
		child_stat->flag = 0; /* Start children at state 0 */
	}
	store_dvalue(total_kilos);
#ifdef NO_PRINT_LLD
	printf("\n\n\tChildren see throughput for %2ld initial writers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	printf("\tParent sees throughput for %2ld initial writers \t= %10.2f %s/sec\n",num_child,((double)(ptotal)/total_time),unit);
#else
	printf("\n\n\tChildren see throughput for %2lld initial writers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	printf("\tParent sees throughput for %2lld initial writers \t= %10.2f %s/sec\n",num_child,((double)(ptotal)/total_time),unit);
#endif
	printf("\tMin throughput per %s \t\t\t= %10.2f %s/sec \n", port,min_throughput,unit);
	printf("\tMax throughput per %s \t\t\t= %10.2f %s/sec\n", port,max_throughput,unit);
	printf("\tMin xfer \t\t\t\t\t= %10.2f %s\n\n", min_xfer,unit);
	if(Cflag)
		for(xyz=0;xyz<num_child;xyz++){
			child_stat = (struct child_stats *) &shmaddr[xyz];
				printf("\t\tChild xfer count \t= %10.2f %s/sec\n", child_stat->actual,unit);
		}
	/**********************************************************/
	/*************** End of intitial writer *******************/
	/**********************************************************/
	sync();
	sleep(2);
	*stop_flag=0;


	/**********************************************************/
	/* Re-write throughput performance test. ******************/
	/**********************************************************/
	jstarttime=0;
	total_kilos=0;
	if(!use_thread)
	{
	   for(xx = 0; xx< num_child ; xx++){
		chid=xx;
		childids[xx] = fork();
		if(childids[xx]==-1){
			printf("\nFork failed\n");
			for(xy = 0; xy< xx ; xy++){
				Kill((long long)childids[xy],(long long)SIGTERM);
			}
			exit(28);
		}
		if(childids[xx] == 0){
#ifdef __LP64__
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
#ifdef __LP64__
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
		for(i=0;i<num_child; i++){
			child_stat = (struct child_stats *)&shmaddr[i];
					/* wait for children to start */
			while(child_stat->flag==0) 
				Poll((long long)1);
		}
		for(i=0;i<num_child; i++)
		{
			child_stat = (struct child_stats *)&shmaddr[i];
			child_stat->flag = 2;	/* tell children to go */
			if(delay_start!=0)
				Poll((long long)delay_start);
		}
		starttime1 = time_so_far();
		goto jump3;
	}
	
jump3:
	getout=0;
	if((long long)myid == getpid()){	/* Parent only here */
		for( i = 0; i < num_child; i++){
			child_stat=(struct child_stats *)&shmaddr[i];
			if(use_thread)
			{
				thread_join(childids[i],(void *)&pstatus);
			}
			else
			{
		   		wait(0);
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
	printf("\nJoin time %10.2f\n",jtime);
#endif
	

	total_kilos=0;
	ptotal=0;

	min_throughput=max_throughput=min_xfer=0;
	printf("\n");
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
	}

	for(xyz=0;xyz<num_child;xyz++){	/* Reset state to 0 */
		child_stat=(struct child_stats *)&shmaddr[xyz];
		child_stat->flag = 0;
	}
	store_dvalue(total_kilos);
#ifdef NO_PRINT_LLD
	printf("\tChildren see throughput for %2ld rewriters \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	printf("\tParent sees throughput for %2ld rewriters \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#else
	printf("\tChildren see throughput for %2lld rewriters \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	printf("\tParent sees throughput for %2lld rewriters \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#endif
	printf("\tMin throughput per %s \t\t\t= %10.2f %s/sec \n", port,min_throughput,unit);
	printf("\tMax throughput per %s \t\t\t= %10.2f %s/sec\n", port,max_throughput,unit);
	printf("\tMin xfer \t\t\t\t\t= %10.2f %s\n\n", min_xfer,unit);
	if(Cflag)
		for(xyz=0;xyz<num_child;xyz++){
			child_stat = (struct child_stats *) &shmaddr[xyz];
			printf("\t\tChild xfer count \t= %10.2f %s/sec\n", 
				child_stat->actual,unit);
		}
	*stop_flag=0;
	/**********************************************************/
	/*************** End of rewrite throughput ****************/
	/**********************************************************/
	sync();
	sleep(2);
next0:
	if(include_tflag)
		if(!(include_mask & 2))
			goto next1;
	/**************************************************************/
	/*** Reader throughput tests **********************************/
	/**************************************************************/
	jstarttime=0;
	total_kilos=0;
	if(!use_thread)
	{
	   for(xx = 0; xx< num_child ; xx++){
		chid=xx;
		childids[xx] = fork();
		if(childids[xx]==-1){
			printf("\nFork failed\n");
			for(xy = 0; xy< xx ; xy++){
				Kill((long long)childids[xy],(long long)SIGTERM);
			}
			exit(30);
		}
		if(childids[xx]==0){
#ifdef __LP64__
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
#ifdef __LP64__
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
		for(i=0;i<num_child; i++){ /* wait for children to start */
			child_stat=(struct child_stats *)&shmaddr[i];
			while(child_stat->flag==0)
				Poll((long long)1);
		}
		for(i=0;i<num_child; i++)
		{
			child_stat=(struct child_stats *)&shmaddr[i];
			child_stat->flag = 2; /* tell children to go */
			if(delay_start!=0)
				Poll((long long)delay_start);
		}
		starttime1 = time_so_far();
		goto jumpend;
	}
jumpend:
	getout=0;
	if(myid == (long long)getpid()){	/* Parent here */
		for( i = 0; i < num_child; i++){
			child_stat = (struct child_stats *)&shmaddr[i];
			if(use_thread)
			{
				thread_join(childids[i],(void *)&pstatus);
			}
			else
			{
				wait(0);
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
	printf("\nJoin time %10.2f\n",jtime);
#endif
	
	total_kilos=0;
	ptotal=0;
	min_throughput=max_throughput=min_xfer=0;
	printf("\n");
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
	}
	store_dvalue(total_kilos);
#ifdef NO_PRINT_LLD
	printf("\tChildren see throughput for %2ld readers \t\t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	printf("\tParent sees throughput for %2ld readers \t\t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#else
	printf("\tChildren see throughput for %2lld readers \t\t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	printf("\tParent sees throughput for %2lld readers \t\t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#endif
	printf("\tMin throughput per %s \t\t\t= %10.2f %s/sec \n", port,min_throughput,unit);
	printf("\tMax throughput per %s \t\t\t= %10.2f %s/sec\n", port,max_throughput,unit);
	printf("\tMin xfer \t\t\t\t\t= %10.2f %s\n\n", min_xfer,unit);
	if(Cflag)
		for(xyz=0;xyz<num_child;xyz++){
			child_stat = (struct child_stats *) &shmaddr[xyz];
			printf("\t\tChild xfer count \t= %10.2f %s/sec\n", child_stat->actual,unit);
		}
	/**********************************************************/
	/*************** End of readers throughput ****************/
	/**********************************************************/
	sync();
	sleep(2);

	if (no_unlink)
		goto next1;	/* rags: skip rereader */

	/**************************************************************/
	/*** ReReader throughput tests **********************************/
	/**************************************************************/
	jstarttime=0;
	*stop_flag=0;
	total_kilos=0;
	if(!use_thread)
	{
	   for(xx = 0; xx< num_child ; xx++){
		chid=xx;
		childids[xx] = fork();
		if(childids[xx]==-1){
			printf("\nFork failed\n");
			for(xy = 0; xy< xx ; xy++){
				Kill((long long)childids[xy],(long long)SIGTERM);
			}
			exit(32);
		}
		if(childids[xx]==0){
#ifdef __LP64__
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
#ifdef __LP64__
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
		for(i=0;i<num_child; i++){ /* wait for children to start */
			child_stat = (struct child_stats *)&shmaddr[i];
			while(child_stat->flag==0)
				Poll((long long)1);
		}
		for(i=0;i<num_child; i++){
			child_stat = (struct child_stats *)&shmaddr[i];
			child_stat->flag = 2;	/* tell children to go */
			if(delay_start!=0)
				Poll((long long)delay_start);
		}
		starttime1 = time_so_far();
		goto jumpend2;
	}
	
jumpend2:
	getout=0;
	if(myid == (long long)getpid()){	 /* Parent here */
		for( i = 0; i < num_child; i++){ /* wait for children to stop */
			child_stat = (struct child_stats *)&shmaddr[i];
			if(use_thread)
			{
				thread_join(childids[i],(void *)&pstatus);
			}
			else
			{
				wait(0);
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
	printf("\nJoin time %10.2f\n",jtime);
#endif
	min_throughput=max_throughput=min_xfer=0;
	total_kilos=0;
	ptotal=0;
	printf("\n");
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
	}
	store_dvalue(total_kilos);
#ifdef NO_PRINT_LLD
	printf("\tChildren see throughput for %ld re-readers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	printf("\tParent sees throughput for %ld re-readers \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#else
	printf("\tChildren see throughput for %lld re-readers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	printf("\tParent sees throughput for %lld re-readers \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#endif
	printf("\tMin throughput per %s \t\t\t= %10.2f %s/sec \n", port,min_throughput,unit);
	printf("\tMax throughput per %s \t\t\t= %10.2f %s/sec\n", port,max_throughput,unit);
	printf("\tMin xfer \t\t\t\t\t= %10.2f %s\n\n", min_xfer,unit);
	if(Cflag)
		for(xyz=0;xyz<num_child;xyz++){
			child_stat = (struct child_stats *) &shmaddr[xyz];
			printf("\t\tChild xfer count \t= %10.2f %s/sec\n", child_stat->actual,unit);
		}
	/**********************************************************/
	/*************** End of re-readers throughput ****************/
	/**********************************************************/

next1:
	if(include_tflag)
		if(!(include_mask & 8))
			goto next2;
	sync();
	sleep(2);

	/**************************************************************/
	/*** Reverse reader throughput tests **************************/
	/**************************************************************/
	jstarttime=0;
	*stop_flag=0;
	total_kilos=0;
	if(!use_thread)
	{
	   for(xx = 0; xx< num_child ; xx++){
		chid=xx;
		childids[xx] = fork();
		if(childids[xx]==-1){
			printf("\nFork failed\n");
			for(xy = 0; xy< xx ; xy++){
				Kill((long long)childids[xy],(long long)SIGTERM);
			}
			exit(34);
		}
		if(childids[xx]==0){
#ifdef __LP64__
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
#ifdef __LP64__
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
		for(i=0;i<num_child; i++){ /* wait for children to start */
			child_stat = (struct child_stats *)&shmaddr[i];
			while(child_stat->flag==0)
				Poll((long long)1);
		}
		for(i=0;i<num_child; i++){
			child_stat = (struct child_stats *)&shmaddr[i];
			child_stat->flag = 2;	/* tell children to go */
			if(delay_start!=0)
				Poll((long long)delay_start);
		}
		starttime1 = time_so_far();
	}
	
	getout=0;
	if(myid == (long long)getpid()){	 /* Parent here */
		for( i = 0; i < num_child; i++){ /* wait for children to stop */
			child_stat = (struct child_stats *)&shmaddr[i];
			if(use_thread)
			{
				thread_join(childids[i],(void *)&pstatus);
			}
			else
			{
				wait(0);
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
	printf("\nJoin time %10.2f\n",jtime);
#endif
	total_kilos=0;
	ptotal=0;
	min_throughput=max_throughput=min_xfer=0;
	printf("\n");
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
	}
	store_dvalue(total_kilos);
#ifdef NO_PRINT_LLD
	printf("\tChildren see throughput for %ld reverse readers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	printf("\tParent sees throughput for %ld reverse readers \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#else
	printf("\tChildren see throughput for %lld reverse readers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	printf("\tParent sees throughput for %lld reverse readers \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#endif
	printf("\tMin throughput per %s \t\t\t= %10.2f %s/sec \n", port,min_throughput,unit);
	printf("\tMax throughput per %s \t\t\t= %10.2f %s/sec\n", port,max_throughput,unit);
	printf("\tMin xfer \t\t\t\t\t= %10.2f %s\n\n", min_xfer,unit);
	if(Cflag)
		for(xyz=0;xyz<num_child;xyz++){
			child_stat = (struct child_stats *) &shmaddr[xyz];
			printf("\t\tChild xfer count \t= %10.2f %s/sec\n", child_stat->actual,unit);
		}

next2:
	if(include_tflag)
		if(!(include_mask & 32))
			goto next3;
	/**************************************************************/
	/*** stride reader throughput tests **************************/
	/**************************************************************/
	jstarttime=0;
	sync();
	sleep(2);
	*stop_flag=0;
	total_kilos=0;
	if(!use_thread)
	{
	   for(xx = 0; xx< num_child ; xx++){
		chid=xx;
		childids[xx] = fork();
		if(childids[xx]==-1){
			printf("\nFork failed\n");
			for(xy = 0; xy< xx ; xy++){
				Kill((long long)childids[xy],(long long)SIGTERM);
			}
			exit(36);
		}
		if(childids[xx]==0){
#ifdef __LP64__
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
#ifdef __LP64__
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
		for(i=0;i<num_child; i++){ /* wait for children to start */
			child_stat = (struct child_stats *)&shmaddr[i];
			while(child_stat->flag==0)
				Poll((long long)1);
		}
		for(i=0;i<num_child; i++){
			child_stat = (struct child_stats *)&shmaddr[i];
			child_stat->flag = 2;	/* tell children to go */
			if(delay_start!=0)
				Poll((long long)delay_start);
		}
		starttime1 = time_so_far();
	}
	
	getout=0;
	if(myid == (long long)getpid()){	 /* Parent here */
		for( i = 0; i < num_child; i++){ /* wait for children to stop */
			child_stat = (struct child_stats *)&shmaddr[i];
			if(use_thread)
			{
				thread_join(childids[i],(void *)&pstatus);
			}
			else
			{
				wait(0);
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
	printf("\nJoin time %10.2f\n",jtime);
#endif
	total_kilos=0;
	ptotal=0;
	min_throughput=max_throughput=min_xfer=0;
	printf("\n");
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
	}
	store_dvalue(total_kilos);
#ifdef NO_PRINT_LLD
	printf("\tChildren see throughput for %ld stride readers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	printf("\tParent sees throughput for %ld stride readers \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#else
	printf("\tChildren see throughput for %lld stride readers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	printf("\tParent sees throughput for %lld stride readers \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#endif
	printf("\tMin throughput per %s \t\t\t= %10.2f %s/sec \n", port,min_throughput,unit);
	printf("\tMax throughput per %s \t\t\t= %10.2f %s/sec\n", port,max_throughput,unit);
	printf("\tMin xfer \t\t\t\t\t= %10.2f %s\n\n", min_xfer,unit);
	if(Cflag)
		for(xyz=0;xyz<num_child;xyz++){
			child_stat = (struct child_stats *) &shmaddr[xyz];
			printf("\t\tChild xfer count \t= %10.2f %s/sec\n", child_stat->actual,unit);
		}

	/**************************************************************/
	/*** random reader throughput tests **************************/
	/**************************************************************/
next3:
	if(include_tflag)
		if(!(include_mask & 4))
			goto next4;
	
	jstarttime=0;
	sync();
	sleep(2);
	*stop_flag=0;
	total_kilos=0;
	if(!use_thread)
	{
	   for(xx = 0; xx< num_child ; xx++){
		chid=xx;
		childids[xx] = fork();
		if(childids[xx]==-1){
			printf("\nFork failed\n");
			for(xy = 0; xy< xx ; xy++){
				Kill((long long)childids[xy],(long long)SIGTERM);
			}
			exit(38);
		}
		if(childids[xx]==0){
#ifdef __LP64__
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
#ifdef __LP64__
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
		for(i=0;i<num_child; i++){ /* wait for children to start */
			child_stat = (struct child_stats *)&shmaddr[i];
			while(child_stat->flag==0)
				Poll((long long)1);
		}
		for(i=0;i<num_child; i++){
			child_stat = (struct child_stats *)&shmaddr[i];
			child_stat->flag = 2;	/* tell children to go */
			if(delay_start!=0)
				Poll((long long)delay_start);
		}
		starttime1 = time_so_far();
	}
	
	getout=0;
	if(myid == (long long)getpid()){	 /* Parent here */
		for( i = 0; i < num_child; i++){ /* wait for children to stop */
			child_stat = (struct child_stats *)&shmaddr[i];
			if(use_thread)
			{
				thread_join(childids[i],(void *)&pstatus);
			}
			else
			{
				wait(0);
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
	printf("\nJoin time %10.2f\n",jtime);
#endif
	total_kilos=0;
	ptotal=0;
	min_throughput=max_throughput=min_xfer=0;
	printf("\n");
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
	}
	store_dvalue(total_kilos);
#ifdef NO_PRINT_LLD
	printf("\tChildren see throughput for %ld random readers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	printf("\tParent sees throughput for %ld random readers \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#else
	printf("\tChildren see throughput for %lld random readers \t= %10.2f %s/sec\n", num_child, total_kilos,unit);
	printf("\tParent sees throughput for %lld random readers \t= %10.2f %s/sec\n", num_child, (double)(ptotal)/total_time,unit);
#endif
	printf("\tMin throughput per %s \t\t\t= %10.2f %s/sec \n", port,min_throughput,unit);
	printf("\tMax throughput per %s \t\t\t= %10.2f %s/sec\n", port,max_throughput,unit);
	printf("\tMin xfer \t\t\t\t\t= %10.2f %s\n\n", min_xfer,unit);
	if(Cflag)
		for(xyz=0;xyz<num_child;xyz++){
			child_stat = (struct child_stats *) &shmaddr[xyz];
			printf("\t\tChild xfer count \t= %10.2f %s/sec\n", child_stat->actual,unit);
		}

next4:
	if (!no_unlink) {
		for(i=0;i<num_child;i++)
		{
			unlink(dummyfile[i]);
		}
	}
	printf("\n");
	printf("\n");
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
	long long j,k;
	off64_t file_position=0;
	off64_t i;
	char *where2;
	where=(unsigned long long *)buffer;

	if(!verify)
		printf("\nOOPS You have entered verify_buffer unexpectedly !!! \n");

	if(sverify)
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
	else
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
	if(sverify)
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
	long long rsize;
	long long tsize;
	VOLATILE long long x[200];
	long long i;
	tsize = 200;
	rsize = min(reclen/cache_line_size,cache_size/cache_line_size);
#ifdef __LP64__
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
	double qtime_start,qtime_stop;
#ifdef unix
	double qtime_u_start,qtime_u_stop;
	double qtime_s_start,qtime_s_stop;
#endif
	long long i,j;
	off64_t numrecs64;
	long long Index = 0;
	long long file_flags = 0;
	unsigned long long writerate[2];
	off64_t filebytes64;
	char *maddr;
	char *wmaddr,*free_addr;
	int fd,returnval,foo;
#ifdef ASYNC_IO
	struct cache *gc=0;

#else
	long long *gc=0;
#endif

	numrecs64 = (kilo64*1024)/reclen;
	filebytes64 = numrecs64*reclen;

	if(Q_flag && (!wol_opened))
	{
		wol_opened++;
		wqfd=fopen("wol.dat","w+");
		if(wqfd==0)
		{
			printf("Unable to open wol.dat\n");
			exit(40);
		}
		rwqfd=fopen("rwol.dat","w+");
		if(rwqfd==0)
		{
			printf("Unable to open rwol.dat\n");
			exit(41);
		}
	}
	fd = 0;
	if(oflag)
		file_flags = O_RDWR|O_SYNC;
	else
		file_flags = O_RDWR;

	for( j=0; j<2; j++)
	{
		if(Uflag) /* Unmount and re-mount the mountpoint */
		{
			purge_buffer_cache();
		}
		if(j==0)
		{
#ifdef _LARGEFILE64_SOURCE 
	  		if((fd = creat64(filename, 0640))<0)
	  		{
				printf("\nCannot create temp file: %s\n", 
					filename);
				perror("creat");
				exit(42);
	  		}
#else
		  	if((fd = creat(filename, 0640))<0)
		  	{
				printf("\nCannot create temp file: %s\n", 
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
			printf("\nCannot open temp file: %s\n", 
				filename);
			perror("open");
			exit(44);
	  	}
#else
	  	if((fd = open(filename, (int)file_flags))<0)
	  	{
			printf("\nCannot open temp file: %s\n", 
				filename);
			perror("open");
			exit(45);
	  	}
#endif
		if(file_lock)
			if(lockf((int) fd, (int) 1, (off_t) 0)!=0)
				printf("File lock for write failed.\n");
		if(mmapflag)
		{
			maddr=(char *)initfile(fd,filebytes64,1,PROT_READ|PROT_WRITE);
		}
		if(mmap_mix)
		{
			if(write(fd, buffer, (size_t) 1) != 1)
			{
#ifdef NO_PRINT_LLD
			    	printf("\nError writing block %ld, fd= %d\n", (long long)0, fd);
#else
			    	printf("\nError writing block %lld, fd= %d\n", (long long)0, fd);
#endif
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
		for(i=0; i<numrecs64; i++){
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
			    if(write(fd, buffer, (size_t ) reclen) != reclen)
			    {
#ifdef NO_PRINT_LLD
			    	printf("\nError writing block %ld, fd= %d\n", i,
					 fd);
#else
			    	printf("\nError writing block %lld, fd= %d\n", i,
					 fd);
#endif
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
				fprintf(wqfd,"%10.1ld %10.0f\n",(i*reclen)/1024,(qtime_stop-qtime_start-time_res)*1000000);
				else
				fprintf(rwqfd,"%10.1ld %10.0f\n",(i*reclen)/1024,(qtime_stop-qtime_start-time_res)*1000000);
#else
				fprintf(wqfd,"%10.1lld %10.0f\n",(i*reclen)/1024,(qtime_stop-qtime_start-time_res)*1000000);
				else
				fprintf(rwqfd,"%10.1lld %10.0f\n",(i*reclen)/1024,(qtime_stop-qtime_start-time_res)*1000000);
#endif
			}
		}
#ifdef unix
		if(Q_flag)
		{
		   qtime_u_stop=utime_so_far();
		   qtime_s_stop=stime_so_far();
		   if(j==0)
			 fprintf(wqfd,"System time %10.3f User time %10.3f Real %10.3f\n",
				(qtime_s_stop-qtime_s_start)/clk_tck(),
				(qtime_u_stop-qtime_u_start)/clk_tck(),
				time_so_far()-starttime1);
		   else
			fprintf(rwqfd,"System time %10.3f User time %10.3f Real %10.3f\n",
				(qtime_s_stop-qtime_s_start)/clk_tck(),
				(qtime_u_stop-qtime_u_start)/clk_tck(),
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
			lockf((int) fd, (int) 0, (off_t) 0);
		if(include_close)
		{
			if(mmapflag)
			{
				mmap_end(maddr,(unsigned long long)numrecs64*reclen);
			}
			close(fd);
		}
		writetime[j] = (time_so_far() - starttime1)-time_res;
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
				mmap_end(maddr,(unsigned long long)numrecs64*reclen);
			}
			close(fd);
		}
	}
	if(OPS_flag || MS_flag){
	   filebytes64=filebytes64/reclen;
	}
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
	store_value((off64_t)writerate[0]);
	store_value((off64_t)writerate[1]);
#ifdef NO_PRINT_LLD
	printf("%8ld",writerate[0]);
	fflush(stdout);
	printf("%8ld",writerate[1]);
	fflush(stdout);
#else
	printf("%8lld",writerate[0]);
	fflush(stdout);
	printf("%8lld",writerate[1]);
	fflush(stdout);
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
			printf("\nCannot fdopen temp file: %s %lld\n", 
				filename,errno);
			perror("fdopen");
			exit(48);
		}
#else
#ifdef _LARGEFILE64_SOURCE
		if((stream=(FILE *)fopen64(filename,how)) == 0)
		{
#ifdef NO_PRINT_LLD
			printf("\nCannot fdopen temp file: %s %ld\n", 
				filename,errno);
#else
			printf("\nCannot fdopen temp file: %s %lld\n", 
				filename,errno);
#endif
			perror("fdopen");
			exit(49);
		}
#else
		if((stream=(FILE *)fopen(filename,how)) == 0)
		{
#ifdef NO_PRINT_LLD
			printf("\nCannot fdopen temp file: %s %ld\n", 
				filename,errno);
#else
			printf("\nCannot fdopen temp file: %s %lld\n", 
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
		for(i=0; i<numrecs64; i++){
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
		writetime[j] = (time_so_far() - starttime1)-time_res;
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
	store_value((off64_t)writerate[0]);
	store_value((off64_t)writerate[1]);
	data1[0]=writerate[0];
#ifdef NO_PRINT_LLD
	printf("%9ld",writerate[0]);
	fflush(stdout);
	printf("%9ld",writerate[1]);
	fflush(stdout);
#else
	printf("%9lld",writerate[0]);
	fflush(stdout);
	printf("%9lld",writerate[1]);
	fflush(stdout);
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

		if(Uflag) /* Unmount and re-mount the mountpoint */
		{
			purge_buffer_cache();
		}
#ifdef IRIX64
		if((stream=(FILE *)fopen(filename,"r")) == 0)
		{
			printf("\nCannot fdopen temp file: %s\n", 
				filename);
			perror("fdopen");
			exit(51);
		}
#else
#ifdef _LARGEFILE64_SOURCE
		if((stream=(FILE *)fopen64(filename,"r")) == 0)
		{
			printf("\nCannot fdopen temp file: %s\n", 
				filename);
			perror("fdopen");
			exit(52);
		}
#else
		if((stream=(FILE *)fopen(filename,"r")) == 0)
		{
			printf("\nCannot fdopen temp file: %s\n", 
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
		starttime2 = time_so_far();
		for(i=0; i<numrecs64; i++) 
		{
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
#ifdef __LP64__
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
		readtime[j] = (time_so_far() - starttime2)-time_res;
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
#ifdef NO_PRINT_LLD
	printf("%8ld",readrate[0]);
	fflush(stdout);
	printf("%9ld",readrate[1]);
	store_value((off64_t)readrate[0]);
	store_value((off64_t)readrate[1]);
	fflush(stdout);
#else
	printf("%8lld",readrate[0]);
	fflush(stdout);
	printf("%9lld",readrate[1]);
	store_value((off64_t)readrate[0]);
	store_value((off64_t)readrate[1]);
	fflush(stdout);
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
	double readtime[2];
#ifdef unix
	double qtime_u_start,qtime_u_stop;
	double qtime_s_start,qtime_s_stop;
#endif
	long long j;
	off64_t i,numrecs64;
	long long Index = 0;
	unsigned long long readrate[2];
	off64_t filebytes64;
	volatile char *buffer1;
	char *maddr;
	char *wmaddr;
	int fd;
	double qtime_start,qtime_stop;
#ifdef ASYNC_IO
	struct cache *gc=0;

#else
	long long *gc=0;
#endif
	numrecs64 = (kilo64*1024)/reclen;

	filebytes64 = numrecs64*reclen;
	fd = 0;
	if(Q_flag && (!rol_opened))
	{
		rol_opened++;
		rqfd=fopen("rol.dat","w+");
		if(rqfd==0)
		{
			printf("Unable to open rol.dat\n");
			exit(56);
		}
		rrqfd=fopen("rrol.dat","w+");
		if(rrqfd==0)
		{
			printf("Unable to open rrol.dat\n");
			exit(57);
		}
	}
	/* 
	 * begin real testing
	 */
	for( j=0; j<2; j++ )
	{

			
		if(Uflag) /* Unmount and re-mount the mountpoint */
		{
			purge_buffer_cache();
		}
#ifdef _LARGEFILE64_SOURCE
		if((fd = open64(filename, O_RDONLY))<0)
		{
			printf("\nCannot open temporary file for read\n");
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
		if((fd = open(filename, O_RDONLY))<0)
		{
			printf("\nCannot open temporary file for read\n");
			perror("open");
			exit(59);
		}
		if(file_lock)
		{
			if(lockf((int) fd, (int) 1, (off_t) 0) != 0)
				printf("File lock for read failed\n");
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
		/* 
		 *  Need to prime the instruction cache & TLB
		 */
		buffer=mainbuffer;
		if(fetchon)
			fetchit(buffer,reclen);
		if(read(fd, (void *)buffer, (size_t) 1) != 1)
		{
#ifdef __LP64__
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
		for(i=0; i<numrecs64; i++) 
		{
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
#ifdef __LP64__
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
				fprintf(rqfd,"%10.1ld %10.0f\n",(i*reclen)/1024,(qtime_stop-qtime_start-time_res)*1000000);
				else
				fprintf(rrqfd,"%10.1ld %10.0f\n",(i*reclen)/1024,(qtime_stop-qtime_start-time_res)*1000000);
#else
				fprintf(rqfd,"%10.1lld %10.0f\n",(i*reclen)/1024,(qtime_stop-qtime_start-time_res)*1000000);
				else
				fprintf(rrqfd,"%10.1lld %10.0f\n",(i*reclen)/1024,(qtime_stop-qtime_start-time_res)*1000000);
#endif
			}
		}
		if(file_lock)
			lockf((int) fd, (int) 0, (off_t) 0);
#ifdef unix
		if(Q_flag)
		{
		   qtime_u_stop=utime_so_far();
		   qtime_s_stop=stime_so_far();
		   if(j==0)
			 fprintf(rqfd,"System time %10.3f User time %10.3f Real %10.3f\n",
				(qtime_s_stop-qtime_s_start)/clk_tck(),
				(qtime_u_stop-qtime_u_start)/clk_tck(),
				time_so_far()-starttime2);
		   else
			fprintf(rrqfd,"System time %10.3f User time %10.3f Real %10.3f\n",
				(qtime_s_stop-qtime_s_start)/clk_tck(),
				(qtime_u_stop-qtime_u_start)/clk_tck(),
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
		readtime[j] = (time_so_far() - starttime2)-time_res;
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
    	}
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
	store_value((off64_t)readrate[0]);
	store_value((off64_t)readrate[1]);
#ifdef NO_PRINT_LLD
	printf("%9ld",readrate[0]);
	fflush(stdout);
	printf("%9ld",readrate[1]);
	fflush(stdout);
#else
	printf("%9lld",readrate[0]);
	fflush(stdout);
	printf("%9lld",readrate[1]);
	fflush(stdout);
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
	long long j;
	off64_t i,numrecs64;
	long long Index=0;
	int flags;
	unsigned long long randreadrate[2];
	off64_t filebytes64;
	volatile char *buffer1;
	char *wmaddr;
	char *maddr,*free_addr;
	int fd;
#ifdef ASYNC_IO
	struct cache *gc=0;
#else
	long long *gc=0;
#endif

	numrecs64 = (kilo64*1024)/reclen;
	flags = O_RDWR;
	fd=0;
	if(oflag)
		flags |= O_SYNC;
	filebytes64 = numrecs64*reclen;
	for( j=0; j<2; j++ )
	{

	     if(Uflag) /* Unmount and re-mount the mountpoint */
	     {
			purge_buffer_cache();
	     }
#ifdef _LARGEFILE64_SOURCE
	     if((fd = open64(filename, (int)flags,0640))<0){
			printf("\nCannot open temporary file for read/write\n");
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
			printf("\nCannot open temporary file for read/write\n");
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
	     starttime2 = time_so_far();
	     if ( j==0 ){
		for(i=0; i<numrecs64; i++) {
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
			  		  if(write(fd, buffer,(size_t)reclen) != reclen)
			  		  {
#ifdef NO_PRINT_LLD
						printf("\nError writing block at %ld\n",
							offset64); 
#else
						printf("\nError writing block at %lld\n",
							offset64); 
#endif
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
	     randreadtime[j] = (time_so_far() - starttime2)-time_res;
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
	store_value((off64_t)randreadrate[0]);
	store_value((off64_t)randreadrate[1]);
#ifdef NO_PRINT_LLD
	printf("%8ld",randreadrate[0]);
	fflush(stdout);
	printf("%8ld",randreadrate[1]);
	fflush(stdout);
#else
	printf("%8lld",randreadrate[0]);
	fflush(stdout);
	printf("%8lld",randreadrate[1]);
	fflush(stdout);
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
	long long j;
	off64_t i,numrecs64;
	long long Index = 0;
	unsigned long long revreadrate[2];
	off64_t filebytes64;
	int fd;
	char *maddr,*wmaddr;
	volatile char *buffer1;
#ifdef ASYNC_IO
	struct cache *gc=0;
#else
	long long *gc=0;
#endif

	numrecs64 = (kilo64*1024)/reclen;
	filebytes64 = numrecs64*reclen;
	fd = 0;
	for( j=0; j<2; j++ )
	{
		if(Uflag) /* Unmount and re-mount the mountpoint */
		{
			purge_buffer_cache();
		}
#ifdef _LARGEFILE64_SOURCE
	 	if((fd = open64(filename, (int)O_RDONLY))<0){
	 		printf("\nCannot open temporary file for read\n");
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
	 	if((fd = open(filename, O_RDONLY))<0){
	 		printf("\nCannot open temporary file for read\n");
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
		revreadtime[j] = (time_so_far() - starttime2)-time_res;
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
	store_value((off64_t)revreadrate[0]);
#ifdef NO_PRINT_LLD
	printf("%8ld",revreadrate[0]);
#else
	printf("%8lld",revreadrate[0]);
#endif
	fflush(stdout);
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
	long long i;
	off64_t numrecs64;
	long long flags;
	long long Index=0;
	unsigned long long writeinrate;
	off64_t filebytes64;
	int fd;
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
                    printf("\nCannot open temporary file %s for write.\n",dummyfile[0]);
		    perror("open");
                    exit(84);
        }
#else
        if((fd = open(dummyfile[0], (int)flags, 0640))<0)
        {
                    printf("\nCannot open temporary file %s for write.\n",dummyfile[0]);
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
	if(write(fd, buffer, (size_t) reclen) != reclen)
	{
#ifdef NO_PRINT_LLD
	    	printf("\nError writing block %ld, fd= %d\n", 0, fd);
#else
	    	printf("\nError writing block %lld, fd= %d\n", 0, fd);
#endif
		perror("write");
		signal_handler();
	}
	*/
	starttime1 = time_so_far();
	for(i=0; i<numrecs64; i++){
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
			       if(write(fd, buffer, (size_t) reclen) != reclen)
			       {
#ifdef NO_PRINT_LLD
		    		   printf("\nError writing block %ld, fd= %d\n", i, fd);
#else
		    		   printf("\nError writing block %lld, fd= %d\n", i, fd);
#endif
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
	writeintime = (time_so_far() - starttime1)-time_res;
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
	store_value((off64_t)writeinrate);
#ifdef NO_PRINT_LLD
	printf("%8ld",writeinrate);
#else
	printf("%8lld",writeinrate);
#endif
	fflush(stdout);
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
	off64_t i,xx;
	off64_t numrecs64,current_position;
	long long Index = 0;
	off64_t savepos64 = 0;
	unsigned long long strideinrate;
	off64_t filebytes64;
	long long uu;
	off64_t internal_offset = 0;
	off64_t stripewrap=0;
	int fd;
	volatile char *buffer1;
	char *maddr;
	char *wmaddr;
#ifdef ASYNC_IO
	struct cache *gc=0;
#else
	long long *gc=0;
#endif

	next64 = (off64_t)0;
	numrecs64 = (kilos64*1024)/reclen;
	filebytes64 = numrecs64*reclen;
	if(Uflag) /* Unmount and re-mount the mountpoint */
	{
		purge_buffer_cache();
	}
#ifdef _LARGEFILE64_SOURCE
        if((fd = open64(filename, (int)O_RDONLY, 0640))<0)
        {
                    printf("\nCannot open temporary file for read\n");
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
        if((fd = open(filename, O_RDONLY, 0640))<0)
        {
                    printf("\nCannot open temporary file for read\n");
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
	for(i=0; i<numrecs64; i++){
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
	strideintime = (time_so_far() - starttime1)-time_res;
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
	store_value((off64_t)strideinrate);
#ifdef NO_PRINT_LLD
	printf("%8ld",strideinrate);
#else
	printf("%8lld",strideinrate);
#endif
	fflush(stdout);
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
	for( j=0; j<2; j++)
	{
		if(Uflag) /* Unmount and re-mount the mountpoint */
		{
			purge_buffer_cache();
		}
		if( j==0 )
		{
#ifdef _LARGEFILE64_SOURCE
		  	if((fd = creat64(filename, 0640))<0)
		  	{
				printf("\nCannot create temp file: %s\n", 
					filename);
				perror("creat");
				exit(95);
		  	}
#else
		  	if((fd = creat(filename, 0640))<0)
		  	{
				printf("\nCannot create temp file: %s\n", 
					filename);
				perror("creat");
				exit(96);
		  	}
#endif
#ifdef _LARGEFILE64_SOURCE
			if((fd = open64(filename, (int)flags_here))<0)
			{
				printf("\nCannot open temp file: %s\n", 
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
				printf("\nCannot open temp file: %s\n", 
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
				printf("\nCannot open temp file: %s\n", 
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
				printf("\nCannot open temp file: %s\n", 
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
		pwritetime[j] = (time_so_far() - starttime1)-time_res;
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
	store_value((off64_t)pwriterate[0]);
	store_value((off64_t)pwriterate[1]);
#ifdef NO_PRINT_LLD
	printf("%8ld",pwriterate[0]);
	fflush(stdout);
	printf("%9ld",pwriterate[1]);
	fflush(stdout);
#else
	printf("%8lld",pwriterate[0]);
	fflush(stdout);
	printf("%9lld",pwriterate[1]);
	fflush(stdout);
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
	long long numrecs64,i;
	long long j;
	long long Index = 0;
	unsigned long long preadrate[2];
	off64_t filebytes64;
	int fd;

	numrecs64 = (kilos64*1024)/reclen;
	filebytes64 = numrecs64*reclen;
	fd = 0;
	for( j=0; j<2; j++ ) 		/* Pread and Re-Pread */
	{

		if(Uflag) /* Unmount and re-mount the mountpoint */
		{
			purge_buffer_cache();
		}
#ifdef _LARGEFILE64_SOURCE
		if((fd = open64(filename, (int)O_RDONLY))<0)
		{
			printf("\nCannot open temporary file for read\n");
			perror("open");
			exit(101);
		}
#ifdef VXFS
		if(direct_flag)
			ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#else
		if((fd = open(filename, O_RDONLY))<0)
		{
			printf("\nCannot open temporary file for read\n");
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
		preadtime[j] = (time_so_far() - starttime2)-time_res;
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
	store_value((off64_t)preadrate[0]);
	store_value((off64_t)preadrate[1]);
#ifdef NO_PRINT_LLD
	printf("%8ld",preadrate[0]);
	fflush(stdout);
	printf("%9ld",preadrate[1]);
	fflush(stdout);
#else
	printf("%8lld",preadrate[0]);
	fflush(stdout);
	printf("%9lld",preadrate[1]);
	fflush(stdout);
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
	 
	for( j=0; j<2; j++)
	{
		if(Uflag) /* Unmount and re-mount the mountpoint */
		{
			purge_buffer_cache();
		}
		if( j==0 )
		{
#ifdef _LARGEFILE64_SOURCE
		  	if((fd = creat64(filename, 0640))<0)
		  	{
				printf("\nCannot create temp file: %s\n", 
					filename);
				perror("creat");
				exit(107);
		  	}
#else
		  	if((fd = creat(filename, 0640))<0)
		  	{
				printf("\nCannot create temp file: %s\n", 
					filename);
				perror("creat");
				exit(108);
		  	}
#endif
#ifdef _LARGEFILE64_SOURCE
			if((fd = open64(filename, (int)flags_here))<0)
			{
				printf("\nCannot open temp file: %s\n", 
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
				printf("\nCannot open temp file: %s\n", 
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
				printf("\nCannot open temp file: %s\n", 
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
				printf("\nCannot open temp file: %s\n", 
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
		pwritevtime[j] = (time_so_far() - starttime1)-time_res;
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
	store_value((off64_t)pwritevrate[0]);
	store_value((off64_t)pwritevrate[1]);
#ifdef NO_PRINT_LLD
	printf("%9ld",pwritevrate[0]);
	fflush(stdout);
	printf("%10ld",pwritevrate[1]);
	fflush(stdout);
#else
	printf("%9lld",pwritevrate[0]);
	fflush(stdout);
	printf("%10lld",pwritevrate[1]);
	fflush(stdout);
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
	long long list_off[PVECMAX];
	long long numvecs,i,j,xx;
	off64_t numrecs64;
	unsigned long long preadvrate[2];
	off64_t filebytes64;
	int fd;

	numrecs64 = (kilos64*1024)/reclen;
	filebytes64 = numrecs64*reclen;
	buffer = mainbuffer;
	fd = 0;
	for( j=0; j<2; j++ )
	{
		if(Uflag) /* Unmount and re-mount the mountpoint */
		{
			purge_buffer_cache();
		}

#ifdef _LARGEFILE64_SOURCE
		if((fd = open64(filename, (int)O_RDONLY))<0)
		{
			printf("\nCannot open temporary file for preadv\n");
			perror("open");
			exit(114);
		}
#ifdef VXFS
		if(direct_flag)
			ioctl(fd,VX_SETCACHE,VX_DIRECT);
#endif
#else
		if((fd = open(filename, O_RDONLY))<0)
		{
			printf("\nCannot open temporary file for preadv\n");
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
		preadvtime[j] = (time_so_far() - starttime2)-time_res;
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
	store_value((off64_t)preadvrate[0]);
	store_value((off64_t)preadvrate[1]);
#ifdef NO_PRINT_LLD
	printf("%10ld",preadvrate[0]);
	printf("%9ld",preadvrate[1]);
	printf("\n");
	fflush(stdout);
#else
	printf("%10lld",preadvrate[0]);
	printf("%9lld",preadvrate[1]);
	printf("\n");
	fflush(stdout);
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
    	   printf(CONTROL_STRING2,
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
    	printf(CONTROL_STRING2,
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
    	   printf(CONTROL_STRING4,		/*kcollins 8-21-96*/
		" ", 				/*kcollins 8-21-96*/
		" ",				/*kcollins 8-21-96*/
		" ",				/*kcollins 8-21-96*/
		" ",				/*kcollins 8-21-96*/
		" ",				/*kcollins 8-21-96*/
		" "				/*kcollins 8-21-96*/
		);				/*kcollins 8-21-96*/
    	printf(CONTROL_STRING4,			/*kcollins 8-21-96*/
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
    	   	printf(CONTROL_STRING3,
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
    		printf(CONTROL_STRING3,
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
    	   		printf(CONTROL_STRING3,
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
    			printf(CONTROL_STRING3,
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
		printf("\nMAX_X too small\n");
		exit(118);
	}
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

	if(bif_flag)
		bif_column++;
	printf("        ");
	for(i=RECLEN_START;i<RECLEN_END;i=i*MULTIPLIER)
		counter++;

	for(i=RECLEN_START;i<=(RECLEN_END/MULTIPLIER);i=i*MULTIPLIER)
	{
		if(bif_flag)
			do_float(bif_fd,(double)(i/1024),bif_row,bif_column++);
#ifdef NO_PRINT_LLD
		printf("%c%ld%c ",042,i/1024,042);
#else
		printf("%c%lld%c ",042,i/1024,042);
#endif
	}
	if(bif_flag)
	{
		do_float(bif_fd,(double)(RECLEN_END/1024),bif_row,bif_column);
	
		bif_column=0;
		bif_row++;
	}
#ifdef NO_PRINT_LLD
	printf("%c%ld%c\n",042,(long long)RECLEN_END/1024,042);
#else
	printf("%c%lld%c\n",042,(long long)RECLEN_END/1024,042);
#endif

	current_file_size = report_array[0][0];
	if(bif_flag)
	{
		do_float(bif_fd,(double)(current_file_size),bif_row,bif_column++);
		
	}
#ifdef NO_PRINT_LLD
	printf("%c%ld%c\t",042,current_file_size,042);
#else
	printf("%c%lld%c\t",042,current_file_size,042);
#endif
	for(i=-1;i<max_y;i++){
		if(report_array[0][i+1] != current_file_size){
			printf("\n");
			current_file_size = report_array[0][i+1];
			if(bif_flag)
			{
				bif_row++;
				bif_column=0;
				do_float(bif_fd,(double)(current_file_size),bif_row,bif_column++);
			}
#ifdef NO_PRINT_LLD
			printf("%c%ld %c ",042,current_file_size,042);
#else
			printf("%c%lld %c ",042,current_file_size,042);
#endif
		}
		if(bif_flag)
			do_float(bif_fd,(double)(report_array[who][i+1]),bif_row,bif_column++);
#ifdef NO_PRINT_LLD
		printf(" %ld ",report_array[who][i+1]);
#else
		printf(" %lld ",report_array[who][i+1]);
#endif
	}
	if(bif_flag)
	{
		bif_row++;
		bif_column=0;
	}
	printf("\n");
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
	if(bif_flag)
	{
		bif_fd=create_xls(bif_filename);
		do_label(bif_fd,"Writer Report",bif_row++,bif_column);
	}
	printf("\n%cWriter report%c\n",042,042);
	dump_report(2); 
	if(bif_flag)
		do_label(bif_fd,"Re-writer Report",bif_row++,bif_column);
	printf("\n%cRe-writer report%c\n",042,042);
	dump_report(3); 
	if(bif_flag)
		do_label(bif_fd,"Reader Report",bif_row++,bif_column);
	printf("\n%cReader report%c\n",042,042);
	dump_report(4); 
	if(bif_flag)
		do_label(bif_fd,"Re-reader Report",bif_row++,bif_column);
	printf("\n%cRe-Reader report%c\n",042,042);
	dump_report(5); 
	if(!RWONLYflag) {				/*kcollins 8-21-96*/
		if(bif_flag)
			do_label(bif_fd,"Random Read Report",bif_row++,bif_column);
		printf("\n%cRandom read report%c\n",042,042);
		dump_report(6); 
		if(bif_flag)
			do_label(bif_fd,"Random Write Report",bif_row++,bif_column);
		printf("\n%cRandom write report%c\n",042,042);
		dump_report(7); 
		if(bif_flag)
			do_label(bif_fd,"Backward Read Report",bif_row++,bif_column);
		printf("\n%cBackward read report%c\n",042,042);
		dump_report(8); 
		if(bif_flag)
			do_label(bif_fd,"Record Rewrite Report",bif_row++,bif_column);
		printf("\n%cRecord rewrite report%c\n",042,042);
		dump_report(9); 
		if(bif_flag)
			do_label(bif_fd,"Stride Read Report",bif_row++,bif_column);
		printf("\n%cStride read report%c\n",042,042);
		dump_report(10); 
		if(bif_flag)
			do_label(bif_fd,"Fwrite Report",bif_row++,bif_column);
		printf("\n%cFwrite report%c\n",042,042);
		dump_report(11); 
		if(bif_flag)
			do_label(bif_fd,"Re-fwrite Report",bif_row++,bif_column);
		printf("\n%cRe-Fwrite report%c\n",042,042);
		dump_report(12); 
		if(bif_flag)
			do_label(bif_fd,"Fread Report",bif_row++,bif_column);
		printf("\n%cFread report%c\n",042,042);
		dump_report(13); 
		if(bif_flag)
			do_label(bif_fd,"Re-fread Report",bif_row++,bif_column);
		printf("\n%cRe-Fread report%c\n",042,042);
		dump_report(14); 
#ifdef HAVE_PREAD
		if(Eflag)
		{
			if(bif_flag)
				do_label(bif_fd,"Pwrite Report",bif_row++,bif_column);
			printf("\n%cPwrite report%c\n",042,042);
			dump_report(15); 
			if(bif_flag)
				do_label(bif_fd,"Re-pwrite Report",bif_row++,bif_column);
		 	printf("\n%cRe-Pwrite report%c\n",042,042);
		 	dump_report(16); 
			if(bif_flag)
				do_label(bif_fd,"Pread Report",bif_row++,bif_column);
		 	printf("\n%cPread report%c\n",042,042);
		 	dump_report(17); 
			if(bif_flag)
				do_label(bif_fd,"Re-pread Report",bif_row++,bif_column);
		 	printf("\n%cRe-Pread report%c\n",042,042);
		 	dump_report(18); 
			if(bif_flag)
				do_label(bif_fd,"Pwritev Report",bif_row++,bif_column);
 			printf("\n%cPwritev report%c\n",042,042);
 			dump_report(19); 
			if(bif_flag)
				do_label(bif_fd,"Re-pwritev Report",bif_row++,bif_column);
 			printf("\n%cRe-Pwritev report%c\n",042,042);
 			dump_report(20); 
			if(bif_flag)
				do_label(bif_fd,"Preadv Report",bif_row++,bif_column);
 			printf("\n%cPreadv report%c\n",042,042);
 			dump_report(21); 
			if(bif_flag)
				do_label(bif_fd,"Re-preadv Report",bif_row++,bif_column);
 			printf("\n%cRe-Preadv report%c\n",042,042);
 			dump_report(22); 
		}
#endif
	}					/*kcollins 8-21-96*/
	if(bif_flag)
		close_xls(bif_fd);
}

char *
alloc_mem(size)
long long size;
{
	long long size1;
	char *addr;
	long long shmid;
	int tfd;
	char *dumb;
	

	size1=max(size,page_size);
	if(!trflag)
	{
		addr=(char *)malloc((size_t)size1);
		return(addr);
	}
#ifdef SHARED_MEM
	size1=max(size,page_size);
	size1=(size1 +page_size) & ~(page_size-1);
	shmid=(long long)shmget((key_t)(IPC_PRIVATE), (size_t)size1 , (int)(IPC_CREAT|0666));
        if(shmid < (long long)0)
        {
                printf("\nUnable to get shared memory segment(shmget)\n");
#ifdef NO_PRINT_LLD
                printf("shmid = %ld, size = %ld, size1 = %d, Error %d\n",shmid,size,(size_t)size1,errno);
#else
                printf("shmid = %lld, size = %lld, size1 = %d, Error %d\n",shmid,size,(size_t)size1,errno);
#endif
                exit(119);
        }
        addr = (char *)shmat(shmid, 0, SHM_W);
#ifdef __LP64__
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

	size1=max(size,page_size);
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
#if defined(SCO)
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

#ifndef Windows
#ifdef HAVE_ANSIC_C
long long max(long long one,long long two)
#else
long long max(one,two)
long long one,two;
#endif
{
	if(one > two)
		return(one);
	else
		return(two);
}
#endif

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

#ifndef Windows
#ifdef HAVE_ANSIC_C
long long min(long long num1,long long num2)
#else
long long min(num1,num2)
long long num1,num2;
#endif
{
	if(num1 >= num2)
		return num2;
	else
		return num1;
}
#endif


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
	long long tt,flags;
	int fd;
	long long recs_per_buffer;
	long long stopped,i;
	off64_t written_so_far, read_so_far, re_written_so_far,re_read_so_far;
	long long xx;
	char *dummyfile [MAXSTREAMS];           /* name of dummy file     */
	char *nbuff;
	char *maddr;
	char *wmaddr,*free_addr;
	int anwser,bind_cpu;
	off64_t filebytes64;

#ifdef ASYNC_IO
	struct cache *gc=0;

#else
	long long *gc=0;
#endif

	filebytes64 = numrecs64*reclen;
	written_so_far=read_so_far=re_written_so_far=re_read_so_far=0;
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
#ifdef _LARGEFILE64_SOURCE
	if((fd = open64(dummyfile[xx], (int)flags))<0)
	{
		printf("\nCannot open temp file: %s\n", 
			filename);
		perror("open");
		exit(125);
	}
#else
	if((fd = open(dummyfile[xx], (int)flags))<0)
	{
		printf("\nCannot open temp file: %s\n", 
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
		maddr=(char *)initfile(fd,(numrecs64*reclen),1,PROT_READ|PROT_WRITE);
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
	child_stat->flag=1; /* Tell parent child is ready to go */
	while(child_stat->flag!=2)   /* Wait for signal from parent */
		Poll((long long)1);

	written_so_far=0;
	child_stat = (struct child_stats *)&shmaddr[xx];
	child_stat->actual = 0;
	child_stat->throughput = 0;
	stopped=0;
	starttime1 = time_so_far();
	child_stat->start_time = starttime1;
	for(i=0; i<numrecs64; i++){
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
			   written_so_far=(written_so_far*1024)/reclen;
			}
			child_stat->throughput = 
			  (double)written_so_far/child_stat->throughput;
			child_stat->actual = (double)written_so_far;
			if(debug1)
			{
				printf("\nStopped by another\n");
			}
			stopped=1;
		}
		if(purge)
			purgeit(nbuff,reclen);
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
		      if(write(fd, nbuff, (size_t) reclen) != reclen)
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
				   written_so_far=(written_so_far*1024)/reclen;
				}
				child_stat->throughput = 
				  (double)written_so_far/child_stat->throughput;
				child_stat->actual = (double)written_so_far;
				if(debug1)
				{
					printf("\nStopped by another\n");
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
			perror("write");
			if (!no_unlink)
				unlink(dummyfile[xx]);
			child_stat->flag = 0;
		    	exit(127);
		      }
		    }
		}
		written_so_far+=reclen/1024;
		if(*stop_flag)
		{
			written_so_far-=reclen/1024;
		}
	}
	
#ifdef ASYNC_IO
	if(async_flag)
	{
		end_async(gc);
		gc=0;
	}
#endif
	if(!xflag)
		*stop_flag=1;
	
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
			mmap_end(maddr,(unsigned long long)numrecs64*reclen);
		close(fd);
	}
	if(!stopped){
		temp_time = time_so_far();
		child_stat->throughput = (temp_time - starttime1)-time_res;
		if(child_stat->throughput < (double).000001) 
		{
			child_stat->throughput= time_res;
			if(rec_prob < reclen)
				rec_prob = reclen;
			res_prob=1;
		}

		if(OPS_flag){
		   written_so_far=(written_so_far*1024)/reclen;
		}
		child_stat->throughput =
			(double)written_so_far/child_stat->throughput;
		child_stat->actual = (double)written_so_far;
		child_stat->stop_time = temp_time;
	}
	child_stat->flag = 3; /* Tell parent I'm done */
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
	int fd;
	long long flags = 0;
	double starttime1 = 0;
	double temp_time;
	long long recs_per_buffer;
	long long i;
	off64_t written_so_far, read_so_far, re_written_so_far,re_read_so_far=0;
	char *dummyfile [MAXSTREAMS];           /* name of dummy file     */
	char *nbuff;
	char *maddr,*free_addr;
	char *wmaddr;
	int anwser,bind_cpu;
#ifdef ASYNC_IO
	struct cache *gc=0;

#else
	long long *gc=0;
#endif

	written_so_far=read_so_far=re_written_so_far=re_read_so_far=0;
	recs_per_buffer = cache_size/reclen ;
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
#ifdef _LARGEFILE64_SOURCE
	if((fd = open64(dummyfile[xx], (int)flags))<0)
	{
#ifdef NO_PRINT_LLD
		printf("\nChild %ld\n",xx);
#else
		printf("\nChild %lld\n",xx);
#endif
		child_stat->flag = 0;
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
		child_stat->flag = 0;
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
	child_stat->flag = 1;
	while(child_stat->flag==1)	/* Wait for parent to say go */
		Poll((long long)1);
	starttime1 = time_so_far();
	child_stat->start_time=starttime1;
	for(i=0; i<numrecs64; i++){
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
			   if(write(fd, nbuff, (size_t) reclen) != reclen)
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
				perror("write");
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = 0;
				signal_handler();
			   }
			}
		}
		re_written_so_far+=reclen/1024;
		if(*stop_flag)
		{
			re_written_so_far-=reclen/1024;
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
	temp_time=time_so_far();
	child_stat=(struct child_stats *)&shmaddr[xx];
	child_stat->throughput = (temp_time - starttime1)-time_res;
	if(child_stat->throughput < (double).000001) 
	{
		child_stat->throughput= time_res;
		if(rec_prob < reclen)
			rec_prob = reclen;
		res_prob=1;
	}

	child_stat->stop_time = temp_time;
	if(OPS_flag){
	   re_written_so_far=(re_written_so_far*1024)/reclen;
	}
	child_stat->throughput = 
		(double)re_written_so_far/child_stat->throughput;
	child_stat->actual = (double)re_written_so_far;
	if(!xflag)
		*stop_flag=1;
	child_stat->flag = 0;	/* Tell parent I'm done */
	if(!include_close)
	{
		if(mmapflag)
		{
			msync(maddr,(size_t)(numrecs64*reclen),MS_SYNC);
			mmap_end(maddr,(unsigned long long)numrecs64*reclen);
		}
		else
			fsync(fd);
		close(fd);
	}
	free(dummyfile[xx]);
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
	int fd;
	long long flags = 0;
	double starttime1 = 0;
	double temp_time;
	off64_t written_so_far, read_so_far, re_written_so_far,re_read_so_far;
	long long recs_per_buffer;
	off64_t i;
	char *dummyfile[MAXSTREAMS];           /* name of dummy file     */
	char *nbuff;
	char *maddr;
	char *wmaddr;
	volatile char *buffer1;
	int anwser,bind_cpu;
#ifdef ASYNC_IO
	struct cache *gc=0;
#else
	long long *gc=0;
#endif

	written_so_far=read_so_far=re_written_so_far=re_read_so_far=0;
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
	if(fetchon)
		fetchit(nbuff,reclen);
	child_stat=(struct child_stats *)&shmaddr[xx];
	child_stat->flag = 1;
	while(child_stat->flag==1)	/* wait for parent to say go */
		Poll((long long)1);
	starttime1 = time_so_far();
	child_stat->start_time = starttime1;

	for(i=0; i<numrecs64; i++){
		if(*stop_flag)
		{
			if(debug1)
				printf("\nStopped by another 2\n");
			break;
		}
		if(purge)
			purgeit(nbuff,reclen);
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
						printf("\nStopped by another 2\n");
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
				child_stat->flag = 0;
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
				child_stat->flag = 0;
				exit(133);
			   }
		   }
		   else
		   {
			   if(verify_buffer(nbuff,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = 0;
				exit(134);
			   }
		   }
		}
		if(async_flag && no_copy_flag)
			async_release(gc);
		read_so_far+=reclen/1024;
		if(*stop_flag)
		{
			read_so_far-=reclen/1024;
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
	child_stat->throughput = (temp_time - starttime1)-time_res;
	if(child_stat->throughput < (double).000001) 
	{
		child_stat->throughput= time_res;
		if(rec_prob < reclen)
			rec_prob = reclen;
		res_prob=1;
	}

	if(OPS_flag){
	   read_so_far=(read_so_far*1024)/reclen;
	}
	child_stat->throughput = read_so_far/child_stat->throughput;
	child_stat->actual = read_so_far;
	if(!xflag)
		*stop_flag=1;
	child_stat->flag = 0; 	/* Tell parent I'm done */
	/*fsync(fd);*/
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
	long long flags = 0;
	double starttime1 = 0;
	double temp_time;
	long long recs_per_buffer;
	off64_t i;
	off64_t written_so_far, read_so_far, re_written_so_far,
		re_read_so_far;
	char *dummyfile[MAXSTREAMS];           /* name of dummy file     */
	char *maddr;
	char *wmaddr;
	volatile char *buffer1;
	int anwser,bind_cpu;
#ifdef ASYNC_IO
	struct cache *gc=0;
#else
	long long *gc=0;
#endif
	/*****************/
	/* Children only */
	/*****************/
	written_so_far=read_so_far=re_written_so_far=re_read_so_far=0;
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
		maddr=(char *)initfile(fd,(numrecs64*reclen),0,PROT_READ);
	}
	if(fetchon)
		fetchit(nbuff,reclen);
	child_stat = (struct child_stats *)&shmaddr[xx];
	child_stat->throughput = 0;
	child_stat->actual = 0;
	child_stat->flag = 1;
	while(child_stat->flag==1)	/* wait for parent to say go */
		Poll((long long)1);
	starttime1 = time_so_far();
	child_stat->start_time = starttime1;

	for(i=0; i<numrecs64; i++){
		if(*stop_flag)
		{
			if(debug1)
				printf("\nStopped by another 3\n");
			break;
		}
		if(purge)
			purgeit(nbuff,reclen);
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
			    	1LL,(numrecs64*reclen),depth);
			    else
			      async_read(gc, (long long)fd, nbuff, (i*reclen),reclen,
			    	1LL,(numrecs64*reclen),depth);
			  }
			  else
			  {
			      if(read((int)fd, (void*)nbuff, (size_t) reclen) != reclen)
			      {
				if(*stop_flag)
				{
					if(debug1)
						printf("\nStopped by another 4\n");
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
				child_stat->flag = 0;
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
				child_stat->flag = 0;
				exit(138);
			}
		   }
		   else
		   {
			if(verify_buffer(nbuff,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = 0;
				exit(139);
			}
		   }
		}
		if(async_flag && no_copy_flag)
			async_release(gc);
		re_read_so_far+=reclen/1024;
		if(*stop_flag)
		{
			re_read_so_far-=reclen/1024;
		}
	}
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
	child_stat->throughput = (temp_time - starttime1)-time_res;
	if(child_stat->throughput < (double).000001) 
	{
		child_stat->throughput= time_res;
		if(rec_prob < reclen)
			rec_prob = reclen;
		res_prob=1;
	}

	if(OPS_flag){
	   re_read_so_far=(re_read_so_far*1024)/reclen;
	}
	child_stat->throughput = re_read_so_far/child_stat->throughput;
	child_stat->actual = re_read_so_far;
	if(!xflag)
		*stop_flag=1;
	child_stat->flag = 0;	/* Tell parent I'm done */
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
	double starttime2 = 0;
	double temp_time;
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
	child_stat = (struct child_stats *)&shmaddr[xx];
	child_stat->throughput = 0;
	child_stat->actual = 0;
	child_stat->flag = 1;
        while(child_stat->flag==1)      /* wait for parent to say go */
                Poll((long long)1);
	starttime2 = time_so_far();
	child_stat->start_time = starttime2;

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
	for(i=0; i<numrecs64; i++) 
	{
		if(*stop_flag)
		{
			if(debug1)
				printf("\nStopped by another 3\n");
			break;
		}
		if(purge)
			purgeit(nbuff,reclen);
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
						printf("\nStopped by another 4\n");
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
				child_stat->flag = 0;
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
				child_stat->flag = 0;
				exit(145);
			}
		   }
		   else
		   {
			if(verify_buffer(nbuff,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = 0;
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
	child_stat->throughput = (temp_time - starttime2)-time_res;
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
		*stop_flag=1;
	child_stat->flag = 0;	/* Tell parent I'm done */
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
	long long tt;
	int fd;
	long long flags = 0;
	double starttime2 = 0;
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
	child_stat = (struct child_stats *)&shmaddr[xx];
	child_stat->throughput = 0;
	child_stat->actual = 0;
	child_stat->flag = 1;
        while(child_stat->flag==1)      /* wait for parent to say go */
                Poll((long long)1);
	starttime2 = time_so_far();
	child_stat->start_time = starttime2;
	for(i=0; i<numrecs64; i++){
		if(*stop_flag)
		{
			if(debug1)
				printf("\nStopped by another 3\n");
			break;
		}
		if(purge)
			purgeit(nbuff,reclen);
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
						printf("\nStopped by another 4\n");
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
				child_stat->flag = 0;
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
				child_stat->flag = 0;
				exit(150);
			}
		   }
		   else
		   {
			if(verify_buffer(nbuff,reclen,(off64_t)savepos64,reclen,(long long)pattern,sverify)){
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = 0;
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
	child_stat->throughput = (temp_time - starttime2)-time_res;
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
		*stop_flag=1;
	child_stat->flag = 0;	/* Tell parent I'm done */
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
	int fd;
	long long flags = 0;
	double starttime1 = 0;
	double temp_time;
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
	child_stat=(struct child_stats *)&shmaddr[xx];
	child_stat->flag = 1;
	while(child_stat->flag==1)	/* wait for parent to say go */
		Poll((long long)1);
	starttime1 = time_so_far();
	child_stat->start_time = starttime1;

#ifdef bsd4_2
        srand();
#else
#ifdef Windows
        srand();
#else
	srand48(0);
#endif
#endif
	for(i=0; i<numrecs64; i++) {
		if(*stop_flag)
		{
			if(debug1)
				printf("\nStopped by another 2\n");
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
						printf("\nStopped by another 2\n");
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
				child_stat->flag = 0;
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
				child_stat->flag = 0;
				exit(161);
			}
		   }
		   else
		   {
			if(verify_buffer(nbuff,reclen,(off64_t)i,reclen,(long long)pattern,sverify)){
				if (!no_unlink)
					unlink(dummyfile[xx]);
				child_stat->flag = 0;
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
	child_stat->throughput = (temp_time - starttime1)-time_res;
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
		*stop_flag=1;
	child_stat->flag = 0; 	/* Tell parent I'm done */
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
#ifdef __LP64__
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
	printf("\n\"Throughput report Y-axis is type of test X-axis is number of %s\"\n",port);
	if(bif_flag)
	{
		bif_fd=create_xls(bif_filename);
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
	printf("\"Record size = %ld Kbytes \"\n",reclen/1024);
#else
	printf("\"Record size = %lld Kbytes \"\n",reclen/1024);
#endif
	printf("\"Output is in %s\"\n\n",label);
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
	for(i=0;i<x;i++)
	{
		printf("\"%15s \"",throughput_tests[i]);
		if(bif_flag)
		{
			sprintf(print_str,"%15s ",throughput_tests[i]);
			do_label(bif_fd,print_str,bif_row,bif_column++);
			bif_column++;
		}
		for(j=0;j<=y;j++)
		{
			if(bif_flag)
			{
				do_float(bif_fd,(double)report_darray[i][j],bif_row,bif_column++);
			}
			printf("%10.2f ",report_darray[i][j]);
		}
		printf("\n\n");
		if(bif_flag)
		{
			bif_column=0;
			bif_row++;
			bif_row++;
		}
	}
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
		printf("\nMAX_X too small\n");
		exit(164);
	}
}

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

/* 	
	This is an interesting function. How much data to 
	copy is a very good question. Here we are using mmap to
	perform I/O. If the benchmark touches every byte then
	this will include a bcopy of the mmap area into the
	users buffer. This is represenative of an application
	that reads and touches every byte that it read. If
	the benchmark reduces the work to touching only
	a long per page then the numbers go up but it
	does not reflect the application to well. For now
	the best assumption is to believe that the application
	will indeed touch every byte.
*/
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

#ifdef unix
static double
clk_tck()		/* Get the clocks per tick for times */
{
	return((double)sysconf(_SC_CLK_TCK));
}

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
#ifdef HAVE_ANSIC_C
static double
stime_so_far(void)  	/* Return user time in ticks as double */
#else
static double
stime_so_far()
#endif
{
  	struct tms tp;

  	times(&tp);
  	return ((double) (tp.tms_stime));
}
#endif
