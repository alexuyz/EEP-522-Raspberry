# CXX ?= g++

# DEBUG ?= 1
# ifeq ($(DEBUG), 1)
#     CXXFLAGS += -g
# else
#     CXXFLAGS += -O2

# endif

# server: main.cpp  ./timer/lst_timer.cpp ./http/http_conn.cpp ./log/log.cpp ./CGImysql/sql_connection_pool.cpp  webserver.cpp config.cpp
# 	$(CXX) -o server  $^ $(CXXFLAGS) -lpthread -lmysqlclient

# clean:
# 	rm  -r server

CXX ?= g++
DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g
else
    CXXFLAGS += -O2
endif

# 使用mysql_config获取合适的编译和链接标志
MYSQL_CFLAGS := $(shell mysql_config --cflags)
MYSQL_LIBS := $(shell mysql_config --libs)

# 将MySQL标志添加到CXXFLAGS和LDFLAGS
CXXFLAGS += $(MYSQL_CFLAGS)
LDFLAGS = -lpthread $(MYSQL_LIBS)

server: main.cpp ./timer/lst_timer.cpp ./http/http_conn.cpp ./log/log.cpp ./CGImysql/sql_connection_pool.cpp webserver.cpp config.cpp
	$(CXX) -o server $^ $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -r server