#ifndef SAES_H
#define SAES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
typedef unsigned char BYTE;
typedef char HALFBYTE;	/*事实上半字节(用char型表示)与全字节(unsigned char)都是8位，
                                                但是由于没有4位的存储类型，所以用char表示半字节(仅用低四位)，与全字节进行区分*/
extern const HALFBYTE SBox[4][4];							/*S盒*/

extern const HALFBYTE SBox_Inv[4][4];						/*逆S盒*/

extern const HALFBYTE MultiplyTable[16][16];				/*乘法表*/

extern const BYTE RCon[2];/*密钥扩展中的轮常数*/
void Extend_Key(const BYTE Key[2],BYTE RoundKey[6]);/*密钥扩展*/
BYTE SubNib(BYTE Byte);/*用于密钥扩展的字节代替*/
void SubHalfByte(HALFBYTE State[2][2],const HALFBYTE Table[4][4]);
void ShiftRows(HALFBYTE State[2][2]);/*行移位*/
void MixColumns(HALFBYTE State[2][2]);/*列混淆*/
void MixColumnsInverse(HALFBYTE State[2][2]);/*列混淆求逆*/
void AddRoundKey(HALFBYTE State[2][2],const BYTE RoundKey[],BYTE RoundNum);/*轮密钥加*/
BYTE ByteRol(BYTE Byte,BYTE Times);/*在一个字节内进行循环左移的位操作*/
void EncryptBlock(const BYTE PlainText[2],const BYTE RoundKey[6],BYTE CipherText[2]);/*加密一个分组16位数据*/
void DecryptBlock(const BYTE CipherText[2],const BYTE RoundKey[6],BYTE PlainText[2]);/*解密一个分组16位数据*/
void EncryptFile(FILE *PlainFile,const BYTE Key[2],FILE *CipherFile);/*加密文件*/
void DecryptFile(FILE *CipherFile,const BYTE Key[2],FILE *PlainFile);/*解密文件*/
#endif