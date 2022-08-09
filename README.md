# fq2fa 尝试使用 fast_zlib

## 1. 查看 fq2fa.c 文件

cat `fq2fa.c`

```c
#include <stdio.h>
#include <zlib.h>
#include "klib/kseq.h"


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
```
## 2. 系统安装zlib 

```shell
yum install -y zlib1g-dev
```

## 3. 下载fast_zlib, 下载zlib

```shell
git clone https://github.com/gildor2/fast_zlib.git
```

- http://www.zlib.net/
- 下载 http://www.zlib.net/zlib-1.2.12.tar.gz
```shell
$ sha256sum zlib-1.2.12.tar.gz 
91844808532e5ce316b3c010929493c0244f3d37593afd6de04f71821d5136d9  zlib-1.2.12.tar.gz
```
## 4. 修改zlib代码


1. 复制 `fast_zlib/Sources/match.h` 到 `zlib-1.2.12`
2. `cd zlib-1.2.12`
3. `mv deflate.c deflate.old.c`
4. `touch deflate.c`
5. 写入：
```c
#define ASMV
#include "deflate.old.c"

#undef local
#define local

#include "match.h"

void match_init()
{
}
```

## 5. 编译安装 zlib-1.2.12

```
./configure --prefix=/home/lixy/Clion/fast_zlib_test/zlib-1.2.12/build --shared --static
make && make install
```

## 6. 编译链接 fq2fa.c


```
gcc -lz -Lzlib -o fq2fa_zlib fq2fa.c
gcc -o fq2fa_fast_zlib fq2fa.c -I/home/lixy/Clion/fast_zlib_test/zlib-1.2.12/build/include -L/home/lixy/Clion/fast_zlib_test/zlib-1.2.12/build/lib -lz
```

查看 MD5:
```
$  md5sum fq2fa_zlib fq2fa_fast_zlib 
4c03dc0377470f6a589e1bb4a9ffb7b0  fq2fa_zlib
e237f08440a7db5821aa902c5a8cfc1a  fq2fa_fast_zlib
```

## 7. 测试 zlib 和 fast_zlib 版本各自的速度

输入文件：
```shell
$ ll 7120_R1.fq.gz 
-rw-rw-r-- 1 lixy lixy 102M Aug  9 16:17 7120_R1.fq.gz
$ zcat 7120_R1.fq.gz | wc -l    
4000000
$ zcat 7120_R1.fq.gz | head -4 
@E100020639L1C001R0020000053/1
AAGAAAGAAAGAAATCAGGTGGGAGAAATCAAGAATGAAGTCAGACATAACAGCTGCAAGGTAGATGGCTGGCAAAGTAACGGAGGCAGAAGAGGCTTTATGAAAAAGTGAAAAAGTTCATGTACTTGTATCAGCCATCTAGACCATTTC
+
EEEFEEEEFEEEEFEEEEEEEEEEEEEEEEEFEEEEEEFE?EEEFEEFEFEEDEEEEEEEEEEEDDEEEEEEEEDEEEFEEEEEEEDEECFEEDA=EDEEEEEEEEECDE?EEEEE.=EEEE7EEEEE6BEEDEBAEDDEEE.DEEBCDE
```


fq2fa_zlib: 
```
$ /bin/time -v ./fq2fa_zlib 712_R1.fq.gz 712_R1_zlib.fa.gz  
	Command being timed: "./fq2fa_zlib 712_R1.fq.gz 712_R1_zlib.fa.gz"
	User time (seconds): 3.42
	System time (seconds): 0.01
	Percent of CPU this job got: 99%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:03.46
	Average shared text size (kbytes): 0
	Average unshared data size (kbytes): 0
	Average stack size (kbytes): 0
	Average total size (kbytes): 0
	Maximum resident set size (kbytes): 980
	Average resident set size (kbytes): 0
	Major (requiring I/O) page faults: 0
	Minor (reclaiming a frame) page faults: 290
	Voluntary context switches: 3
	Involuntary context switches: 85
	Swaps: 0
	File system inputs: 264
	File system outputs: 9312
	Socket messages sent: 0
	Socket messages received: 0
	Signals delivered: 0
	Page size (bytes): 4096
	Exit status: 0

```

fq2fa_fast_zlib: 
```
$ /bin/time -v ./fq2fa_fast_zlib 712_R1.fq.gz 712_R1_fast_zlib.fa.gz  
	Command being timed: "./fq2fa_fast_zlib 712_R1.fq.gz 712_R1_fast_zlib.fa.gz"
	User time (seconds): 2.43
	System time (seconds): 0.00
	Percent of CPU this job got: 99%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:02.44
	Average shared text size (kbytes): 0
	Average unshared data size (kbytes): 0
	Average stack size (kbytes): 0
	Average total size (kbytes): 0
	Maximum resident set size (kbytes): 1008
	Average resident set size (kbytes): 0
	Major (requiring I/O) page faults: 0
	Minor (reclaiming a frame) page faults: 292
	Voluntary context switches: 1
	Involuntary context switches: 39
	Swaps: 0
	File system inputs: 0
	File system outputs: 9320
	Socket messages sent: 0
	Socket messages received: 0
	Signals delivered: 0
	Page size (bytes): 4096
	Exit status: 0
```

查看输出结果一致性：
```
$ md5sum 712_R1_zlib.fa.gz 712_R1_fast_zlib.fa.gz  
5bd3de8cf81c78962aa7100da6ab2719  712_R1_zlib.fa.gz
5bd3de8cf81c78962aa7100da6ab2719  712_R1_fast_zlib.fa.gz
```

## 8. 疑问？

- fast_zlib 对 zlib的优化是否成功？ 如果成功了，为什么两个版本的程序速度没有差异



## 9. 听从大佬建议，使用静态库


或者 直接用 

```
gcc -o fq2fa_fast_zlib fq2fa.c /home/lixy/Clion/fast_zlib_test/zlib-1.2.12/build/lib/libz.a

$ md5sum fq2fa_zlib fq2fa_fast_zlib               
4c03dc0377470f6a589e1bb4a9ffb7b0  fq2fa_zlib
262db896e101b93ca1f2b0b7b6ee8ddd  fq2fa_fast_zlib
```


```
$ /bin/time -v ./fq2fa_fast_zlib 7120_R1.fq.gz 712_R1_fast_zlib2.fa.gz
	Command being timed: "./fq2fa_fast_zlib 7120_R1.fq.gz 712_R1_fast_zlib2.fa.gz"
	User time (seconds): 24.55
	System time (seconds): 0.12
	Percent of CPU this job got: 99%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:24.84
	Average shared text size (kbytes): 0
	Average unshared data size (kbytes): 0
	Average stack size (kbytes): 0
	Average total size (kbytes): 0
	Maximum resident set size (kbytes): 1012
	Average resident set size (kbytes): 0
	Major (requiring I/O) page faults: 0
	Minor (reclaiming a frame) page faults: 292
	Voluntary context switches: 8
	Involuntary context switches: 617
	Swaps: 0
	File system inputs: 105752
	File system outputs: 93128
	Socket messages sent: 0
	Socket messages received: 0
	Signals delivered: 0
	Page size (bytes): 4096
	Exit status: 0
```

速度快了1/3