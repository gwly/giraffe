/**
 * File		: FriendllyExceptionHandle.h
 * Author	: LiJia
 * Date		: 2008-02-20
 *
 * /

#pragma once


 ** 
 * @brief 启用或禁用友好的异常处理
 * 
 * 启用友好异常处理时，程序使用 SetUnhandledExceptionFilter 捕获所有
 * 未处理的异常，通过 Windows DbgHelp 的相关功能，生成异常的环境转储，
 * 收集版本等信息，将其提交给服务器保存。
 *
 * @param bEnable 是否启用 
 */
void EnableFriendllyExceptionHandle(BOOL bEnable);
