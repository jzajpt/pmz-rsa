# Project: Pmz_RSA
# Makefile created by Dev-C++ 4.9.9.2

CPP  = g++
CC   = gcc
RES  = 
OBJ  = src/Main.o src/BigNum/BigNum.o src/BigNum/NumberBuffer.o src/Common/Base64.o src/Common/Exceptions.o src/Rsa/Rsa.o src/Rsa/RsaKey.o $(RES)
LINKOBJ  = src/Main.o src/BigNum/BigNum.o src/BigNum/NumberBuffer.o src/Common/Base64.o src/Common/Exceptions.o src/Rsa/Rsa.o src/Rsa/RsaKey.o $(RES)
# LIBS =  -L"C:/Dev-Cpp/lib"  -s 
# INCS =  -I"C:/Dev-Cpp/include" 
# CXXINCS =  -I"C:/Dev-Cpp/lib/gcc/mingw32/3.4.2/include"  -I"C:/Dev-Cpp/include/c++/3.4.2/backward"  -I"C:/Dev-Cpp/include/c++/3.4.2/mingw32"  -I"C:/Dev-Cpp/include/c++/3.4.2"  -I"C:/Dev-Cpp/include" 
BIN  = Pmz_RSA
CXXFLAGS = $(CXXINCS)   -O1
CFLAGS = $(INCS)   -O1
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before Pmz_RSA all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o "Pmz_RSA" $(LIBS)

src/Main.o: src/Main.cc
	$(CPP) -c src/Main.cc -o src/Main.o $(CXXFLAGS)

src/BigNum/BigNum.o: src/BigNum/BigNum.cc
	$(CPP) -c src/BigNum/BigNum.cc -o src/BigNum/BigNum.o $(CXXFLAGS)

src/BigNum/NumberBuffer.o: src/BigNum/NumberBuffer.cc
	$(CPP) -c src/BigNum/NumberBuffer.cc -o src/BigNum/NumberBuffer.o $(CXXFLAGS)

src/Common/Base64.o: src/Common/Base64.cc
	$(CPP) -c src/Common/Base64.cc -o src/Common/Base64.o $(CXXFLAGS)

src/Common/Exceptions.o: src/Common/Exceptions.cc
	$(CPP) -c src/Common/Exceptions.cc -o src/Common/Exceptions.o $(CXXFLAGS)

src/Rsa/Rsa.o: src/Rsa/Rsa.cc
	$(CPP) -c src/Rsa/Rsa.cc -o src/Rsa/Rsa.o $(CXXFLAGS)

src/Rsa/RsaKey.o: src/Rsa/RsaKey.cc
	$(CPP) -c src/Rsa/RsaKey.cc -o src/Rsa/RsaKey.o $(CXXFLAGS)
