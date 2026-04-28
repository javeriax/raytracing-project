/* stb_image_write - minimal version for PNG writing */
#ifndef INCLUDE_STB_IMAGE_WRITE_H
#define INCLUDE_STB_IMAGE_WRITE_H
#include <stdlib.h>
#ifdef __cplusplus
extern "C" {
#endif
extern int stbi_write_png(char const *filename, int w, int h, int comp, const void *data, int stride_in_bytes);
#ifdef __cplusplus
}
#endif
#endif

#ifdef STB_IMAGE_WRITE_IMPLEMENTATION
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

typedef unsigned int stbiw_uint32;
#define STBIW_UCHAR(x) (unsigned char)((x) & 0xff)
#define STBIW_MALLOC(s) malloc(s)
#define STBIW_FREE(p) free(p)
#define STBIW_REALLOC(p,s) realloc(p,s)
#define STBIW_REALLOC_SIZED(p,o,s) realloc(p,s)
#define STBIW_MEMMOVE(a,b,s) memmove(a,b,s)
#define STBIW_ASSERT(x) 

/* stretchy buffer */
#define stbiw__sbraw(a) ((int*)(void*)(a)-2)
#define stbiw__sbm(a) stbiw__sbraw(a)[0]
#define stbiw__sbn(a) stbiw__sbraw(a)[1]
#define stbiw__sbneedgrow(a,n) ((a)==0 || stbiw__sbn(a)+(n) >= stbiw__sbm(a))
#define stbiw__sbmaybegrow(a,n) (stbiw__sbneedgrow(a,(n)) ? stbiw__sbgrow(a,n) : 0)
#define stbiw__sbgrow(a,n) stbiw__sbgrowf((void**)&(a),(n),sizeof(*(a)))
#define stbiw__sbpush(a,v) (stbiw__sbmaybegrow(a,1),(a)[stbiw__sbn(a)++]=(v))
#define stbiw__sbcount(a) ((a)?stbiw__sbn(a):0)
#define stbiw__sbfree(a) ((a)?STBIW_FREE(stbiw__sbraw(a)),0:0)

static void *stbiw__sbgrowf(void **arr, int increment, int itemsize) {
    int m = *arr ? 2*stbiw__sbm(*arr)+increment : increment+1;
    void *p = STBIW_REALLOC_SIZED(*arr ? stbiw__sbraw(*arr):0,
        *arr?(stbiw__sbm(*arr)*itemsize+sizeof(int)*2):0, itemsize*m+sizeof(int)*2);
    if(p){ if(!*arr)((int*)p)[1]=0; *arr=(void*)((int*)p+2); stbiw__sbm(*arr)=m; }
    return *arr;
}

static unsigned int stbiw__crc32(unsigned char *buf, int len) {
    static unsigned int t[256];
    if(!t[1]){
        for(int i=0;i<256;i++){
            unsigned int c=i;
            for(int j=0;j<8;j++) c=(c&1)?(0xEDB88320^(c>>1)):(c>>1);
            t[i]=c;
        }
    }
    unsigned int c=~0u;
    for(int i=0;i<len;i++) c=(c>>8)^t[buf[i]^(c&0xff)];
    return ~c;
}

static unsigned char *stbi_zlib_compress(unsigned char *data, int dlen, int *olen, int q) {
    /* store uncompressed with zlib wrapper */
    unsigned int s1=1,s2=0;
    for(int i=0;i<dlen;i++){s1=(s1+data[i])%65521;s2=(s2+s1)%65521;}
    unsigned char *out=NULL;
    stbiw__sbpush(out,0x78); stbiw__sbpush(out,0x01);
    for(int j=0;j<dlen;){
        int blen=dlen-j; if(blen>32767)blen=32767;
        stbiw__sbpush(out,dlen-j==blen);
        stbiw__sbpush(out,STBIW_UCHAR(blen));
        stbiw__sbpush(out,STBIW_UCHAR(blen>>8));
        stbiw__sbpush(out,STBIW_UCHAR(~blen));
        stbiw__sbpush(out,STBIW_UCHAR(~blen>>8));
        for(int k=0;k<blen;k++) stbiw__sbpush(out,data[j+k]);
        j+=blen;
    }
    stbiw__sbpush(out,STBIW_UCHAR(s2>>8));
    stbiw__sbpush(out,STBIW_UCHAR(s2));
    stbiw__sbpush(out,STBIW_UCHAR(s1>>8));
    stbiw__sbpush(out,STBIW_UCHAR(s1));
    *olen=stbiw__sbn(out);
    STBIW_MEMMOVE(stbiw__sbraw(out),out,*olen);
    return (unsigned char*)stbiw__sbraw(out);
}

#define stbiw__wp32(d,v) (d)[0]=STBIW_UCHAR((v)>>24),(d)[1]=STBIW_UCHAR((v)>>16),(d)[2]=STBIW_UCHAR((v)>>8),(d)[3]=STBIW_UCHAR(v),(d)+=4
#define stbiw__wptag(d,s) (d)[0]=s[0],(d)[1]=s[1],(d)[2]=s[2],(d)[3]=s[3],(d)+=4
static void stbiw__wpcrc(unsigned char **d,int len){
    unsigned int c=stbiw__crc32(*d-len-4,len+4);
    stbiw__wp32(*d,c);
}

static unsigned char *stbi_write_png_to_mem(const unsigned char *pix, int stride, int x, int y, int n, int *olen) {
    if(stride==0) stride=x*n;
    /* filter with None (0) */
    unsigned char *filt=(unsigned char*)STBIW_MALLOC((x*n+1)*y);
    if(!filt) return 0;
    for(int j=0;j<y;j++){
        filt[j*(x*n+1)]=0;
        memcpy(filt+j*(x*n+1)+1, pix+j*stride, x*n);
    }
    int zlen;
    unsigned char *zlib=stbi_zlib_compress(filt,y*(x*n+1),&zlen,8);
    STBIW_FREE(filt);
    if(!zlib) return 0;
    unsigned char *out=(unsigned char*)STBIW_MALLOC(8+12+13+12+zlen+12);
    if(!out){STBIW_FREE(zlib);return 0;}
    *olen=8+12+13+12+zlen+12;
    unsigned char *o=out;
    unsigned char sig[8]={137,80,78,71,13,10,26,10};
    memcpy(o,sig,8); o+=8;
    stbiw__wp32(o,13); stbiw__wptag(o,"IHDR");
    stbiw__wp32(o,x); stbiw__wp32(o,y);
    *o++=8; *o++=(n==1?0:n==2?4:n==3?2:6); *o++=0; *o++=0; *o++=0;
    stbiw__wpcrc(&o,13);
    stbiw__wp32(o,zlen); stbiw__wptag(o,"IDAT");
    memcpy(o,zlib,zlen); o+=zlen; STBIW_FREE(zlib);
    stbiw__wpcrc(&o,zlen);
    stbiw__wp32(o,0); stbiw__wptag(o,"IEND"); stbiw__wpcrc(&o,0);
    return out;
}

int stbi_write_png(char const *fname, int x, int y, int comp, const void *data, int stride) {
    int len;
    unsigned char *png=stbi_write_png_to_mem((const unsigned char*)data,stride,x,y,comp,&len);
    if(!png) return 0;
    FILE *f=fopen(fname,"wb");
    if(!f){STBIW_FREE(png);return 0;}
    fwrite(png,1,len,f); fclose(f); STBIW_FREE(png);
    return 1;
}
#endif /* STB_IMAGE_WRITE_IMPLEMENTATION */
