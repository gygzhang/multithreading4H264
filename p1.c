#include<stdio.h>
#include<stdlib.h>
#define high_code_stram  0x000000FA
#define low_code_stram   0x000000FB
#define audio_code_stram 0x000000FC

#define printx(val) printf("%08x\n",val)
#define print(val) printf("%d\n",val)

typedef unsigned char  uchar;
typedef unsigned short uint16;
typedef unsigned int   uint32;
typedef unsigned long  uint64;

typedef struct package_head{
    uint32 stream;
    uint32 len;
    uint32 fram;
    uchar type;
}package_head;



void print_bytes(void *p, size_t len)
{
    size_t i;
    printf("(");
    for (i = 0; i < len; ++i)
        printf("%02X", ((unsigned char*)p)[i]);
    printf(")");
}

void p(void *p, size_t len)
{
    size_t i;
    printf("(");
    for (i = 0; i < len; ++i)
        printf("%02X", ((unsigned char*)p)[i]);
    printf(")");
}

void ex_endian(void* p,size_t len){
    int i;
    uint32 tmep;
    uchar temp1;
    if(len%2!=0) return;
    for(i=0;i<len/2;i++){
        temp1 = ((uchar*)p)[i];
        ((uchar*)p)[i] = ((uchar*)p)[len-i-1];
        ((uchar*)p)[len-i-1] = temp1;
    }
}

uint32 ibp_type(uchar type){
    return type&~0xfc;
}

uint32 is_fill(uchar type){
    return (type&~0x7f)==128?1:0;
}

uint32 data_len(uint32 len){
    return len;
}

int main(){
    FILE* f = fopen("./BB.bin","rb");
    FILE* fh264 = fopen("./BB.h264","wb");
    package_head phead;
    uint32 ihead,readbytes,len;
    int ii=0;
    unsigned short temp1,temp2;
    readbytes = 0;
    uchar *data;
    //fread(&ihead,sizeof(int),1,f);
    //printx(ihead);
    //ex_endian((void*)&ihead,4);
    //printx(ihead);
    //return 0;
    //short temp1,temp2;
    //fseek(f,0X00342a60,SEEK_CUR);
    while(fread(&temp1,sizeof(short),1,f)==1){
        //fseek(fp,SEEK_CUR)
        if(temp1==0x0000){
            if(fread(&temp2,sizeof(short),1,f)!=1) return 0;
            //            print_bytes((void*)&temp2,2);
            //printf("%02X\n",temp2);
            ii++;
            //if(ii==4) return 0; 

            //  return 0; 
            ex_endian((void*)&temp2,2);
            if(temp2==0x00fa){
                fseek(f,-4,SEEK_CUR);
                fread(&phead,sizeof(package_head),1,f);
                fseek(f,-2,SEEK_CUR);
                ex_endian((void*)&(phead.stream),4);
                len = data_len(phead.len);
                data = (uchar*)malloc(sizeof(uchar)*len);
                fread(data,sizeof(uchar)*len,1,f);
                fwrite((void*)data,len,1,fh264);
                // ex_endian((void*)&(phead.len),4);
                //ex_endian((void*)&(phead.fram),4);
                //fread(&ihead,sizeof(int),1,f);
                //ex_endian((void*)&ihead,4);
                printx(phead.stream);
                print(phead.len);
                print(phead.fram);
                printf("%02x\n",phead.type);
                print_bytes((void*)data,12);
                printf("\n%d  %d\n\n\n",ibp_type(phead.type),is_fill(phead.type));
                //printf("%d\n",sizeof(package_head));
                //                print_bytes((void*)&temp2,2);
                //return 0;
            }
            //return 0;
        }
        
    }
    //print_bytes((void*)&phead,13);
    //    printf("%x\n",phead.stream);
    //int aa = high_code_stram;
    //printf("%x\n",aa);

    return 0;

}
