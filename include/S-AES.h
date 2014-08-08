#ifndef SAES_H
#define SAES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <conio.h>
typedef unsigned char BYTE;
typedef char HALFBYTE;	/*??????(?char???)????(unsigned char)??8??
                                                ??????4??????????char?????(?????)?????????*/
extern const HALFBYTE SBox[4][4];							/*S?*/

extern const HALFBYTE SBox_Inv[4][4];						/*?S?*/

extern const HALFBYTE MultiplyTable[16][16];				/*???*/

extern const BYTE RCon[2];/*?????????*/
void Extend_Key(const BYTE Key[2],BYTE RoundKey[6]);/*????*/
BYTE SubNib(BYTE Byte);/*???????????*/
void SubHalfByte(HALFBYTE State[2][2],const HALFBYTE Table[4][4]);
void ShiftRows(HALFBYTE State[2][2]);/*???*/
void MixColumns(HALFBYTE State[2][2]);/*???*/
void MixColumnsInverse(HALFBYTE State[2][2]);/*?????*/
void AddRoundKey(HALFBYTE State[2][2],const BYTE RoundKey[],BYTE RoundNum);/*????*/
BYTE ByteRol(BYTE Byte,BYTE Times);/*????????????????*/
void EncryptBlock(const BYTE PlainText[2],const BYTE RoundKey[6],BYTE CipherText[2]);/*??????16???*/
void DecryptBlock(const BYTE CipherText[2],const BYTE RoundKey[6],BYTE PlainText[2]);/*??????16???*/
void EncryptFile(FILE *PlainFile,const BYTE Key[2],FILE *CipherFile);/*????*/
void DecryptFile(FILE *CipherFile,const BYTE Key[2],FILE *PlainFile);/*????*/


void EncryptFile(FILE *PlainFile,const BYTE Key[2],FILE *CipherFile)
{
        BYTE PlainText[2]={0},CipherText[2]={0},RoundKey[6]={0};
        int count=0;
        Extend_Key(Key,RoundKey);
        while (!feof(PlainFile))
        {
                if(2==(count=fread(PlainText,sizeof(BYTE),2,PlainFile)))
                {
                        EncryptBlock(PlainText,RoundKey,CipherText);
                        fwrite(CipherText,sizeof(BYTE),2,CipherFile);
                }
        }
        if (count)/*????????2??????????*/
        {
                memset(PlainText+count,'\0',1);
                EncryptBlock(PlainText,RoundKey,CipherText);
                fwrite(CipherText,sizeof(BYTE),2,CipherFile);
        }
}

#endif
