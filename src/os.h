/*
** 2001 September 16
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
** This header file (together with is companion C source-code file
** "os.c") attempt to abstract the underlying operating system so that
** the SQLite library will work on both POSIX and windows systems.
*/
#ifndef _SQLITE_OS_H_
#define _SQLITE_OS_H_

/*
** Figure out if we are dealing with Unix, Windows or MacOS.
**
** N.B. MacOS means Mac Classic (or Carbon). Treat Darwin (OS X) as Unix.
**      The MacOS build is designed to use CodeWarrior (tested with v8)
*/
#ifndef OS_UNIX
# ifndef OS_WIN
#  ifndef OS_MAC
#    if defined(__MACOS__)
#      define OS_MAC 1
#      define OS_WIN 0
#      define OS_UNIX 0
#    elif defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#      define OS_MAC 0
#      define OS_WIN 1
#      define OS_UNIX 0
#    else
#      define OS_MAC 0
#      define OS_WIN 0
#      define OS_UNIX 1
#    endif
#  else
#    define OS_WIN 0
#    define OS_UNIX 0
#  endif
# else
#  define OS_MAC 0
#  define OS_UNIX 0
# endif
#else
# define OS_MAC 0
# ifndef OS_WIN
#  define OS_WIN 0
# endif
#endif

/*
** Invoke the appropriate operating-system specific header file.
*/
#if OS_UNIX
# include "os_unix.h"
#endif
#if OS_WIN
# include "os_win.h"
#endif
#if OS_MAC
# include "os_mac.h"
#endif

/*
** Temporary files are named starting with this prefix followed by 16 random
** alphanumeric characters, and no file extension. They are stored in the
** OS's standard temporary file directory, and are deleted prior to exit.
** If sqlite is being embedded in another program, you may wish to change the
** prefix to reflect your program's name, so that if your program exits
** prematurely, old temporary files can be easily identified. This can be done
** using -DTEMP_FILE_PREFIX=myprefix_ on the compiler command line.
*/
#ifndef TEMP_FILE_PREFIX
# define TEMP_FILE_PREFIX "sqlite_"
#endif

/*
** The following values may be passed as the second argument to
** sqlite3OsLock().
*/
#define NO_LOCK         0
#define SHARED_LOCK     1
#define RESERVED_LOCK   2
#define PENDING_LOCK    3
#define EXCLUSIVE_LOCK  4

/*
** Windows file locking notes:
**
** We cannot use LockFileEx() or UnlockFileEx() on Win95/98/ME because
** those functions are not available.  So we use only LockFile() and
** UnlockFile().
**
** LockFile() prevents not just writing but also reading by other processes.
** (This is a design error on the part of Windows, but there is nothing
** we can do about that.)  So the region used for locking is at the
** end of the file where it is unlikely to ever interfere with an
** actual read attempt.
**
** A SHARED_LOCK is obtained by locking a single randomly-chosen 
** byte out of a specific range of bytes. The lock byte is obtained at 
** random so two separate readers can probably access the file at the 
** same time, unless they are unlucky and choose the same lock byte.
** An EXCLUSIVE_LOCK is obtained by locking all bytes in the range.
** There can only be one writer.  A RESERVED_LOCK is obtained by locking
** a single byte of the file that is designated as the reserved lock byte.
** A PENDING_LOCK is obtained by locking a designated byte different from
** the RESERVED_LOCK byte.
**
** On WinNT/2K/XP systems, LockFileEx() and UnlockFileEx() are available,
** which means we can use reader/writer locks.  When reader/writer locks
** are used, the lock is placed on the same range of bytes that is used
** for probabilistic locking in Win95/98/ME.  Hence, the locking scheme
** will support two or more Win95 readers or two or more WinNT readers.
** But a single Win95 reader will lock out all WinNT readers and a single
** WinNT reader will lock out all other Win95 readers.
**
** The following #defines specify the range of bytes used for locking.
** SHARED_SIZE is the number of bytes available in the pool from which
** a random byte is selected for a shared lock.  The pool of bytes for
** shared locks begins at SHARED_FIRST. 
**
** These #defines are available in os.h so that Unix can use the same
** byte ranges for locking.  This leaves open the possiblity of having
** clients on win95, winNT, and unix all talking to the same shared file
** and all locking correctly.
**
** Locking in windows is manditory.  For this reason, we cannot store
** actual data in the bytes used for locking.  The pager never allocates
** the pages involved in locking therefore.
*/
#define SHARED_SIZE       10238
#define SHARED_FIRST      (0x3fffffff - (SHARED_SIZE - 1))
#define RESERVED_BYTE     (SHARED_FIRST - 1)
#define PENDING_BYTE      (RESERVED_BYTE - 1)


int sqlite3OsDelete(const char*);
int sqlite3OsFileExists(const char*);
int sqliteOsFileRename(const char*, const char*);
int sqlite3OsOpenReadWrite(const char*, OsFile*, int*);
int sqlite3OsOpenExclusive(const char*, OsFile*, int);
int sqlite3OsOpenReadOnly(const char*, OsFile*);
int sqlite3OsOpenDirectory(const char*, OsFile*);
int sqlite3OsTempFileName(char*);
int sqlite3OsClose(OsFile*);
int sqlite3OsRead(OsFile*, void*, int amt);
int sqlite3OsWrite(OsFile*, const void*, int amt);
int sqlite3OsSeek(OsFile*, off_t offset);
int sqlite3OsSync(OsFile*);
int sqlite3OsTruncate(OsFile*, off_t size);
int sqlite3OsFileSize(OsFile*, off_t *pSize);
int sqlite3OsRandomSeed(char*);
int sqlite3OsSleep(int ms);
int sqlite3OsCurrentTime(double*);
void sqlite3OsEnterMutex(void);
void sqlite3OsLeaveMutex(void);
char *sqlite3OsFullPathname(const char*);
int sqlite3OsLock(OsFile*, int);
int sqlite3OsUnlock(OsFile*, int);
int sqlite3OsCheckReservedLock(OsFile *id);

#endif /* _SQLITE_OS_H_ */
