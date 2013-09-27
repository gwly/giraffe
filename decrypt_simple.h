#ifndef MONITOR_DECRYPT_SIMPLE_H_
#define MONITOR_DECRYPT_SIMPLE_H_

#include "flags.h"
#include "structs.h"
#include "decrypt.h"

class DecryptSimple:public Decrypt
{
public:
    DecryptSimple(){};
    virtual ~DecryptSimple(){};
    void DecryptData(BYTE *pBuf, int nLen);
protected:
private:
};


#endif // MONITOR_DECRYPT_SIMPLE_H_
