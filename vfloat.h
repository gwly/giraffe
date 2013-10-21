/********************************************************************
	created:	2013-03-07   14:31
	filename: 	vfloat.h
	author:		fu.changyou
	
	purpose:	64bit表示浮点数，其中第一位表示正负符号
                第2-61位表示去掉小数点的值，后3位表示浮点的精度,最多2^3-1个小数
                内存存储如下
				+++++++++++++++++++++++++++++++++++++++++++++++++++++
				+1位tag+         60位实际数值              + 3位精度+
				+++++++++++++++++++++++++++++++++++++++++++++++++++++
	attention:  1.由于double、float本身精度问题，因此VFloat类型不接受double和float类型
	            2.在调用GetValue()和GetABSValue()可能由于double精度问题导致结果不准确
				3.在调用乘法*和除法/由于考虑到超过56位的表示范围，
				  所以在调用乘法是先做精度运算再做两个数相乘
				  在调用除法时过程恰恰相反，是先做两个数相除再做精度运算
				  因此结果会存在误差
				4.接受字符串表示的浮点数，第一个字符可以是+、-符号或者小数点
				  如：+12345  +12345.6780   -2548.24  0.245  .25
                5.INT64和VFloat内存块互换，如果是INT64赋值给VFloat可以直接使用等号=，
                  如果是VFloat赋值给INT64，必须使用ToInt64()方法，如：
                  VFloat f1;
                  INT64  f2;
                  f1 = f2;  f2 = f1.ToInt64();正确
                  f2 = f1;错误，因为没有重载INT64的等号操作
                  也就是说INT64和VFloat互操作时，INT64只能作为右操作，不能作为左操作
                6.如果要将一个真实的INT类型传递给VFloat有两种方法：
                  第一种是采用字符串
                  第二种是在构造函数中调用VFloat(INT64 nBase, unsigned short sPrec)
                  切记不可直接用整形，否则会当作VFloat对应的64位内存块使用
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
    VFloat(); //默认构造函数
    VFloat(INT64 fValue); //按照VFloat结构保存的INT64整数作为构造函数参数
    VFloat(const VFloat &obj); //拷贝构造函数
    VFloat(const char* strValue);	//传入一个字符串表示的浮点数做构造函数参数，第一个字符可以说+、-或者小数点
    VFloat(INT64 nBase, unsigned short sPrec);  //传入参数nBase和sPrec

    operator double() const{return GetValue();};
    double GetValue() const; //谨慎使用，可能会有精度损失
    double GetABSValue() const; //谨慎使用，可能会有精度损失
    const char* GetStrValue(char sValue[]); //传入一个参数用作保存该值，并返回该字符串,请确保sValue有足够的空间

    //以下+、-、*、/可能会溢出，请谨慎使用，最大值为288,230,376,151,711,743
    //VFloat的加减乘除
    VFloat operator+(const VFloat& f) const;
    VFloat operator-(const VFloat& f) const;
    VFloat operator*(const VFloat& f) const;
    VFloat operator/(const VFloat& f) const;

    //字符串的加减乘除
    VFloat operator+(const char* f) const;
    VFloat operator-(const char* f) const;
    VFloat operator*(const char* f) const;
    VFloat operator/(const char* f) const;

    //Int64内存块的加减乘除，切忌本Int64是指的与VFloat内存块对应的内存值，并非真实值
    VFloat operator+(INT64 f) const;
    VFloat operator-(INT64 f) const;
    VFloat operator*(INT64 f) const;
    VFloat operator/(INT64 f) const;

    //VFloat的加减乘除并赋值
    const VFloat& operator+=(const VFloat& f) const;
    const VFloat& operator-=(const VFloat& f) const;
    const VFloat& operator*=(const VFloat& f) const;
    const VFloat& operator/=(const VFloat& f) const;

    //字符串的加减乘除并赋值
    const VFloat& operator+=(const char* f) const;
    const VFloat& operator-=(const char* f) const;
    const VFloat& operator*=(const char* f) const;
    const VFloat& operator/=(const char* f) const;

    //Int64内存块的加减乘除并赋值，切忌本Int64是指的与VFloat内存块对应的内存值，并非真实值
    const VFloat& operator+=(INT64 f) const;
    const VFloat& operator-=(INT64 f) const;
    const VFloat& operator*=(INT64 f) const;
    const VFloat& operator/=(INT64 f) const;

    //等于赋值
    const VFloat& operator=(const VFloat& f) const;
    const VFloat& operator=(const char* f)const;
    const VFloat& operator=(INT64 f)const;//切忌本Int64是指的与VFloat内存块对应的内存值，并非真实值

    //相等判断
    bool   operator==(const VFloat& f) const;
    bool   operator==(const char* f) const;
    bool   operator==(INT64 f) const;//切忌本Int64是指的与VFloat内存块对应的内存值，并非真实值

    //不等判断
    bool   operator!=(const VFloat& f) const;
    bool   operator!=(const char* f) const;
    bool   operator!=(INT64 f) const;//切忌本Int64是指的与VFloat内存块对应的内存值，并非真实值

    //大于判断
    bool   operator>(const VFloat& f) const;
    bool   operator>(const char* f) const;
    bool   operator>(INT64 f) const;//切忌本Int64是指的与VFloat内存块对应的内存值，并非真实值

    //小于判断
    bool   operator<(const VFloat& f) const;
    bool   operator<(const char* f) const;
    bool   operator<(INT64 f) const;//切忌本Int64是指的与VFloat内存块对应的内存值，并非真实值

    bool   IsZero() const {return _data._real_data.m_llBase == 0;}
    bool   IsMinus() const {return _data._real_data.m_llTag == 1;}
    short  GetPrec(){return _data._real_data.m_llPrec;}  //获取精度
    INT64  GetBase(){return IsMinus() ? ~((INT64)(_data._real_data.m_llBase))+1 : _data._real_data.m_llBase;}  
    INT64  ToInt64(){return _data._value;};  //得到一个与VFloat内存块对应的Int64值
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
    void   ChangePrec(short prec);//改变精度，对应的Base采用四舍五入处理，只对精度缩小的进行处理，放大的不处理

private:
    bool   CheckValid(const char* strValue) const; //检查strValue是否位有效的小数表示形式
    int    Power10(short n) const;  //计算10的n次方
    VFloat Str2VFloat(VFloat& f, const char* str) const; //str转换成VFloat

private: 
    union{
        struct{
            mutable UINT64 m_llTag:1;  //第一位作正负号表示
            mutable UINT64 m_llBase:60;  //第2-61位表示正数形式
            mutable UINT64 m_llPrec:3;  //地62-64位表示小数个数
        }_real_data;
        mutable INT64 _value;
    }_data;
};


#endif //_PUB_VFLOAT_H_
