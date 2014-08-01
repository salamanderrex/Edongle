
#include "S-AES.h"

void main()
{
        printf("\t\t��������������S-AES�ӽ��ܹ��ߡ�������������\n\n");
        while(1)
        {
                char OpNum=0,FileName[256]="";
                BYTE PlainText[2]={0},Key[2]={0},RoundKey[6]={0},CipherText[2]={0};
                FILE *PlainFile=NULL,*CipherFile=NULL;
                printf("\n����������ţ�1-����һ������ 2-�����ļ� 3-����һ������ 4-�����ļ� 5-�˳�\n");
                do 
                {
                        OpNum=getch();
                }while (!(OpNum>='1'&&OpNum<='5'));
                switch(OpNum)
                {
                case '1':
                        printf("\n������16λ���ģ�ʮ�����Ƹ�ʽ����λ���Կհ��ַ��ָ���:\n");
                        fflush(stdin);
                        while (!scanf("%x%x",&PlainText[0],&PlainText[1]))
                        {
                                printf("\n������������������������룺\n");
                                fflush(stdin);
                        }
                        printf("\n������16λ��Կ��ʮ�����Ƹ�ʽ����λ���Կհ��ַ��ָ���:\n");
                        fflush(stdin);
                        while (!scanf("%x%x",&Key[0],&Key[1]))
                        {
                                printf("\n���������Կ�������������룺\n");
                                fflush(stdin);
                        }
                        Extend_Key(Key,RoundKey);
                        EncryptBlock(PlainText,RoundKey,CipherText);
                        printf("\n���ܺ������Ϊ��%X %X\n",CipherText[0],CipherText[1]);
                        break;
                case '2':
                        printf("\n������Ҫ���ܵ��ļ�����������չ������\n");
                        scanf("%s",FileName);
                        while(NULL==(PlainFile=fopen(FileName,"rb")))
                        {
                                fflush(stdin);
                                printf("\n���ļ�ʧ�ܣ���ȷ���ļ��������ͬһĿ¼�¡�\n\n���������ļ�����\n");
                                scanf("%s",FileName);
                        }
                        printf("\n������Ҫ���������ļ�����������չ������\n");
                        scanf("%s",FileName);
                        while(NULL==(CipherFile=fopen(FileName,"wb")))
                        {
                                fflush(stdin);
                                printf("\n�����ļ�ʧ�ܣ����������ļ�����\n");
                                scanf("%s",FileName);
                        }
                        printf("\n������16λ��Կ��ʮ�����Ƹ�ʽ����λ���Կհ��ַ��ָ���:\n");
                        while (!scanf("%x%x",&Key[0],&Key[1]))
                        {
                                printf("\n���������Կ�������������룺\n");
                                fflush(stdin);
                        }
                        EncryptFile(PlainFile,Key,CipherFile);
                        printf("\n���ܺ�Ľ���������%s�ļ��С�\n",FileName);
                        fclose(PlainFile);
                        fclose(CipherFile);
                        break;
                case '3':
                        printf("\n������16λ���ģ�ʮ�����Ƹ�ʽ����λ���Կհ��ַ��ָ���:\n");
                        fflush(stdin);
                        while (!scanf("%x%x",&CipherText[0],&CipherText[1]))
                        {
                                printf("\n������������������������룺\n");
                                fflush(stdin);
                        }
                        printf("\n������16λ��Կ��ʮ�����Ƹ�ʽ����λ���Կհ��ַ��ָ���:\n");
                        fflush(stdin);
                        while (!scanf("%x%x",&Key[0],&Key[1]))
                        {
                                printf("\n���������Կ�������������룺\n");
                                fflush(stdin);
                        }
                        Extend_Key(Key,RoundKey);
                        DecryptBlock(CipherText,RoundKey,PlainText);
                        printf("\n���ܺ������Ϊ��%X %X\n",PlainText[0],PlainText[1]);
                        break;
                case '4':
                        printf("\n������Ҫ���ܵ��ļ�����������չ������\n");
                        scanf("%s",FileName);
                        while(NULL==(PlainFile=fopen(FileName,"rb")))
                        {
                                fflush(stdin);
                                printf("\n���ļ�ʧ�ܣ���ȷ���ļ��������ͬһĿ¼�¡�\n\n���������ļ�����\n");
                                scanf("%s",FileName);
                        }
                        printf("\n������Ҫ���������ļ�����������չ������\n");
                        scanf("%s",FileName);
                        while(NULL==(CipherFile=fopen(FileName,"wb")))
                        {
                                fflush(stdin);
                                printf("\n�����ļ�ʧ�ܣ����������ļ�����\n");
                                scanf("%s",FileName);
                        }
                        printf("\n������16λ��Կ��ʮ�����Ƹ�ʽ����λ���Կհ��ַ��ָ���:\n");
                        while (!scanf("%x%x",&Key[0],&Key[1]))
                        {
                                printf("\n���������Կ�������������룺\n");
                                fflush(stdin);
                        }
                        DecryptFile(PlainFile,Key,CipherFile);
                        printf("\n���ܺ�Ľ���������%s�ļ��С�\n",FileName);
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