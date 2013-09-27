#ifndef MONITOR_DECRYPT_AES_H_
#define MONITOR_DECRYPT_AES_H_

#include <iostream>
#include "decrypt.h"
#include "flags.h"
#include "aes/aes.h"


#define AES_KEY_SIZE 16
#define AES_CBC_SIZE 16

const unsigned char AES_key128[AES_KEY_SIZE] = {
                            0x63,0xFA,0xC0,0xD0,
                            0x34,0xD9,0xF7,0x93,
                            0x19,0x9E,0x9D,0x6D,
                            0xF3,0x9A,0xA8,0x16 };
const unsigned char AES_cbc128[AES_CBC_SIZE] = {
                            0x37,0x36,0x35,0x34,
                            0x33,0x32,0x31,0x20,
                            0x4E,0x6F,0x77,0x20,
                            0x69,0x73,0x20,0x74 };

class DecryptAES:public Decrypt
{
public:
    DecryptAES()
    {

    };
    virtual ~DecryptAES(){};
    void DecryptData(BYTE *pbuf, int len);
protected:
private:
};


#endif // MONITOR_DECRYPT_AES_H_
