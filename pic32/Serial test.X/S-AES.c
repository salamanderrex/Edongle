/*******************************************************************/
/*                          简化的AES加解密算法					   */
/******************************************************************/
#include "S-AES.h"
const HALFBYTE SBox[4][4]={							/*S盒*/
{0x9,0x4,0xA,0xB},		
{0xD,0x1,0x8,0x5},
{0x6,0x2,0x0,0x3},
{0xC,0xE,0xF,0x7}};
const HALFBYTE SBox_Inv[4][4]={						/*逆S盒*/
{0xA,0x5,0x9,0xB},		
{0x1,0x7,0x8,0xF},
{0x6,0x0,0x2,0x3},
{0xC,0x4,0xD,0xE}};
const HALFBYTE MultiplyTable[16][16]={				/*乘法表*/
{0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
{0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF},
{0x0,0x2,0x4,0x6,0x8,0xA,0xC,0xE,0x3,0x1,0x7,0x5,0xB,0x9,0xF,0xD},
{0x0,0x3,0x6,0x5,0xC,0xF,0xA,0x9,0xB,0x8,0xD,0xE,0x7,0x4,0x1,0x2},
{0x0,0x4,0x8,0xC,0x3,0x7,0xB,0xF,0x6,0x2,0xE,0xA,0x5,0x1,0xD,0x9},
{0x0,0x5,0xA,0xF,0x7,0x2,0xD,0x8,0xE,0xB,0x4,0x1,0x9,0xC,0x3,0x6},
{0x0,0x6,0xC,0xA,0xB,0xD,0x7,0x1,0x5,0x3,0x9,0xF,0xE,0x8,0x2,0x4},
{0x0,0x7,0xE,0x9,0xF,0x8,0x1,0x6,0xD,0xA,0x3,0x4,0x2,0x5,0xC,0xB},
{0x0,0x8,0x3,0xB,0x6,0xE,0x5,0xD,0xC,0x4,0xF,0x7,0xA,0x2,0x9,0x1},
{0x0,0x9,0x1,0x8,0x2,0xB,0x3,0xA,0x4,0xD,0x5,0xC,0x6,0xF,0x7,0xE},
{0x0,0xA,0x7,0xD,0xE,0x4,0x9,0x3,0xF,0x5,0x8,0x2,0x1,0xB,0x6,0xC},
{0x0,0xB,0x5,0xE,0xA,0x1,0xF,0x4,0x7,0xC,0x2,0x9,0xD,0x6,0x8,0x3},
{0x0,0xC,0xB,0x7,0x5,0x9,0xE,0x2,0xA,0x6,0x1,0xD,0xF,0x3,0x4,0x8},
{0x0,0xD,0x9,0x4,0x1,0xC,0x8,0x5,0x2,0xF,0xB,0x6,0x3,0xE,0xA,0x7},
{0x0,0xE,0xF,0x1,0xD,0x3,0x2,0xC,0x9,0x7,0x6,0x8,0x4,0xA,0xB,0x5},
{0x0,0xF,0xD,0x2,0x9,0x6,0x4,0xB,0x1,0xE,0xC,0x3,0x8,0x7,0x5,0xA},
};
const BYTE RCon[2]={0x80,0x30};/*密钥扩展中的轮常数*/
/*******************************************************************/
/*	函数功能：将16位初始密钥(两个8位的字)扩展成48位(6个字)
        参数说明：const BYTE Key[2] 16位的初始密钥
                         BYTE RoundKey[6] 得到的48位轮密钥	*/
/*******************************************************************/
void Extend_Key(const BYTE Key[2],BYTE RoundKey[6])/*密钥扩展*/
{
        BYTE i;
        memcpy(RoundKey,Key,2*sizeof(BYTE));/*密钥的前2个字即为初始密钥*/
        for (i=2;i<6;i+=2)
        {
                RoundKey[i]=RoundKey[i-2]^RCon[i/2-1]^SubNib(ByteRol(RoundKey[i-1],4));
                RoundKey[i+1]=RoundKey[i]^RoundKey[i-1];
        }
}

/*******************************************************************/
/*	函数功能：在一个字节内进行循环左移，用于密钥扩展中
        参数说明：BYTE Byte 要进行移位的字节数据
                         BYTE Times 循环左移次数		*/
/*******************************************************************/
BYTE ByteRol(BYTE Byte,BYTE Times)
{
        BYTE i,temp=0;/*保存移出的位*/
        for (i=0;i<Times;i++)
        {
                temp=(Byte&0x80)>>7;/*提取出Byte要移出的最高位，放到temp的最低位*/
                Byte<<=1;/*Byte左移一位*/
                Byte|=temp;/*移出的最高位给最低位*/
        }
        return Byte;/*返回结果*/
}

/*******************************************************************/
/*	函数功能：密钥扩展中对一个字节数据进行字节代替
        参数说明：BYTE Byte 要进行字节代替的字节数据		*/
/*******************************************************************/
BYTE SubNib(BYTE Byte)
{
        return SBox[Byte>>6][(Byte>>4)&0x03]<<4|SBox[Byte>>2&0x03][Byte&0x03];
}

/*******************************************************************/
/*	函数功能：轮密钥加函数
        参数说明：HALFBYTE State[2][2] 要进行轮密钥加的矩阵
                         const BYTE RoundKey[] 轮密钥
                         BYTE RoundNum 轮数，用以确定选择哪个轮密钥	*/
/*******************************************************************/
void AddRoundKey(HALFBYTE State[2][2],const BYTE RoundKey[],BYTE RoundNum)
{
        State[0][0]=State[0][0]^(RoundKey[RoundNum*2]>>4);
        State[1][0]=State[1][0]^(RoundKey[RoundNum*2]&0x0F);
        State[0][1]=State[0][1]^(RoundKey[RoundNum*2+1]>>4);
        State[1][1]=State[1][1]^(RoundKey[RoundNum*2+1]&0x0F);
}

/*******************************************************************/
/*	函数功能：半字节代替
        参数说明：HALFBYTE State[2][2] 要进行半字节代替的矩阵
                         const HALFBYTE Table[4][4] 查找表，当为SBox时是正变换，
                                                                                当为SBox_Inv时是逆变换	*/
/*******************************************************************/
void SubHalfByte(HALFBYTE State[2][2],const HALFBYTE Table[4][4])
{
        State[0][0]=Table[State[0][0]>>2][State[0][0]&0x03];
        State[0][1]=Table[State[0][1]>>2][State[0][1]&0x03];
        State[1][0]=Table[State[1][0]>>2][State[1][0]&0x03];
        State[1][1]=Table[State[1][1]>>2][State[1][1]&0x03];
}

/*******************************************************************/
/*	函数功能：行移位
        参数说明：HALFBYTE State[2][2] 要进行移位的矩阵	*/
/*******************************************************************/
void ShiftRows(HALFBYTE State[2][2])	/*行移位，第一行不变，第二行进行半字节的循环移位*/
{
        State[1][0]^=State[1][1];/*相当于第二行的两个数交换*/
        State[1][1]^=State[1][0];
        State[1][0]^=State[1][1];
}

/*******************************************************************/
/*	函数功能：列混淆
        参数说明：HALFBYTE State[2][2] 要进行列混淆的矩阵		*/
/*******************************************************************/
void MixColumns(HALFBYTE State[2][2])
{
        HALFBYTE temp=State[0][0];
        State[0][0]^=(MultiplyTable[4][State[1][0]]);
        State[1][0]^=(MultiplyTable[4][temp]);
        temp=State[0][1];
        State[0][1]^=(MultiplyTable[4][State[1][1]]);
        State[1][1]^=(MultiplyTable[4][temp]);
}

/*******************************************************************/
/*	函数功能：逆向列混淆
        参数说明：HALFBYTE State[2][2] 要进行逆向列混淆的矩阵		*/
/*******************************************************************/
void MixColumnsInverse(HALFBYTE State[2][2])
{
        HALFBYTE temp=State[0][0];
        State[0][0]=(MultiplyTable[9][State[0][0]]^MultiplyTable[2][State[1][0]]);
        State[1][0]=(MultiplyTable[9][State[1][0]]^MultiplyTable[2][temp]);
        temp=State[0][1];
        State[0][1]=(MultiplyTable[9][State[0][1]]^MultiplyTable[2][State[1][1]]);
        State[1][1]=(MultiplyTable[9][State[1][1]]^MultiplyTable[2][temp]);

}

/*******************************************************************/
/*	函数功能：加密一个分组（16位）明文
        参数说明：const BYTE PlainText[2] 要进行加密的明文分组
                         const BYTE RoundKey[6] 进行加密用的轮密钥
                         BYTE CipherText[2] 加密后的密文结果			*/
/*******************************************************************/
void EncryptBlock(const BYTE PlainText[2],const BYTE RoundKey[6],BYTE CipherText[2])
{
        HALFBYTE State[2][2];
        State[0][0]=PlainText[0]>>4;/*取高四位*/
        State[1][0]=PlainText[0]&0x0F;/*取低四位*/
        State[0][1]=PlainText[1]>>4;
        State[1][1]=PlainText[1]&0x0F;

        AddRoundKey(State,RoundKey,0);/*轮密钥加*/

        SubHalfByte(State,SBox);/*半字节代替*/
        ShiftRows(State);/*行移位*/
        MixColumns(State);/*列混淆*/
        AddRoundKey(State,RoundKey,1);/*轮密钥加*/

        SubHalfByte(State,SBox);/*半字节代替*/
        ShiftRows(State);/*行移位*/
        AddRoundKey(State,RoundKey,2);/*轮密钥加*/

        CipherText[0]=State[0][0]<<4|State[1][0];/*四个半字节转换为两个字节*/
        CipherText[1]=State[0][1]<<4|State[1][1];
}

/*******************************************************************/
/*	函数功能：解密一个分组（16位）密文
        参数说明：const BYTE CipherText[2] 要进行解密的密文分组
                         const BYTE RoundKey[6] 进行解密用的轮密钥
                         BYTE CipherText[2] 解密后的明文结果			*/
/*******************************************************************/
void DecryptBlock(const BYTE CipherText[2],const BYTE RoundKey[6],BYTE PlainText[2])
{
        HALFBYTE State[2][2]={0};
        State[0][0]=CipherText[0]>>4;/*取高四位*/
        State[1][0]=CipherText[0]&0x0F;/*取低四位*/
        State[0][1]=CipherText[1]>>4;
        State[1][1]=CipherText[1]&0x0F;

        AddRoundKey(State,RoundKey,2);

        ShiftRows(State);
        SubHalfByte(State,SBox_Inv);
        AddRoundKey(State,RoundKey,1);
        MixColumnsInverse(State);

        ShiftRows(State);
        SubHalfByte(State,SBox_Inv);
        AddRoundKey(State,RoundKey,0);

        PlainText[0]=State[0][0]<<4|State[1][0];
        PlainText[1]=State[0][1]<<4|State[1][1];
}

/*******************************************************************/
/*	函数功能：对文件进行加密
        参数说明：FILE *PlainFile 明文所在的文件指针
                         const BYTE Key[2] 输入的初始密钥
                         FILE *CipherFile 加密后的密文输出文件指针	*/
/*******************************************************************/




/*******************************************************************/
/*	函数功能：对文件进行解密
        参数说明：FILE *CipherFile 密文所在的文件指针
                         const BYTE Key[2] 输入的初始密钥
                         FILE *PlainFile 解密后的明文输出文件指针	*/
/*******************************************************************/
void DecryptFile(FILE *CipherFile,const BYTE Key[2],FILE *PlainFile)
{
        BYTE CipherText[2]={0},RoundKey[6]={0},PlainText[2]={0};
        Extend_Key(Key,RoundKey);
        while(!feof(CipherFile))
        {
                if (2==fread(CipherText,sizeof(BYTE),2,CipherFile))
                {
                        DecryptBlock(CipherText,RoundKey,PlainText);
                        fwrite(PlainText,sizeof(BYTE),2,PlainFile);
                }
        }
}
