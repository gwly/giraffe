// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__BF5C875E_BE09_411C_B974_2A13E5FFA223__INCLUDED_)
#define AFX_STDAFX_H__BF5C875E_BE09_411C_B974_2A13E5FFA223__INCLUDED_

#ifdef _MSC_VER
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _WIN32_DCOM

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers


//#include <afxwin.h>         // MFC core and standard components
//#include <afxext.h>         // MFC extensions
//#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
//#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>			// MFC support for Windows Common Controls
//#endif // _AFX_NO_AFXCMN_SUPPORT

#define BABY_NONE       0
#define BABY_RDWR       1
#define BABY_RD         2
#define BABY_WR         3

#include <WINSOCK2.H>
#define RESET_AUTO_PTR(OLD, NEW) OLD = NEW;

#import <msxml3.dll> raw_interfaces_only named_guids
using namespace MSXML2;

#define chSTR2(x)	   #x
#define chSTR(x)	chSTR2(x)
#define chMSG(desc) message(__FILE__ "(" chSTR(__LINE__) "):" #desc)

#define Max(a,b)    (((a) > (b)) ? (a) : (b))
#define Min(a,b)    (((a) < (b)) ? (a) : (b))
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#else
#include "ldgloble.h"

#define Max(a,b)    (((a) > (b)) ? (a) : (b))
#define Min(a,b)    (((a) < (b)) ? (a) : (b))

#endif
#endif // !defined(AFX_STDAFX_H__BF5C875E_BE09_411C_B974_2A13E5FFA223__INCLUDED_)
