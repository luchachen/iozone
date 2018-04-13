/*
 * Maintainer: Don Capps
 * 10/20/2004
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
int x;
double starttime,endtime,speed;

static double time_so_far(void);
void file_create(int);
void file_delete(int);
void splash(void);
void usage(void);

#define THISVERSION "        Version $Revision: 1.2 $"
#define NULL 0

char version[]=THISVERSION;

main(int argc, char **argv)
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
	starttime=time_so_far();
	file_create(x);
	endtime=time_so_far();
	speed=endtime-starttime;
	printf("File create: Files = %d Total Time = %.3f seconds\n",(x*x*x),speed);
	printf("             Creates/sec = %.3f\n\n",(x*x*x)/speed);

	starttime=time_so_far();
	file_delete(x);
	endtime=time_so_far();
	speed=endtime-starttime;
	printf("File delete: Files = %d Total time = %.3f seconds\n",(x*x*x),speed);
	printf("             Deletes/sec = %.3f\n\n",(x*x*x)/speed);
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
	      fd=creat(buf,O_RDWR);
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
