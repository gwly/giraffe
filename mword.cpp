#include "stdafx.h"
#include "structs.h"

MWORD::MWORD(int n)
{
	m_nMul = 0;
	while(n>MAXBASE || n<-MAXBASE)
	{
		m_nMul++;
		n /= 16;
	}
	m_nBase = n;
}

MWORD::MWORD(int nBase,DWORD dwMul)
{
	m_nBase = nBase;
	m_nMul = dwMul;
}

INT64 MWORD::GetValue() const
{
	INT64 n = m_nBase;
	for(DWORD i=0;i<m_nMul;i++)
		n *= 16;
	return n;
}

INT64 MWORD::GetABSValue() const
{
	INT64 n = GetValue();
	if(n<0)
		n = -n;
	return n;
}

MWORD MWORD::operator-=(const MWORD& d)
{
	INT64 n = d;
	*this -= n;
	return *this;
}

MWORD MWORD::operator-=(const INT64 d)
{
	INT64 n = *this;
	n -= d;
	*this = n;
	return *this;
}

MWORD MWORD::operator+=(const MWORD& d)
{
	INT64 n = d;
	*this += n;
	return *this;
}

MWORD MWORD::operator+=(const INT64 d)
{
	INT64 n = *this;
	n += d;
	*this = n;
	return *this;
}

MWORD MWORD::operator*=(const MWORD& d)
{
	INT64 n = d;
	*this *= n;
	return *this;
}

MWORD MWORD::operator*=(const INT64 d)
{
	INT64 n = *this;
	n *= d;
	*this = n;
	return *this;
}

MWORD MWORD::operator/=(const MWORD& d)
{
	INT64 n = d;
	*this /= n;
	return *this;
}

MWORD MWORD::operator/=(const INT64 d)
{
	INT64 n = *this;
	n /= d;
	*this = n;
	return *this;
}

INT64 MWORD::operator+(const MWORD& d) const
{
	INT64 n = *this;
	INT64 m = d;
	return n+m;
}

INT64 MWORD::operator-(const MWORD& d) const
{
	INT64 n = *this;
	INT64 m = d;
	return n-m;
}

INT64 MWORD::operator*(const MWORD& d) const
{
	INT64 n = *this;
	INT64 m = d;
	return n*m;
}

INT64 MWORD::operator/(const MWORD& d) const
{
	INT64 n = *this;
	INT64 m = d;
	return n/m;
}

INT64 MWORD::operator+(const int d) const
{
	INT64 n = *this;
	return n+d;
}

INT64 MWORD::operator-(const int d) const
{
	INT64 n = *this;
	return n-d;
}

INT64 MWORD::operator*(const int d) const
{
	INT64 n = *this;
	return n*d;
}

INT64 MWORD::operator/(const int d) const
{
	INT64 n = *this;
	return n/d;
}

MWORD MWORD::operator=(const MWORD& d)
{
	m_nMul = d.m_nMul;
	m_nBase = d.m_nBase;
	return *this;
}

MWORD MWORD::operator=(const INT64 n)
{
	INT64 d = n;
	int nInc = 0;
	m_nMul = 0;
	while(d+nInc>MAXBASE || d+nInc<-MAXBASE)
	{
		nInc = (d%16)>=8;	//ËÄÉáÎåÈë
		d /= 16;
		m_nMul++;
		if(m_nMul==3)
			break;
	}
	m_nBase = (int)(d+nInc);
	return *this;
}

BOOL MWORD::operator==(const MWORD& d) const
{
	INT64 n = *this;
	INT64 m = d;
	return n==m;
}

BOOL MWORD::operator==(const INT64 d) const
{
	INT64 n = *this;
	return n==d;
}

BOOL MWORD::operator==(const int d) const
{
	INT64 n = *this;
	return n==d;
}

BOOL MWORD::operator!=(const MWORD& d) const
{
	INT64 n = *this;
	INT64 m = d;
	return n!=m;
}

BOOL MWORD::operator!=(const INT64 d) const
{
	INT64 n = *this;
	return n!=d;
}

BOOL MWORD::operator!=(const int d) const
{
	INT64 n = *this;
	return n!=d;
}

BOOL MWORD::operator>(int d) const
{
	INT64 n = *this;
	return n>d;
}

BOOL MWORD::operator>(MWORD d) const
{
	INT64 n = *this;
	INT64 m = d;
	return n>m;
}

BOOL MWORD::operator<(int d) const
{
	INT64 n = *this;
	return n<d;
}

BOOL MWORD::operator<(MWORD d) const
{
	INT64 n = *this;
	INT64 m = d;
	return n<m;
}

DWORD MWORD::GetRawData()
{
	return *(DWORD*)this;
}

void MWORD::SetRawData(DWORD dw)
{
	DWORD* p = (DWORD*)this;
	*p = dw;
}
