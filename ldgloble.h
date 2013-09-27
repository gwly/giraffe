#ifndef _GLOBLE_HEAD_
#define _GLOBLE_HEAD_
#ifndef _MSC_VER

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>


#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/mman.h>



#define BABY_NONE       0
#define BABY_RDWR       1
#define BABY_RD         2
#define BABY_WR         3

#define RESET_AUTO_PTR(OLD, NEW) OLD.reset( NEW.release() );

#ifndef SOCKET
typedef int SOCKET;
#endif

#ifndef BOOL
typedef int BOOL;
#endif

#ifndef TRUE
#define TRUE         1
#endif

#ifndef FALSE
#define FALSE        0
#endif

#ifndef WORD
typedef unsigned short WORD;
#endif

#ifndef DWORD
typedef unsigned int DWORD;
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef byte
typedef unsigned char byte;
#endif

#ifndef UINT
typedef unsigned int UINT;
#endif

#ifndef LPCSTR
typedef const char * LPCSTR;
#endif

#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
/*
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
*/
#define TRACE()
#define TRACE3()

#ifndef LPSTR
typedef char * LPSTR;
#endif

#define ASSERT()

#endif
#endif



