/*
 * Maintainer: Don Capps
 * 5/22/2005
 *
 * A long time ago I ran across this simple benchmark.
 * It simply tests how fast can a system create and
 * destroy files.
 *
 * Usage:  fileop X
 *
 * X is a force factor. The total number of files will
 *   be X * X * X   ( X ^ 3 )
 *   The structure of the file tree is:
 *   X number of Level 1 directorys, with X number of
 *   level 2 directories, with X number of files in each
 *   of the level 2 directories.
 *
 *   Example:  fileop 2
 *
 *           dir_1                        dir_2
 *          /     \                      /     \
 *    sdir_1       sdir_2          sdir_1       sdir_2
 *    /     \     /     \          /     \      /     \
 * file_1 file_2 file_1 file_2   file_1 file_2 file_1 file_2
 *
 * Each file will be created, and then 1 byte is written to the file.
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>


int x;
double starttime,endtime,speed;

static double time_so_far(void);
void file_create(int);
void file_stat(int);
void file_access(int);
void file_readdir(int);
void file_delete(int);
void file_link(int);
void file_unlink(int);
void splash(void);
void usage(void);

#define THISVERSION "        $Revision: 1.9 $"
/*#define NULL 0*/

char version[]=THISVERSION;

int main(int argc, char **argv)
{
	if(argc == 2)
	{
		x=atoi(argv[1]);
	}
	else
	{
		usage();
		exit(1);
	}
	splash();
	/*
	 * Create test 
	 */
	starttime=time_so_far();
	file_create(x);
	endtime=time_so_far();
	speed=endtime-starttime;
	printf("create:  Files = %9d Total Time = %7.3f seconds\n",(x*x*x),speed);
	printf("         Creates/sec = %12.2f\n\n",(x*x*x)/speed);

	/*
	 * Stat test 
	 */
	starttime=time_so_far();
	file_stat(x);
	endtime=time_so_far();
	speed=endtime-starttime;
	printf("stat:    Files = %9d Total Time = %7.3f seconds\n",(x*x*x),speed);
	printf("         Stats/sec   = %12.2f\n\n",(x*x*x)/speed);

	/*
	 * Access test 
	 */
	starttime=time_so_far();
	file_access(x);
	endtime=time_so_far();
	speed=endtime-starttime;
	printf("access:  Files = %9d Total Time = %7.3f seconds\n",(x*x*x),speed);
	printf("         Access/sec  = %12.2f\n\n",(x*x*x)/speed);
	/*
	 * readdir test 
	 */
	starttime=time_so_far();
	file_readdir(x);
	endtime=time_so_far();
	speed=endtime-starttime;
	printf("readdir: Dirs  = %9d Total Time = %7.3f seconds\n",(x*x),speed);
	printf("         Readdir/sec = %12.2f\n\n",(x*x)/speed);
	/*
	 * link test 
	 */
	starttime=time_so_far();
	file_link(x);
	endtime=time_so_far();
	speed=endtime-starttime;
	printf("link:    Link  = %9d Total Time = %7.3f seconds\n",(x*x*x),speed);
	printf("         Links/sec   = %12.2f\n\n",(x*x*x)/speed);
	/*
	 * unlink test 
	 */
	starttime=time_so_far();
	file_unlink(x);
	endtime=time_so_far();
	speed=endtime-starttime;
	printf("unlink: Unlink = %9d Total Time = %7.3f seconds\n",(x*x*x),speed);
	printf("         Unlinks/sec = %12.2f\n\n",(x*x*x)/speed);
	/*
	 * Delete test 
	 */
	starttime=time_so_far();
	file_delete(x);
	endtime=time_so_far();
	speed=endtime-starttime;
	printf("delete:  Files = %9d Total time = %7.3f seconds\n",(x*x*x),speed);
	printf("         Deletes/sec = %12.2f\n\n",(x*x*x)/speed);
	return(0);
}
void 
file_create(int x)
{
	int i,j,k;
	int fd;
	int ret;
	char buf[100];
	for(i=0;i<x;i++)
	{
	  sprintf(buf,"iozone_L1_%d",i);
	  ret=mkdir(buf,0777);
	  if(ret < 0)
	  {
	      printf("Mkdir failed\n");
	      exit(1);
	  }
	  chdir(buf);
	  for(j=0;j<x;j++)
	  {
	    sprintf(buf,"iozone_L2_%d",j);
	    ret=mkdir(buf,0777);
	    if(ret < 0)
	    {
	      printf("Mkdir failed\n");
	      exit(1);
	    }
	    chdir(buf);
	    for(k=0;k<x;k++)
	    {
	      sprintf(buf,"iozone_file%d",k);
	      fd=creat(buf,O_RDWR|0600);
	      if(fd < 0)
	      {
	        printf("Create failed\n");
	        exit(1);
	      }
	      write(fd,"a",1);
	      close(fd);
	    }
	    chdir("..");
	  }
	  chdir("..");
	}
}
void 
file_stat(int x)
{
	int i,j,k,y;
	char buf[100];
	struct stat mystat;
	for(i=0;i<x;i++)
	{
	  sprintf(buf,"iozone_L1_%d",i);
	  chdir(buf);
	  for(j=0;j<x;j++)
	  {
	    sprintf(buf,"iozone_L2_%d",j);
	    chdir(buf);
	    for(k=0;k<x;k++)
	    {
	      sprintf(buf,"iozone_file%d",k);
	      y=stat(buf,&mystat);
	      if(y < 0)
	      {
	        printf("Stat failed\n");
	        exit(1);
	      }
	    }
	    chdir("..");
	  }
	  chdir("..");
	}
}

void 
file_access(int x)
{
	int i,j,k,y;
	char buf[100];
	for(i=0;i<x;i++)
	{
	  sprintf(buf,"iozone_L1_%d",i);
	  chdir(buf);
	  for(j=0;j<x;j++)
	  {
	    sprintf(buf,"iozone_L2_%d",j);
	    chdir(buf);
	    for(k=0;k<x;k++)
	    {
	      sprintf(buf,"iozone_file%d",k);
	      y=access(buf,W_OK|F_OK);
	      if(y < 0)
	      {
	        printf("access failed\n");
		perror("what");
	        exit(1);
	      }
	    }
	    chdir("..");
	  }
	  chdir("..");
	}
}
void 
file_readdir(int x)
{
	int i,j,ret1;
	char buf[100];
	DIR *dirbuf;
	struct dirent *y;
	for(i=0;i<x;i++)
	{
	  sprintf(buf,"iozone_L1_%d",i);
	  chdir(buf);
	  for(j=0;j<x;j++)
	  {
	    sprintf(buf,"iozone_L2_%d",j);
	    chdir(buf);
	    dirbuf=opendir(".");
	    if(dirbuf==0)
	    {
		printf("opendir failed\n");
		exit(1);
	    }
	    y=readdir(dirbuf);
	    if(y == 0)
	    {
	      printf("readdir failed\n");
	      exit(1);
	    }
	    ret1=closedir(dirbuf);
	    if(ret1 < 0)
	    {
	      printf("closedir failed\n");
	      exit(1);
	    }
	    chdir("..");
	  }
	  chdir("..");
	}
}
void 
file_link(int x)
{
	int i,j,k,y;
	char buf[100];
	char bufn[100];
	for(i=0;i<x;i++)
	{
	  sprintf(buf,"iozone_L1_%d",i);
	  chdir(buf);
	  for(j=0;j<x;j++)
	  {
	    sprintf(buf,"iozone_L2_%d",j);
	    chdir(buf);
	    for(k=0;k<x;k++)
	    {
	      sprintf(buf,"iozone_file%d",k);
	      sprintf(bufn,"iozone_file%dL",k);
	      y=link(buf,bufn);
	      if(y < 0)
	      {
	        printf("Link failed\n");
	        exit(1);
	      }
	    }
	    chdir("..");
	  }
	  chdir("..");
	}
}
void 
file_unlink(int x)
{
	int i,j,k,y;
	char buf[100];
	char bufn[100];
	for(i=0;i<x;i++)
	{
	  sprintf(buf,"iozone_L1_%d",i);
	  chdir(buf);
	  for(j=0;j<x;j++)
	  {
	    sprintf(buf,"iozone_L2_%d",j);
	    chdir(buf);
	    for(k=0;k<x;k++)
	    {
	      sprintf(buf,"iozone_file%d",k);
	      sprintf(bufn,"iozone_file%dL",k);
	      y=unlink(bufn);
	      if(y < 0)
	      {
	        printf("Unlink failed\n");
	        exit(1);
	      }
	    }
	    chdir("..");
	  }
	  chdir("..");
	}
}
void
file_delete(int x)
{
	int i,j,k;
	char buf[100];
	for(i=0;i<x;i++)
	{
	  sprintf(buf,"iozone_L1_%d",i);
	  chdir(buf);
	  for(j=0;j<x;j++)
	  {
	    sprintf(buf,"iozone_L2_%d",j);
	    chdir(buf);
	    for(k=0;k<x;k++)
	    {
	      sprintf(buf,"iozone_file%d",k);
	      unlink(buf);
	    }
	    chdir("..");
	    sprintf(buf,"iozone_L2_%d",j);
	    rmdir(buf);
	  }
	  chdir("..");
	  sprintf(buf,"iozone_L1_%d",i);
	  rmdir(buf);
	}
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

void
splash(void)
{
	printf("\n");
	printf("     --------------------------------------\n");
	printf("     |              Fileop                | \n");
	printf("     | %s           | \n",version);
	printf("     |                                    | \n");
	printf("     |                by                  |\n");
	printf("     |                                    | \n");
	printf("     |             Don Capps              |\n");
	printf("     --------------------------------------\n");
	printf("\n");
}

void 
usage(void)
{
  printf("fileop X\n");
  printf("\n");
  printf("X .. Force factor. X^3 files will be created and removed.\n");
  printf("\n");
  printf("   The structure of the file tree is:\n");
  printf("   X number of Level 1 directorys, with X number of\n");
  printf("   level 2 directories, with X number of files in each\n");
  printf("   of the level 2 directories.\n");
  printf("\n");
  printf("   Example:  fileop 2\n");
  printf("\n");
  printf("           dir_1                        dir_2\n");
  printf("          /     \\                      /     \\ \n");
  printf("    sdir_1       sdir_2          sdir_1       sdir_2\n");
  printf("    /     \\     /     \\          /     \\      /     \\ \n");
  printf(" file_1 file_2 file_1 file_2   file_1 file_2 file_1 file_2\n");
  printf("\n");
  printf(" Each file will be created, and then 1 byte is written to the file.\n");
  printf("\n");
}
