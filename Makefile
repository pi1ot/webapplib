# /// \file Makefile 
# /// Web Application Library Makefile

################################################################################
# 使用方法

# 使用 make 命令编译生成库文件；
# 使用 make install 命令将库文件、库头文件复制到系统目录；
# 使用 make uninstall 命令删除系统目录中的库文件、库头文件；
# 使用 make clean 命令清除当前目录的编译结果及临时文件；
# 使用动态库时（默认情况下）必须执行 make install 将库文件安装到系统目录，
# 在项目中使用 webapplib，Makefile 内容可参考 Makefile.example；
# 不具备 root 权限的用户可以将本 Makefile 中 $(LIBPATH) 改为其他目录，
# 并将该目录添加到执行文件运行环境的环境变量 LD_LIBRARY_PATH 中.

################################################################################
# 当前WEB开发库版本号 $(WEBAPPLIB_VERSION)
WEBAPPLIB_VERSION = 1.0

# C++ 编译器命令
CXX = g++

# 编译选项
CXXFLAGS = -Wall -O2 -fPIC #-s
# -Wall 显示所有警告信息
# -O2 编译优化
# -fPIC 编译地址无关代码，用于生成动态链接库
# -s 去除调试符号信息，减小对象文件尺寸

################################################################################
# 系统头文件目录
INCPATH = /usr/local/include/webapplib
# 系统库文件目录
LIBPATH = /usr/local/lib
SYSLIB = /usr/lib

################################################################################
# 是否编译 MysqlClient，若不编译 MysqlClient 则注释本变量
MYSQL = yes
# MySQL 头文件路径
MYSQLINC = -I/usr/include/mysql
# MySQL 库文件路径及链接参数
MYSQLLIB = -L/usr/lib/mysql -lmysqlclient -lm -lz

################################################################################
# 开发库对象文件列表
LIBS = String Encode Cgi FileSystem DateTime Template HttpClient TextFile ConfigFile Utility

# 是否编译MysqlClient组件
ifdef MYSQL
LIBS += MysqlClient
else
CXXFLAGS += -D_WEBAPPLIB_NOMYSQL
MYSQLINC :=
MYSQLLIB :=
endif

OBJS = $(foreach n,$(LIBS),wa$(n).o)
	
# 开发库头文件列表
WEBAPPINC = webapplib.h $(OBJS:%.o=%.h)
# 开发库静态库文件名
WEBAPPLIB = libwebapp.a.$(WEBAPPLIB_VERSION)
# 开发库动态链接库文件名
WEBAPPDLL = libwebapp.so.$(WEBAPPLIB_VERSION)

################################################################################
# 编译目标
all: $(WEBAPPLIB) $(WEBAPPDLL)

# 编译开发库对象文件
$(OBJS): %.o: %.cpp %.h
	@echo ""
	@echo "Compile $(@:%.o=%.cpp) ..."
	$(CXX) $(CXXFLAGS) -c $(@:%.o=%.cpp) $(MYSQLINC)

# 生成静态库文件
$(WEBAPPLIB): $(OBJS)
	@echo ""
	@echo "Build $(WEBAPPLIB) ..."
	$(AR) rc $@ $(OBJS)

# 生成动态库文件
$(WEBAPPDLL): $(OBJS)
	@echo ""
	@echo "Build $(WEBAPPDLL) ..."
	$(CXX) $(CXXFLAGS) -shared -o $@ $(OBJS)
	@echo ""
	@echo "Type \"make install\" to install webapplib"
	@echo "Type \"make uninstall\" to uninstall webapplib"
	@echo "Type \"make -f Makefile.example\" to build example"
	@echo ""

################################################################################
# 执行安装
install:
	@echo ""
	@echo "Install webapplib ..."
	@echo ""
	mkdir -p $(INCPATH)
	chmod 777 $(INCPATH)
	cp -f $(WEBAPPINC) $(INCPATH)
	cp -f $(WEBAPPLIB) $(WEBAPPDLL) $(LIBPATH)
	ln -fs $(LIBPATH)/$(WEBAPPLIB) $(LIBPATH)/libwebapp.a
	ln -fs $(LIBPATH)/$(WEBAPPDLL) $(LIBPATH)/libwebapp.so
	ln -fs $(LIBPATH)/$(WEBAPPLIB) $(SYSLIB)/libwebapp.a
	ln -fs $(LIBPATH)/$(WEBAPPDLL) $(SYSLIB)/libwebapp.so

# 执行删除
uninstall:
	@echo ""
	@echo "Uninstall webapplib ..."
	@echo ""
	for each in $(WEBAPPINC); \
	do \
		rm -f $(INCPATH)/$$each; \
	done;

	rm -f $(LIBPATH)/$(WEBAPPLIB)
	rm -f $(LIBPATH)/$(WEBAPPDLL)
	unlink $(LIBPATH)/libwebapp.a
	unlink $(LIBPATH)/libwebapp.so
	unlink $(SYSLIB)/libwebapp.a
	unlink $(SYSLIB)/libwebapp.so	
	
# 清空编译结果
clean:
	@echo ""
	@echo "Clean webapplib ..."
	@echo ""
	rm -f $(OBJS) $(WEBAPPLIB) $(WEBAPPDLL)

