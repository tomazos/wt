//typedef unsigned char __u_char;
//typedef unsigned short int __u_short;
//typedef unsigned int __u_int;
//typedef unsigned long int __u_long;
//
//typedef signed char __int8_t;
//typedef unsigned char __uint8_t;
//typedef signed short int __int16_t;
//typedef unsigned short int __uint16_t;
//typedef signed int __int32_t;
//typedef unsigned int __uint32_t;
//
//typedef signed long int __int64_t;
//typedef unsigned long int __uint64_t;
//
//typedef long int __quad_t;
//typedef unsigned long int __u_quad_t;
//
//typedef unsigned long int __dev_t;
//typedef unsigned int __uid_t;
//typedef unsigned int __gid_t;
//typedef unsigned long int __ino_t;
//typedef unsigned long int __ino64_t;
//typedef unsigned int __mode_t;
//typedef unsigned long int __nlink_t;
//typedef long int __off_t;
//typedef long int __off64_t;
//typedef int __pid_t;
//typedef struct
//{
//	int __val[2];
//} __fsid_t;
//typedef long int __clock_t;
//typedef unsigned long int __rlim_t;
//typedef unsigned long int __rlim64_t;
//typedef unsigned int __id_t;
//typedef long int __time_t;
//typedef unsigned int __useconds_t;
//typedef long int __suseconds_t;
//
//typedef int __daddr_t;
//typedef long int __swblk_t;
//typedef int __key_t;
//
//typedef int __clockid_t;
//
//typedef void * __timer_t;
//
//typedef long int __blksize_t;
//
//typedef long int __blkcnt_t;
//typedef long int __blkcnt64_t;
//
//typedef unsigned long int __fsblkcnt_t;
//typedef unsigned long int __fsblkcnt64_t;
//
//typedef unsigned long int __fsfilcnt_t;
//typedef unsigned long int __fsfilcnt64_t;
//
//typedef long int __ssize_t;
//
//typedef __off64_t  __loff_t;
//
//typedef __quad_t *__qaddr_t;
//typedef char *__caddr_t;
//
//typedef long int __intptr_t;
//
//typedef unsigned int __socklen_t;

struct timespec
{
	time_t tv_sec;
	long int tv_nsec;
};

struct file_stat
{
	dev_t st_dev;
	ino_t st_ino;
	nlink_t st_nlink;
	mode_t st_mode;
	uid_t st_uid;
	gid_t st_gid;
	int __pad0;
	dev_t st_rdev;
	off_t st_size;
	blksize_t st_blksize;
	blkcnt_t st_blocks;
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	long int __unused[3];
};

int main()
{
	struct stat s;
	stat("test.cpp", &s);
}
