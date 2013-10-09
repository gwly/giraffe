
#ifndef STEP_UTILITY_H
#define STEP_UTILITY_H

#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 4290 )
#endif

#ifdef _MSC_VER
/////////////////////////////////////////////
#include <Winsock2.h>
#include <process.h>
#include <direct.h>
#include <time.h>
#include <io.h>
typedef int socklen_t;
/////////////////////////////////////////////
#else
/////////////////////////////////////////////
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
/////////////////////////////////////////////
#endif

#include <cstring>
#include <cctype>
#include <ctime>
#include <cstdlib>
#include <cstdio>

namespace STEP
{
void string_replace( const std::string& oldValue,
                     const std::string& newValue,
                     std::string& value );

void socket_init();
int socket_createConnector( const char* address, int port );
int socket_createListener(int nPort);
bool socket_send( int s, const char* msg, int length );
void socket_close( int s );
int WaitForData(int, char, long);

tm time_gmtime( const time_t* t );
tm time_localtime( const time_t* t);

std::string file_separator();
#ifdef _MSC_VER
void file_mkdir( const char* path );
#else
void file_mkdir( const char* path );
#endif
FILE* file_fopen( const char* path, const char* mode );
void file_unlink( const char* path );
std::string file_appendpath( const std::string& path, const std::string& file );

void UTF8_2_GB2312(char*  in,  int  inLen,  char*  out,  int  outLen);


}

#ifdef _MSC_VER
typedef unsigned int (_stdcall THREAD_START_ROUTINE)(void *);
#define  THREAD_PROC unsigned int _stdcall
#else
extern "C" { typedef void * (THREAD_START_ROUTINE)(void *); }
#define THREAD_PROC void *
#endif

bool thread_spawn( THREAD_START_ROUTINE func, void* var, unsigned& thread );
bool thread_spawn( THREAD_START_ROUTINE func, void* var );
void thread_join( unsigned thread );
void thread_detach( unsigned thread );
unsigned thread_self();

void process_sleep( double s );
void * SfRealloc( void * pMem, int nNewSize);
void str_to_upper(char *string);
void str_to_lower(char *string);
void trim(char *s);
char *next_token( char *buf, char *token, char *stopchars); 
double  Power(int n);

unsigned int IP2DWORD(char* pcAddress);

void FormatFileName(char* newname, char* oldname,int nDate, int nType);
int ConvertStrTime(char* time);
int ConvertShortTime(char* time);
int ConvertStrDateTime(char* datetime, int& nDate, int& nTime);
void DelReturn(char *s); 
void mySleep(int sec);

void AtoH(char *szDest, char *szSource, int iCount);
unsigned char BtoH(char ch);

#if( _MSC_VER >= 1400 )
#define FILE_FSCANF fscanf_s
#else
#define FILE_FSCANF fscanf
#endif

#ifdef _MSC_VER
char *strsep(char **stringp, const char *delim);
INT64 String2Int64(char* str, int len);
#else
long long String2Int64(char* str, int len);
#endif

#if( _MSC_VER >= 1400 )
#define STRING_SPRINTF sprintf_s
#else
#define STRING_SPRINTF sprintf
#endif

#if (!defined(_MSC_VER) || (_MSC_VER >= 1300)) && !defined(HAVE_STLPORT)
using std::abort;
using std::sprintf;
using std::atoi;
using std::atol;
using std::atof;
using std::isdigit;
using std::strcmp;
using std::strftime;
using std::strlen;
using std::abs;
using std::labs;
using std::memcpy;
using std::memset;
using std::exit;
#endif

#endif
