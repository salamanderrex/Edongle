
#include "S-AES.h"

void main()
{
        printf("\t\t———————S-AES加解密工具———————\n\n");
        while(1)
        {
                char OpNum=0,FileName[256]="";
                BYTE PlainText[2]={0},Key[2]={0},RoundKey[6]={0},CipherText[2]={0};
                FILE *PlainFile=NULL,*CipherFile=NULL;
                printf("\n请输入操作号：1-加密一个分组 2-加密文件 3-解密一个分组 4-解密文件 5-退出\n");
                do 
                {
                        OpNum=getch();
                }while (!(OpNum>='1'&&OpNum<='5'));
                switch(OpNum)
                {
                case '1':
                        printf("\n请输入16位明文（十六进制格式，八位间以空白字符分隔）:\n");
                        fflush(stdin);
                        while (!scanf("%x%x",&PlainText[0],&PlainText[1]))
                        {
                                printf("\n您输入的明文有误！请重新输入：\n");
                                fflush(stdin);
                        }
                        printf("\n请输入16位密钥（十六进制格式，八位间以空白字符分隔）:\n");
                        fflush(stdin);
                        while (!scanf("%x%x",&Key[0],&Key[1]))
                        {
                                printf("\n您输入的密钥有误！请重新输入：\n");
                                fflush(stdin);
                        }
                        Extend_Key(Key,RoundKey);
                        EncryptBlock(PlainText,RoundKey,CipherText);
                        printf("\n加密后的密文为：%X %X\n",CipherText[0],CipherText[1]);
                        break;
                case '2':
                        printf("\n请输入要加密的文件名（包括扩展名）：\n");
                        scanf("%s",FileName);
                        while(NULL==(PlainFile=fopen(FileName,"rb")))
                        {
                                fflush(stdin);
                                printf("\n打开文件失败！请确保文件与程序在同一目录下。\n\n重新输入文件名：\n");
                                scanf("%s",FileName);
                        }
                        printf("\n请输入要输出结果的文件名（包括扩展名）：\n");
                        scanf("%s",FileName);
                        while(NULL==(CipherFile=fopen(FileName,"wb")))
                        {
                                fflush(stdin);
                                printf("\n创建文件失败！重新输入文件名：\n");
                                scanf("%s",FileName);
                        }
                        printf("\n请输入16位密钥（十六进制格式，八位间以空白字符分隔）:\n");
                        while (!scanf("%x%x",&Key[0],&Key[1]))
                        {
                                printf("\n您输入的密钥有误！请重新输入：\n");
                                fflush(stdin);
                        }
                        EncryptFile(PlainFile,Key,CipherFile);
                        printf("\n加密后的结果已输出至%s文件中。\n",FileName);
                        fclose(PlainFile);
                        fclose(CipherFile);
                        break;
                case '3':
                        printf("\n请输入16位密文（十六进制格式，八位间以空白字符分隔）:\n");
                        fflush(stdin);
                        while (!scanf("%x%x",&CipherText[0],&CipherText[1]))
                        {
                                printf("\n您输入的密文有误！请重新输入：\n");
                                fflush(stdin);
                        }
                        printf("\n请输入16位密钥（十六进制格式，八位间以空白字符分隔）:\n");
                        fflush(stdin);
                        while (!scanf("%x%x",&Key[0],&Key[1]))
                        {
                                printf("\n您输入的密钥有误！请重新输入：\n");
                                fflush(stdin);
                        }
                        Extend_Key(Key,RoundKey);
                        DecryptBlock(CipherText,RoundKey,PlainText);
                        printf("\n解密后的明文为：%X %X\n",PlainText[0],PlainText[1]);
                        break;
                case '4':
                        printf("\n请输入要解密的文件名（包括扩展名）：\n");
                        scanf("%s",FileName);
                        while(NULL==(PlainFile=fopen(FileName,"rb")))
                        {
                                fflush(stdin);
                                printf("\n打开文件失败！请确保文件与程序在同一目录下。\n\n重新输入文件名：\n");
                                scanf("%s",FileName);
                        }
                        printf("\n请输入要输出结果的文件名（包括扩展名）：\n");
                        scanf("%s",FileName);
                        while(NULL==(CipherFile=fopen(FileName,"wb")))
                        {
                                fflush(stdin);
                                printf("\n创建文件失败！重新输入文件名：\n");
                                scanf("%s",FileName);
                        }
                        printf("\n请输入16位密钥（十六进制格式，八位间以空白字符分隔）:\n");
                        while (!scanf("%x%x",&Key[0],&Key[1]))
                        {
                                printf("\n您输入的密钥有误！请重新输入：\n");
                                fflush(stdin);
                        }
                        DecryptFile(PlainFile,Key,CipherFile);
                        printf("\n解密后的结果已输出至%s文件中。\n",FileName);
                        fclose(PlainFile);
                        fclose(CipherFile);
                        break;
                case '5':
                        exit(0);
                        break;
                default:
                        break;
                }
        }
}