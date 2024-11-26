# libiconv

使用 gnu make 编译 libiconv 。

1. 将 库源码放置在 ./libiconv 目录下。
1. 将 [./libiconv/libcharset/include/localcharset.h.build.in](./libiconv/libcharset/include/localcharset.h.build.in) 复制为 [./include/localcharset.h](./include/localcharset.h) 。
    1. 处理 *LIBCHARSET_DLL_EXPORTED* 宏定义。
1. 将 [./libiconv/include/iconv.h.build.in](./libiconv/include/iconv.h.build.in) 复制为 [./include/iconv.h](./include/iconv.h) 。
    1. 处理 *LIBICONV_DLL_EXPORTED* 宏定义。
    1. 去除 *@DLL_VARIABLE@* 。
    1. *@ICONV_CONST@* 替换为 *const* 。
    1. 去除所有 *@* 字符。
1. 将 [./libiconv/config.h.in](./libiconv/config.h.in) 复制为 [./include/config.h](./include/config.h) 。
    1. 注释 line.28 `#undef EILSEQ`