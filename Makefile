CC = g++
CFLAGS = -o3 -Wall -std=c++11
LIBS = 
TESTLIBS = 
SRC = src
TEST_DIR =
OUT_DIR = bin
SOURCES = $(wildcard $(SRC)/*.cpp)
TESTS = 
OBJS = bin/matching
OBJSTEST = 
DBFLAGS = -g
PRFFLAGS = -pg
MKDIR_P = mkdir -p

all: directories
	$(CC) $(CFLAGS) $(SOURCES) -o $(OBJS) $(LIBS)

directories: $(OUT_DIR)

${OUT_DIR}:
	${MKDIR_P} ${OUT_DIR}

.PHONY: test