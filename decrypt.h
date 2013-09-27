#ifndef MONITOR_DECRYPT_H_
#define MONITOR_DECRYPT_H_

class Decrypt
{
public:
    Decrypt(){};
    virtual ~Decrypt(){};
    virtual void DecryptData(unsigned char *pbuf, int len) = 0;
protected:
private:
};

#endif // MONITOR_DECRYPT_H_
