#ifndef SAES_H
#define SAES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
typedef unsigned char BYTE;
typedef char HALFBYTE;	/*��ʵ�ϰ��ֽ�(��char�ͱ�ʾ)��ȫ�ֽ�(unsigned char)����8λ��
                                                ��������û��4λ�Ĵ洢���ͣ�������char��ʾ���ֽ�(���õ���λ)����ȫ�ֽڽ�������*/
extern const HALFBYTE SBox[4][4];							/*S��*/

extern const HALFBYTE SBox_Inv[4][4];						/*��S��*/

extern const HALFBYTE MultiplyTable[16][16];				/*�˷���*/

extern const BYTE RCon[2];/*��Կ��չ�е��ֳ���*/
void Extend_Key(const BYTE Key[2],BYTE RoundKey[6]);/*��Կ��չ*/
BYTE SubNib(BYTE Byte);/*������Կ��չ���ֽڴ���*/
void SubHalfByte(HALFBYTE State[2][2],const HALFBYTE Table[4][4]);
void ShiftRows(HALFBYTE State[2][2]);/*����λ*/
void MixColumns(HALFBYTE State[2][2]);/*�л���*/
void MixColumnsInverse(HALFBYTE State[2][2]);/*�л�������*/
void AddRoundKey(HALFBYTE State[2][2],const BYTE RoundKey[],BYTE RoundNum);/*����Կ��*/
BYTE ByteRol(BYTE Byte,BYTE Times);/*��һ���ֽ��ڽ���ѭ�����Ƶ�λ����*/
void EncryptBlock(const BYTE PlainText[2],const BYTE RoundKey[6],BYTE CipherText[2]);/*����һ������16λ����*/
void DecryptBlock(const BYTE CipherText[2],const BYTE RoundKey[6],BYTE PlainText[2]);/*����һ������16λ����*/
void EncryptFile(FILE *PlainFile,const BYTE Key[2],FILE *CipherFile);/*�����ļ�*/
void DecryptFile(FILE *CipherFile,const BYTE Key[2],FILE *PlainFile);/*�����ļ�*/
#endif