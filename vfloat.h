/********************************************************************
	created:	2013-03-07   14:31
	filename: 	vfloat.h
	author:		fu.changyou
	
	purpose:	64bit��ʾ�����������е�һλ��ʾ��������
                ��2-61λ��ʾȥ��С�����ֵ����3λ��ʾ����ľ���,���2^3-1��С��
                �ڴ�洢����
				+++++++++++++++++++++++++++++++++++++++++++++++++++++
				+1λtag+         60λʵ����ֵ              + 3λ����+
				+++++++++++++++++++++++++++++++++++++++++++++++++++++
	attention:  1.����double��float���������⣬���VFloat���Ͳ�����double��float����
	            2.�ڵ���GetValue()��GetABSValue()��������double�������⵼�½����׼ȷ
				3.�ڵ��ó˷�*�ͳ���/���ڿ��ǵ�����56λ�ı�ʾ��Χ��
				  �����ڵ��ó˷����������������������������
				  �ڵ��ó���ʱ����ǡǡ�෴�����������������������������
				  ��˽����������
				4.�����ַ�����ʾ�ĸ���������һ���ַ�������+��-���Ż���С����
				  �磺+12345  +12345.6780   -2548.24  0.245  .25
                5.INT64��VFloat�ڴ�黥���������INT64��ֵ��VFloat����ֱ��ʹ�õȺ�=��
                  �����VFloat��ֵ��INT64������ʹ��ToInt64()�������磺
                  VFloat f1;
                  INT64  f2;
                  f1 = f2;  f2 = f1.ToInt64();��ȷ
                  f2 = f1;������Ϊû������INT64�ĵȺŲ���
                  Ҳ����˵INT64��VFloat������ʱ��INT64ֻ����Ϊ�Ҳ�����������Ϊ�����
                6.���Ҫ��һ����ʵ��INT���ʹ��ݸ�VFloat�����ַ�����
                  ��һ���ǲ����ַ���
                  �ڶ������ڹ��캯���е���VFloat(INT64 nBase, unsigned short sPrec)
                  �мǲ���ֱ�������Σ�����ᵱ��VFloat��Ӧ��64λ�ڴ��ʹ��
*********************************************************************/
#ifndef _PUB_VFLOAT_H_
#define _PUB_VFLOAT_H_

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
#elif _LINUX_VERSION
#define  COMPRESS_API
#else
#define COMPRESS_API _declspec(dllimport)
#endif
#endif

class COMPRESS_API VFloat
{
public:
    VFloat(); //Ĭ�Ϲ��캯��
    VFloat(INT64 fValue); //����VFloat�ṹ�����INT64������Ϊ���캯������
    VFloat(const VFloat &obj); //�������캯��
    VFloat(const char* strValue);	//����һ���ַ�����ʾ�ĸ����������캯����������һ���ַ�����˵+��-����С����
    VFloat(INT64 nBase, unsigned short sPrec);  //�������nBase��sPrec

    operator double() const{return GetValue();};
    double GetValue() const; //����ʹ�ã����ܻ��о�����ʧ
    double GetABSValue() const; //����ʹ�ã����ܻ��о�����ʧ
    const char* GetStrValue(char sValue[]); //����һ���������������ֵ�������ظ��ַ���,��ȷ��sValue���㹻�Ŀռ�

    //����+��-��*��/���ܻ�����������ʹ�ã����ֵΪ288,230,376,151,711,743
    //VFloat�ļӼ��˳�
    VFloat operator+(const VFloat& f) const;
    VFloat operator-(const VFloat& f) const;
    VFloat operator*(const VFloat& f) const;
    VFloat operator/(const VFloat& f) const;

    //�ַ����ļӼ��˳�
    VFloat operator+(const char* f) const;
    VFloat operator-(const char* f) const;
    VFloat operator*(const char* f) const;
    VFloat operator/(const char* f) const;

    //Int64�ڴ��ļӼ��˳����мɱ�Int64��ָ����VFloat�ڴ���Ӧ���ڴ�ֵ��������ʵֵ
    VFloat operator+(INT64 f) const;
    VFloat operator-(INT64 f) const;
    VFloat operator*(INT64 f) const;
    VFloat operator/(INT64 f) const;

    //VFloat�ļӼ��˳�����ֵ
    const VFloat& operator+=(const VFloat& f) const;
    const VFloat& operator-=(const VFloat& f) const;
    const VFloat& operator*=(const VFloat& f) const;
    const VFloat& operator/=(const VFloat& f) const;

    //�ַ����ļӼ��˳�����ֵ
    const VFloat& operator+=(const char* f) const;
    const VFloat& operator-=(const char* f) const;
    const VFloat& operator*=(const char* f) const;
    const VFloat& operator/=(const char* f) const;

    //Int64�ڴ��ļӼ��˳�����ֵ���мɱ�Int64��ָ����VFloat�ڴ���Ӧ���ڴ�ֵ��������ʵֵ
    const VFloat& operator+=(INT64 f) const;
    const VFloat& operator-=(INT64 f) const;
    const VFloat& operator*=(INT64 f) const;
    const VFloat& operator/=(INT64 f) const;

    //���ڸ�ֵ
    const VFloat& operator=(const VFloat& f) const;
    const VFloat& operator=(const char* f)const;
    const VFloat& operator=(INT64 f)const;//�мɱ�Int64��ָ����VFloat�ڴ���Ӧ���ڴ�ֵ��������ʵֵ

    //����ж�
    bool   operator==(const VFloat& f) const;
    bool   operator==(const char* f) const;
    bool   operator==(INT64 f) const;//�мɱ�Int64��ָ����VFloat�ڴ���Ӧ���ڴ�ֵ��������ʵֵ

    //�����ж�
    bool   operator!=(const VFloat& f) const;
    bool   operator!=(const char* f) const;
    bool   operator!=(INT64 f) const;//�мɱ�Int64��ָ����VFloat�ڴ���Ӧ���ڴ�ֵ��������ʵֵ

    //�����ж�
    bool   operator>(const VFloat& f) const;
    bool   operator>(const char* f) const;
    bool   operator>(INT64 f) const;//�мɱ�Int64��ָ����VFloat�ڴ���Ӧ���ڴ�ֵ��������ʵֵ

    //С���ж�
    bool   operator<(const VFloat& f) const;
    bool   operator<(const char* f) const;
    bool   operator<(INT64 f) const;//�мɱ�Int64��ָ����VFloat�ڴ���Ӧ���ڴ�ֵ��������ʵֵ

    bool   IsZero() const {return _data._real_data.m_llBase == 0;}
    bool   IsMinus() const {return _data._real_data.m_llTag == 1;}
    short  GetPrec(){return _data._real_data.m_llPrec;}  //��ȡ����
    INT64  GetBase(){return IsMinus() ? ~((INT64)(_data._real_data.m_llBase))+1 : _data._real_data.m_llBase;}  
    INT64  ToInt64(){return _data._value;};  //�õ�һ����VFloat�ڴ���Ӧ��Int64ֵ
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
    VFloat Str2VFloat(VFloat& f, const char* str) const; //strת����VFloat

private: 
    union{
        struct{
            mutable UINT64 m_llTag:1;  //��һλ�������ű�ʾ
            mutable UINT64 m_llBase:60;  //��2-61λ��ʾ������ʽ
            mutable UINT64 m_llPrec:3;  //��62-64λ��ʾС������
        }_real_data;
        mutable INT64 _value;
    }_data;
};


#endif //_PUB_VFLOAT_H_
