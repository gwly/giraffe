/**
 * File		: FriendllyExceptionHandle.h
 * Author	: LiJia
 * Date		: 2008-02-20
 *
 * /

#pragma once


 ** 
 * @brief ���û�����Ѻõ��쳣����
 * 
 * �����Ѻ��쳣����ʱ������ʹ�� SetUnhandledExceptionFilter ��������
 * δ������쳣��ͨ�� Windows DbgHelp ����ع��ܣ������쳣�Ļ���ת����
 * �ռ��汾����Ϣ�������ύ�����������档
 *
 * @param bEnable �Ƿ����� 
 */
void EnableFriendllyExceptionHandle(BOOL bEnable);
