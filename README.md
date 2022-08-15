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
yum install -y zlib1g-dev zlib zlib-devel
```

```
apt-get install -y zlib1g zlib1g.dev zlib
```

## 3. 下载fast_zlib, 下载zlib

- 下载 klib
```
git clone https://github.com/attractivechaos/klib.git
```
移动到 `fq2fa` 文件夹


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
4. `vim deflate.c`
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

下载klib
```
git clone git@github.com:attractivechaos/klib.git
```

```
gcc -o fq2fa_zlib fq2fa.c -lz -Lzlib 
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
	User time (seconds): 35.82
	System time (seconds): 0.17
	Percent of CPU this job got: 99%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:36.29
	Average shared text size (kbytes): 0
	Average unshared data size (kbytes): 0
	Average stack size (kbytes): 0
	Average total size (kbytes): 0
	Maximum resident set size (kbytes): 980
	Average resident set size (kbytes): 0
	Major (requiring I/O) page faults: 1
	Minor (reclaiming a frame) page faults: 289
	Voluntary context switches: 14
	Involuntary context switches: 813
	Swaps: 0
	File system inputs: 205752
	File system outputs: 93184
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
	User time (seconds): 34.85
	System time (seconds): 0.11
	Percent of CPU this job got: 99%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:35.31
	Average shared text size (kbytes): 0
	Average unshared data size (kbytes): 0
	Average stack size (kbytes): 0
	Average total size (kbytes): 0
	Maximum resident set size (kbytes): 980
	Average resident set size (kbytes): 0
	Major (requiring I/O) page faults: 0
	Minor (reclaiming a frame) page faults: 290
	Voluntary context switches: 5
	Involuntary context switches: 730
	Swaps: 0
	File system inputs: 8
	File system outputs: 93184
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
$ /bin/time -v ./fq2fa_fast_zlib 712_R1.fq.gz 712_R1_fast_zlib.fa.gz                          
	Command being timed: "./fq2fa_fast_zlib 712_R1.fq.gz 712_R1_fast_zlib.fa.gz"
	User time (seconds): 24.68
	System time (seconds): 0.12
	Percent of CPU this job got: 99%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:24.99
	Average shared text size (kbytes): 0
	Average unshared data size (kbytes): 0
	Average stack size (kbytes): 0
	Average total size (kbytes): 0
	Maximum resident set size (kbytes): 1012
	Average resident set size (kbytes): 0
	Major (requiring I/O) page faults: 0
	Minor (reclaiming a frame) page faults: 292
	Voluntary context switches: 6
	Involuntary context switches: 535
	Swaps: 0
	File system inputs: 0
	File system outputs: 93240
	Socket messages sent: 0
	Socket messages received: 0
	Signals delivered: 0
	Page size (bytes): 4096
	Exit status: 0
```

可以看到，速度明显快了1/3


## 10. 发现异常

- 上述测试是在Centos 7.9, 2 CPUs, 4G MEM 环境下测试
- 切换至 Ubuntu 18.04, 36 CPUs, 128G MEM  / Ubuntu 20.04, 32 CPUs, 128G MEM后，发现 优化后的速度还不如不优化

```
for i in $(seq 1 10);do printf "712_R1.fq.gz ";done
cat 712_R1.fq.gz 712_R1.fq.gz 712_R1.fq.gz 712_R1.fq.gz 712_R1.fq.gz 712_R1.fq.gz 712_R1.fq.gz 712_R1.fq.gz 712_R1.fq.gz 712_R1.fq.gz > aa.fq.gz 
```

```
$ /usr/bin/time -v ./fq2fa_fast_zlib aa.fq.gz aa_fast_zlib.fa.gz 
	Command being timed: "./fq2fa_fast_zlib aa.fq.gz aa_fast_zlib.fa.gz"
	User time (seconds): 19.61
	System time (seconds): 0.02
	Percent of CPU this job got: 99%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:19.63
	Average shared text size (kbytes): 0
	Average unshared data size (kbytes): 0
	Average stack size (kbytes): 0
	Average total size (kbytes): 0
	Maximum resident set size (kbytes): 1888
	Average resident set size (kbytes): 0
	Major (requiring I/O) page faults: 0
	Minor (reclaiming a frame) page faults: 152
	Voluntary context switches: 1
	Involuntary context switches: 25
	Swaps: 0
	File system inputs: 0
	File system outputs: 93128
	Socket messages sent: 0
	Socket messages received: 0
	Signals delivered: 0
	Page size (bytes): 4096
	Exit status: 0
```


```
$ /usr/bin/time -v ./fq2fa_zlib aa.fq.gz aa_zlib.fa.gz     
	Command being timed: "./fq2fa_zlib aa.fq.gz aa_zlib.fa.gz"
	User time (seconds): 18.20
	System time (seconds): 0.03
	Percent of CPU this job got: 100%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:18.24
	Average shared text size (kbytes): 0
	Average unshared data size (kbytes): 0
	Average stack size (kbytes): 0
	Average total size (kbytes): 0
	Maximum resident set size (kbytes): 2040
	Average resident set size (kbytes): 0
	Major (requiring I/O) page faults: 0
	Minor (reclaiming a frame) page faults: 160
	Voluntary context switches: 1
	Involuntary context switches: 23
	Swaps: 0
	File system inputs: 0
	File system outputs: 93064
	Socket messages sent: 0
	Socket messages received: 0
	Signals delivered: 0
	Page size (bytes): 4096
	Exit status: 0
```

- 推测：
  - Ubuntu 系统上`fast_zlib` 对 `longest_match` 函数的实现 与 CentOS 系统上的不同，所以相同的修改效果不显著，甚至是无用的
  - 更换的两个Ubuntu系统均为多核心CPU, 高内存服务器，使得 `fast_zlib` 对 `longest_match` 函数的优化仅能在 较少CPU和较少内存是体现优势


## 11. 解决 10 提出的异常

### 11.1 重新编译，保持单一变量
- 上诉两个程序的编译命令不同，不符合单一变量原则
- 解决：
  - 解压 zlib-1.2.12.tar.gz
  - `cp -r zlib-1.2.12 fast_zlib-1.2.12`
  - 不修改 zlib 代码，直接编译 `zlib-1.2.12`
  - 按 4 5 步骤，修改zlib代码，编译 `fast_zlib-1.2.12`
  - 分别编译链接 `fq2fc`
    - `gcc -o fq2fa_fast_zlib fq2fa.c /home/lixy/myproject/fast_zlib_test/fast_zlib-1.2.12/build/lib/libz.a -I/home/lixy/myproject/fast_zlib_test/fast_zlib-1.2.12/build/include`
	- `gcc -o fq2fa_zlib fq2fa.c /home/lixy/myproject/fast_zlib_test/zlib-1.2.12/build/lib/libz.a -I/home/lixy/myproject/fast_zlib_test/zlib-1.2.12/build/include`

测试两个文件的速度：

```
$ /usr/bin/time -v ./fq2fa_zlib aa.fq.gz aa_zlib.fa.gz
	Command being timed: "./fq2fa_zlib aa.fq.gz aa_zlib.fa.gz"
	User time (seconds): 28.85
	System time (seconds): 0.05
	Percent of CPU this job got: 99%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:28.91
	Average shared text size (kbytes): 0
	Average unshared data size (kbytes): 0
	Average stack size (kbytes): 0
	Average total size (kbytes): 0
	Maximum resident set size (kbytes): 1892
	Average resident set size (kbytes): 0
	Major (requiring I/O) page faults: 0
	Minor (reclaiming a frame) page faults: 153
	Voluntary context switches: 1
	Involuntary context switches: 37
	Swaps: 0
	File system inputs: 0
	File system outputs: 93064
	Socket messages sent: 0
	Socket messages received: 0
	Signals delivered: 0
	Page size (bytes): 4096
	Exit status: 0
```


```
$ /usr/bin/time -v ./fq2fa_fast_zlib aa.fq.gz aa_fast_zlib.fa.gz 
	Command being timed: "./fq2fa_fast_zlib aa.fq.gz aa_fast_zlib.fa.gz"
	User time (seconds): 19.87
	System time (seconds): 0.05
	Percent of CPU this job got: 99%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:19.92
	Average shared text size (kbytes): 0
	Average unshared data size (kbytes): 0
	Average stack size (kbytes): 0
	Average total size (kbytes): 0
	Maximum resident set size (kbytes): 1948
	Average resident set size (kbytes): 0
	Major (requiring I/O) page faults: 0
	Minor (reclaiming a frame) page faults: 152
	Voluntary context switches: 1
	Involuntary context switches: 26
	Swaps: 0
	File system inputs: 0
	File system outputs: 93128
	Socket messages sent: 0
	Socket messages received: 0
	Signals delivered: 0
	Page size (bytes): 4096
	Exit status: 0
```

- 结论：在 ubuntu系统中，`fast_zlib` 项目对 `zlib`代码的修改，依旧有较大的速度提升

- 新的问题：在 ubuntu系统中，直接使用 `gcc -o fq2fa_zlib_u fq2fa.c -lz -Lzlib` 编译链接，速度比 `fast_zlib` 修改版的尽然还要稍微快一点，原因是什么？
  - 使用 `-lz -Lzlib` 时候，使用的是系统的 `zlib`, 该版本比 `zlib-1.2.12` 有较大的速度提升 ？

```
$ /usr/bin/time -v ./fq2fa_zlib-ubuntu aa.fq.gz aa_zlib-ubuntu.fa.gz
	Command being timed: "./fq2fa_zlib-ubuntu aa.fq.gz aa_zlib-ubuntu.fa.gz"
	User time (seconds): 18.59
	System time (seconds): 0.07
	Percent of CPU this job got: 99%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:18.68
	Average shared text size (kbytes): 0
	Average unshared data size (kbytes): 0
	Average stack size (kbytes): 0
	Average total size (kbytes): 0
	Maximum resident set size (kbytes): 2020
	Average resident set size (kbytes): 0
	Major (requiring I/O) page faults: 0
	Minor (reclaiming a frame) page faults: 158
	Voluntary context switches: 2
	Involuntary context switches: 24
	Swaps: 0
	File system inputs: 0
	File system outputs: 93064
	Socket messages sent: 0
	Socket messages received: 0
	Signals delivered: 0
	Page size (bytes): 4096
	Exit status: 0
```

查看系统（ubuntu）中zlib的版本

```shell
$ cat /usr/lib/x86_64-linux-gnu/pkgconfig/zlib.pc
prefix=/usr
exec_prefix=${prefix}
libdir=${prefix}/lib/x86_64-linux-gnu
sharedlibdir=${libdir}
includedir=${prefix}/include

Name: zlib
Description: zlib compression library
Version: 1.2.11

Requires:
Libs: -L${libdir} -L${sharedlibdir} -lz
Cflags: -I${includedir}
```

那么，`zlib-1.2.11` 会比 `zlib-1.2.12` 更快吗？

测试如下：

```
axel -n 8 https://github.com/madler/zlib/archive/refs/tags/v1.2.11.tar.gz
```

```
$ md5sum zlib-1.2.11.tar.gz 
0095d2d2d1f3442ce1318336637b695f  zlib-1.2.11.tar.gz
```

编译安装 
```
mkdir build
./configure --prefix=/home/lixy/myproject/fast_zlib_test/zlib-1.2.11/build  --shared --static
make && make install
```

编译 
```shell
gcc -o fq2fa_zlib-1.2.12 fq2fa.c /home/lixy/myproject/fast_zlib_test/zlib-1.2.12/build/lib/libz.a -I/home/lixy/myproject/fast_zlib_test/zlib-1.2.12/build/include

gcc -o fq2fa_zlib-1.2.11 fq2fa.c /home/lixy/myproject/fast_zlib_test/zlib-1.2.11/build/lib/libz.a -I/home/lixy/myproject/fast_zlib_test/zlib-1.2.11/build/include

gcc -o fq2fa_zlib-ubuntu fq2fa.c -lz -Lzlib

(
	gcc -o fq2fa_zlib-ubuntu fq2fa.c /usr/lib/x86_64-linux-gnu/libz.a -I/usr/include/
)
```

```
$ /usr/bin/time -v ./fq2fa_zlib-1.2.11 aa.fq.gz aa_zlib-1.2.11.fa.gz  
	Command being timed: "./fq2fa_zlib-1.2.11 aa.fq.gz aa_zlib-1.2.11.fa.gz"
	User time (seconds): 29.69
	System time (seconds): 0.03
	Percent of CPU this job got: 99%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:29.73
	Average shared text size (kbytes): 0
	Average unshared data size (kbytes): 0
	Average stack size (kbytes): 0
	Average total size (kbytes): 0
	Maximum resident set size (kbytes): 1948
	Average resident set size (kbytes): 0
	Major (requiring I/O) page faults: 0
	Minor (reclaiming a frame) page faults: 153
	Voluntary context switches: 1
	Involuntary context switches: 38
	Swaps: 0
	File system inputs: 0
	File system outputs: 93064
	Socket messages sent: 0
	Socket messages received: 0
	Signals delivered: 0
	Page size (bytes): 4096
	Exit status: 0



$ /usr/bin/time -v ./fq2fa_zlib-1.2.12 aa.fq.gz aa_zlib-1.2.12.fa.gz   
	Command being timed: "./fq2fa_zlib-1.2.12 aa.fq.gz aa_zlib-1.2.12.fa.gz"
	User time (seconds): 29.02
	System time (seconds): 0.07
	Percent of CPU this job got: 99%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:29.10
	Average shared text size (kbytes): 0
	Average unshared data size (kbytes): 0
	Average stack size (kbytes): 0
	Average total size (kbytes): 0
	Maximum resident set size (kbytes): 1948
	Average resident set size (kbytes): 0
	Major (requiring I/O) page faults: 0
	Minor (reclaiming a frame) page faults: 153
	Voluntary context switches: 2
	Involuntary context switches: 39
	Swaps: 0
	File system inputs: 0
	File system outputs: 93064
	Socket messages sent: 0
	Socket messages received: 0
	Signals delivered: 0
	Page size (bytes): 4096
	Exit status: 0


$ /usr/bin/time -v ./fq2fa_zlib-ubuntu aa.fq.gz aa_zlib-ubuntu.fa.gz 
	Command being timed: "./fq2fa_zlib-ubuntu aa.fq.gz aa_zlib-ubuntu.fa.gz"
	User time (seconds): 18.58
	System time (seconds): 0.03
	Percent of CPU this job got: 100%
	Elapsed (wall clock) time (h:mm:ss or m:ss): 0:18.61
	Average shared text size (kbytes): 0
	Average unshared data size (kbytes): 0
	Average stack size (kbytes): 0
	Average total size (kbytes): 0
	Maximum resident set size (kbytes): 2008
	Average resident set size (kbytes): 0
	Major (requiring I/O) page faults: 0
	Minor (reclaiming a frame) page faults: 157
	Voluntary context switches: 1
	Involuntary context switches: 22
	Swaps: 0
	File system inputs: 0
	File system outputs: 93064
	Socket messages sent: 0
	Socket messages received: 0
	Signals delivered: 0
	Page size (bytes): 4096
	Exit status: 0
```

- 发现，我们编译的 `zlib-1.2.11` 速度和 `zlib-1.2.12` 一样，但是却比使用系统默认zlib的版本慢很多
- 难道是 zlib 在编译的过程中，可以加入一些优化参数？
  - 后续我在 一个docker image 中测试了几种版本的区别，发现，现在 ubuntu 中安装 zlib 相关的包，再 `gcc -o fq2fa_zlib-ubuntu fq2fa.c -lz -Lzlib`
  出的程序，确实比 使用 `zlib-1.2.11` 速度和 `zlib-1.2.12` 快，原因未知。
