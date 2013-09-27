/********************************************************************
	created:	2012:10:22   14:31
	filename: 	pub_pfloat.h
	author:		fu.changyou
	
	purpose:	64bit��ʾ�����������е�һλ��ʾ��������
                ��2-56λ��ʾȥ��С�����ֵ����8λ��ʾ����ľ���,���2^8��С��
                �ڴ�洢����
				+++++++++++++++++++++++++++++++++++++++++++++++++++++
				+1λtag+         55λʵ����ֵ              + 8λ����+
				+++++++++++++++++++++++++++++++++++++++++++++++++++++
	attention:  1.����double��float���������⣬���PFloat���Ͳ�����double��float����
	            2.�ڵ���GetValue()��GetABSValue()��������double�������⵼�½����׼ȷ
				3.�ڵ��ó˷�*�ͳ���/���ڿ��ǵ�����56λ�ı�ʾ��Χ��
				  �����ڵ��ó˷����������������������������
				  �ڵ��ó���ʱ����ǡǡ�෴�����������������������������
				  ��˽����������
				4.�����ַ�����ʾ�ĸ���������һ���ַ�������+��-���Ż���С����
				  �磺+12345  +12345.6780   -2548.24  0.245  .25
                5.INT64��PFloat�ڴ�黥���������INT64��ֵ��PFloat����ֱ��ʹ�õȺ�=��
                  �����PFloat��ֵ��INT64������ʹ��ToInt64()�������磺
                  PFloat f1;
                  INT64  f2;
                  f1 = f2;  f2 = f1.ToInt64();��ȷ
                  f2 = f1;������Ϊû������INT64�ĵȺŲ���
                  Ҳ����˵INT64��PFloat������ʱ��INT64ֻ����Ϊ�Ҳ�����������Ϊ�����
                6.���Ҫ��һ����ʵ��INT���ʹ��ݸ�PFloat�����ַ�����
                  ��һ���ǲ����ַ���
                  �ڶ������ڹ��캯���е���PFloat(INT64 nBase, unsigned short sPrec)
                  �мǲ���ֱ�������Σ�����ᵱ��PFloat��Ӧ��64λ�ڴ��ʹ��
*********************************************************************/
#ifndef _PUB_PFLOAT_H_
#define _PUB_PFLOAT_H_

#ifndef INT64
#if defined(WIN32)
typedef unsigned __int64 UINT64;
typedef __int64 INT64;
#else
typedef unsigned long long UINT64;
typedef long long INT64;
#endif
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef COMPRESS_API
#ifdef DLL_FILE
#define COMPRESS_API _declspec(dllexport)
#elif __linux
#define  COMPRESS_API
#else
#define COMPRESS_API _declspec(dllimport)
#endif
#endif

class COMPRESS_API PFloat
{
public:
    PFloat(); //Ĭ�Ϲ��캯��
    PFloat(INT64 fValue); //����PFloat�ṹ�����INT64������Ϊ���캯������
    PFloat(const PFloat &obj); //�������캯��
    PFloat(const char* strValue);	//����һ���ַ�����ʾ�ĸ����������캯����������һ���ַ�����˵+��-����С����
    PFloat(INT64 nBase, unsigned short sPrec);  //�������nBase��sPrec

    operator double() const{return GetValue();};
    double GetValue() const; //����ʹ�ã����ܻ��о�����ʧ
    double GetABSValue() const; //����ʹ�ã����ܻ��о�����ʧ
    const char* GetStrValue(char sValue[]); //����һ���������������ֵ�������ظ��ַ���,��ȷ��sValue���㹻�Ŀռ�

    //����+��-��*��/���ܻ�����������ʹ�ã����ֵΪ288,230,376,151,711,743
    //PFloat�ļӼ��˳�
    PFloat operator+(const PFloat& f) const;
    PFloat operator-(const PFloat& f) const;
    PFloat operator*(const PFloat& f) const;
    PFloat operator/(const PFloat& f) const;

    //�ַ����ļӼ��˳�
    PFloat operator+(const char* f) const;
    PFloat operator-(const char* f) const;
    PFloat operator*(const char* f) const;
    PFloat operator/(const char* f) const;

    //Int64�ڴ��ļӼ��˳����мɱ�Int64��ָ����PFloat�ڴ���Ӧ���ڴ�ֵ��������ʵֵ
    PFloat operator+(INT64 f) const;
    PFloat operator-(INT64 f) const;
    PFloat operator*(INT64 f) const;
    PFloat operator/(INT64 f) const;

    //PFloat�ļӼ��˳�����ֵ
    const PFloat& operator+=(const PFloat& f) const;
    const PFloat& operator-=(const PFloat& f) const;
    const PFloat& operator*=(const PFloat& f) const;
    const PFloat& operator/=(const PFloat& f) const;

    //�ַ����ļӼ��˳�����ֵ
    const PFloat& operator+=(const char* f) const;
    const PFloat& operator-=(const char* f) const;
    const PFloat& operator*=(const char* f) const;
    const PFloat& operator/=(const char* f) const;

    //Int64�ڴ��ļӼ��˳�����ֵ���мɱ�Int64��ָ����PFloat�ڴ���Ӧ���ڴ�ֵ��������ʵֵ
    const PFloat& operator+=(INT64 f) const;
    const PFloat& operator-=(INT64 f) const;
    const PFloat& operator*=(INT64 f) const;
    const PFloat& operator/=(INT64 f) const;

    //���ڸ�ֵ
    const PFloat& operator=(const PFloat& f) const;
    const PFloat& operator=(const char* f)const;
    const PFloat& operator=(INT64 f)const;//�мɱ�Int64��ָ����PFloat�ڴ���Ӧ���ڴ�ֵ��������ʵֵ

    //����ж�
    bool   operator==(const PFloat& f) const;
    bool   operator==(const char* f) const;
    bool   operator==(INT64 f) const;//�мɱ�Int64��ָ����PFloat�ڴ���Ӧ���ڴ�ֵ��������ʵֵ

    //�����ж�
    bool   operator!=(const PFloat& f) const;
    bool   operator!=(const char* f) const;
    bool   operator!=(INT64 f) const;//�мɱ�Int64��ָ����PFloat�ڴ���Ӧ���ڴ�ֵ��������ʵֵ

    //�����ж�
    bool   operator>(const PFloat& f) const;
    bool   operator>(const char* f) const;
    bool   operator>(INT64 f) const;//�мɱ�Int64��ָ����PFloat�ڴ���Ӧ���ڴ�ֵ��������ʵֵ

    //С���ж�
    bool   operator<(const PFloat& f) const;
    bool   operator<(const char* f) const;
    bool   operator<(INT64 f) const;//�мɱ�Int64��ָ����PFloat�ڴ���Ӧ���ڴ�ֵ��������ʵֵ

    bool   IsZero() const {return _data._real_data.m_llBase == 0;}
    bool   IsMinus() const {return _data._real_data.m_llTag == 1;}
    short  GetPrec(){return _data._real_data.m_llPrec;}  //��ȡ���� 
	INT64  GetBase(){return IsMinus() ? ~((INT64)(_data._real_data.m_llBase))+1 : _data._real_data.m_llBase;}  
    INT64  ToInt64(){return _data._value;};  //�õ�һ����PFloat�ڴ���Ӧ��Int64ֵ
	void   SetBase(INT64 base)
	{
		if (base < 0)
		{
			_data._real_data.m_llTag = 1;
			_data._real_data.m_llBase = ~base+1;
		}
		else
		{
			_data._real_data.m_llTag = 0;
			_data._real_data.m_llBase = base;
		}
	}
	void   SetPrec(short prec){_data._real_data.m_llPrec = prec;}
    void   ChangePrec(short prec);//�ı侫�ȣ���Ӧ��Base�����������봦��ֻ�Ծ�����С�Ľ��д����Ŵ�Ĳ�����

private:
    bool   CheckValid(const char* strValue) const; //���strValue�Ƿ�λ��Ч��С����ʾ��ʽ
    int    Power10(short n) const;  //����10��n�η�
    PFloat Str2PFloat(PFloat& f, const char* str) const; //strת����PFloat

private: 
    union{
        struct{
            mutable UINT64 m_llTag:1;  //��һλ�������ű�ʾ
            mutable UINT64 m_llBase:55;  //��2-55λ��ʾ������ʽ
            mutable UINT64 m_llPrec:8;  //��56-64λ��ʾС������
        }_real_data;
        mutable INT64 _value;
    }_data;
};


#endif //_PUB_PFLOAT_H_
