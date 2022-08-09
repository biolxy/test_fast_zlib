#include <stdio.h>
#include <zlib.h>
#include "klib/kseq.h"


// gcc -lz -Lzlib -o fq2fa_zlib fq2fa2.c
// gcc -o fq2fa_fast_zlib fq2fa2.c -I/home/lixy/Clion/fast_zlib_test/zlib-1.2.12/build/include -L/home/lixy/Clion/fast_zlib_test/zlib-1.2.12/build/lib -lz
// $ md5sum fq2fa_zlib fq2fa_fast_zlib
// d7870660082e47d66314dd9cbb72d5c0  fq2fa_zlib
// 2bdfb104f82bddec142b9fa8c9c4ec82  fq2fa_fast_zlib

KSEQ_INIT(gzFile, gzread)

int main(int argc, char *argv[])
{

        gzFile fp;
        gzFile fo;
        if (argc < 2 ){
            return -1;
        }
        if ( argc == 3 ){
            fo = gzopen (argv[2], "wb");
        }

        kseq_t *seq;
        int l;
        if (argc == 1){
            fprintf(stderr, "Usage: %s <in.fasta|in.fasta.gz>\n", argv[0]);
            return 1;
        }

        fp = gzopen(argv[1], "r");
        seq = kseq_init(fp); // 分配内存给seq
        while( (l = kseq_read(seq)) >= 0){ //读取数据到seq中
            gzprintf(fo, "%s", seq->name.s);
            gzprintf(fo, "%s", seq->seq.s);
        }

        kseq_destroy(seq); //释放内存
        gzclose(fp);
        if (argc == 3) gzclose(fo);
        return 0;


}