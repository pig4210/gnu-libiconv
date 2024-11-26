INCLUDE_PATH    := $(SRCPATH)/include
LUA             := $(SRCPATH)/tools/lua.exe

# iconv.c 只替换 ICONV_CONST 为 const 。
$(DSTPATH)/iconv.c : $(SRCPATH)/libiconv/lib/iconv.c | $(DSTPATH)
	cd "$(DSTPATH)" && $(LUA) -e "local file = io.open([[$<]], 'r'); local ss = file:read('a'); file:close(); file = io.open([[$(@F)]], 'wb'); ss = ss:gsub('ICONV_CONST', 'const'); file:write(ss); file:close();"
# iconv.h
#     替换 '@' 为 空。
#     替换 '@HAVE_VISIBILITY@' 为 '0' 。
#     替换 '@DLL_VARIABLE@' 为 空 。
#     替换 'ICONV_CONST' 为 'const' 。
#     替换 '‘' '’' 为 ''' 。
$(DSTPATH)/iconv.h : $(SRCPATH)/libiconv/include/iconv.h.build.in | $(DSTPATH)
	cd "$(DSTPATH)" && $(LUA) -e "local file = io.open([[$<]], 'r'); local ss = file:read('a'); file:close(); file = io.open([[$(@F)]], 'wb'); ss = ss:gsub('@HAVE_VISIBILITY@', '0'):gsub('@DLL_VARIABLE@', ''):gsub('ICONV_CONST', 'const'):gsub('@', ''):gsub('\xE2\x80\x98', '\''):gsub('\xE2\x80\x99', '\''); file:write(ss); file:close();"
# relocatable.c 直接复制。
$(DSTPATH)/relocatable.c : $(SRCPATH)/libiconv/lib/relocatable.c | $(DSTPATH)
	copy /y "$(subst /,\,$(abspath $<))" "$(DSTPATH)/$(@F)"
# localcharset.c 直接复制。
$(DSTPATH)/localcharset.c : $(SRCPATH)/libiconv/libcharset/lib/localcharset.c | $(DSTPATH)
	copy /y "$(subst /,\,$(abspath $<))" "$(DSTPATH)/$(@F)"
# localcharset.h 替换 '@HAVE_VISIBILITY@' 为 '0' 。
$(DSTPATH)/localcharset.h : $(SRCPATH)/libiconv/libcharset/include/localcharset.h.build.in | $(DSTPATH)
	cd "$(DSTPATH)" && $(LUA) -e "local file = io.open([[$<]], 'r'); local ss = file:read('a'); file:close(); file = io.open([[$(@F)]], 'wb'); ss = ss:gsub('@HAVE_VISIBILITY@', '0'); file:write(ss); file:close();"
# config.h 注释 '#undef EILSEQ' 。
$(DSTPATH)/config.h : $(SRCPATH)/libiconv/config.h.in | $(DSTPATH)
	cd "$(DSTPATH)" && $(LUA) -e "local file = io.open([[$<]], 'r'); local ss = file:read('a'); file:close(); file = io.open([[$(@F)]], 'wb'); ss = ss:gsub('#undef EILSEQ', '//#undef EILSEQ'); file:write(ss); file:close();"

# lib 目录下所有 .h 取出。
ICONV_LIB_HFILES := $(shell cd "$(SRCPATH)/libiconv/lib" && dir /t:w /o-d *.h /b /a-d)
DEST_LIB_HFILES = $(addprefix $(DSTPATH)/, $(ICONV_LIB_HFILES))
$(DSTPATH)/%.h : $(SRCPATH)/libiconv/lib/%.h | $(DSTPATH)
	copy /y "$(subst /,\,$(abspath $<))" "$(DSTPATH)/$(@F)"
# lib 目录下所有 .def 取出。
ICONV_DEF_HFILES := $(shell cd "$(SRCPATH)/libiconv/lib" && dir /t:w /o-d *.def /b /a-d)
DEST_DEF_HFILES = $(addprefix $(DSTPATH)/, $(ICONV_DEF_HFILES))
$(DSTPATH)/%.def : $(SRCPATH)/libiconv/lib/%.def | $(DSTPATH)
	copy /y "$(subst /,\,$(abspath $<))" "$(DSTPATH)/$(@F)"

$(INCLUDE_PATH) :
	@mkdir "$@"
$(INCLUDE_PATH)/iconv.h : $(DSTPATH)/iconv.h | $(INCLUDE_PATH)
	copy /y "$(subst /,\,$(abspath $<))" "$(INCLUDE_PATH)/$(@F)"

.PHONY : check_lib_files
check_lib_files : \
    $(DSTPATH)/iconv.c \
    $(DSTPATH)/iconv.h \
    $(DSTPATH)/relocatable.c \
    $(DSTPATH)/localcharset.c \
    $(DSTPATH)/localcharset.h \
    $(DSTPATH)/config.h \
    $(DEST_LIB_HFILES) \
    $(DEST_DEF_HFILES)

vpath %.c   $(DSTPATH)
vpath %.h   $(DSTPATH)

CFLAGS          += /I"$(DSTPATH)" /wd4018 /wd4090 /wd4100 /wd4127 /wd4189 /wd4206 /wd4244 /wd4245 /wd4267 /wd4311
CFLAGS          += /D_CRT_SECURE_NO_WARNINGS

#CFLAGS          += /Zc:threadSafeInit-

################################################################
# 静态库只需要如下三个 .o 文件。
OBJ :=  iconv.o localcharset.o relocatable.o

%.o : %.c | $(DSTPATH)
	$(CC) $(CFLAGS) /Fd"$(DSTPATH)/" /Fo"$(DSTPATH)/$(@F)" "$<"

iconv.lib : $(OBJ) | $(DSTPATH)
	$(AR) $(ARFLAGS) /LIBPATH:"$(DSTPATH)" /OUT:"$(DSTPATH)/$(@F)" $(^F)
	@echo.

################################################################
all : check_lib_files \
      iconv.lib \
	  $(INCLUDE_PATH)/iconv.h
	@echo make done.