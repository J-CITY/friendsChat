#ifndef SALSA20_H_INCLUDED
#define SALSA20_H_INCLUDED

#include <vector>
#include <string>
#include <stdio.h>
#include <fstream>
#include <iostream>

struct Matrix {
    std::vector<std::vector<uint32_t>> mat;

    Matrix() {
        mat.resize(4);
        mat[0].resize(4);
        mat[1].resize(4);
        mat[2].resize(4);
        mat[3].resize(4);
    }

    std::vector<uint32_t> GetRow(int i) {
        return mat[i];
    }

    std::vector<uint32_t> GetCol(int i) {
        std::vector<uint32_t> res(4);
        res[0] = mat[0][i];
        res[1] = mat[1][i];
        res[2] = mat[2][i];
        res[3] = mat[3][i];
        return res;
    }

    void SetRow(std::vector<uint32_t> v, uint32_t i) {
        mat[i] = v;
        return;
    }
    void SetCol(std::vector<uint32_t> v, uint32_t i) {
        mat[0][i] = v[0];
        mat[1][i] = v[1];
        mat[2][i] = v[2];
        mat[3][i] = v[3];
        return;
    }

    Matrix& operator=(const Matrix& right) {
        //проверка на самоприсваивание
        if (this == &right) {
            return *this;
        }
        mat = right.mat;
        return *this;
    }
};

class Salsa20 {
public:
    std::vector<unsigned char> s0 = {101, 120, 112, 97};
    std::vector<unsigned char> s1 = {110, 100,  32, 51};
    std::vector<unsigned char> s2 = { 50,  45,  98, 121};
    std::vector<unsigned char> s3 = {116, 101,  32, 107};
    std::vector<unsigned char> key;
    std::string text;
    Salsa20(std::string in) {
        std::ifstream file(in);
        std::string line;
        while (std::getline(file, line)) {
            text += line;
       }
       std::cout << text << std::endl;
    }

    void GenKey() {
        for (int i = 0; i < 32; i++) {
            key.push_back(rand() % 256);
        }
        std::ofstream stream ("key.txt");
        for(int i = 0; i < key.size(); ++i) {
            stream << key[i];
        }
    }

    Matrix mat;
    Matrix matdef;

    std::vector<uint32_t> Quarterround(std::vector<uint32_t> y) {
        std::vector<uint32_t> z;
        z.resize(4);
        z[1] = y[1] ^ LeftShift(((y[0] + y[3]) % 4294967296), 7);
        z[2] = y[2] ^ LeftShift(((z[1] + y[0]) % 4294967296), 9);
        z[3] = y[3] ^ LeftShift(((z[2] + z[1]) % 4294967296), 13);
        z[0] = y[0] ^ LeftShift(((z[3] + z[2]) % 4294967296), 18);
        return z;
    }

    void Rowround() {
        std::vector<uint32_t> v;
        for (int i = 0; i < 4; ++i) {
            v = Quarterround(mat.GetRow(i));
            mat.SetRow(v, i);
        }
    }

    void Columnround() {
        std::vector<uint32_t> v;
        for (int i = 0; i < 4; ++i) {
            v = Quarterround(mat.GetCol(i));
            mat.SetCol(v, i);
        }
    }

    void Doubleround() {
        Columnround();
        Rowround();
    }

    void XOR() {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; j++) {
                mat.mat[i][j] ^= matdef.mat[i][j];
            }
        }
    }

    void Run() {
        for (int i = 0; i < 20; ++i) {
            Doubleround();
        }
        XOR();
    }

    void Encode(std::string _key) {
        if (_key == "") {
            GenKey();
        } else {
            for (int i = 0; i < _key.size(); ++i) {
                key.push_back(_key[i]);
            }
        }

        for (int i = 0; i < text.size(); i+=4) {
            std::vector<unsigned char> tmp(4);
            tmp[0] = i < text.size() ? text[i] : 0;
            tmp[1] = i+1 < text.size() ? text[i+1] : 0;
            tmp[2] = i+2 < text.size() ? text[i+2] : 0;
            tmp[3] = i+3 < text.size() ? text[i+3] : 0;


            textToIntVec.push_back(FourCharToInt(tmp));
        }

        for(int i = 0; i < textToIntVec.size(); i+=16) {
            EncodeText(i);
        }


        std::string textOut;

        for(int i = 0; i < textToIntVec.size(); ++i) {
            std::vector<unsigned char> v = IntToFourChar(textToIntVec[i]);
            for(int j = 0; j < v.size(); ++j) {
                if (v[j] != 0)
                    textOut += v[j];
            }
        }

        std::ofstream stream ("out.txt");
        stream << textOut;
    }

//private:
    std::vector<uint32_t> textToIntVec;

    unsigned long long int EncodeText(unsigned long long int pos) {
        mat.mat[0][0] = FourCharToInt(s0);
        mat.mat[0][1] = FourCharToInt(key[0],key[1],key[2],key[3]);
        mat.mat[0][2] = FourCharToInt(key[4],key[5],key[6],key[7]);
        mat.mat[0][3] = FourCharToInt(key[8],key[9],key[10],key[11]);
        mat.mat[1][0] = FourCharToInt(key[12],key[13],key[14],key[15]);
        mat.mat[1][1] = FourCharToInt(s1);
        mat.mat[1][2] = pos;
        mat.mat[1][3] = pos << 32;
        mat.mat[2][0] = pos;
        mat.mat[2][1] = pos << 32;
        mat.mat[2][2] = FourCharToInt(s2);
        mat.mat[2][3] = FourCharToInt(key[16],key[17],key[18],key[19]);
        mat.mat[3][0] = FourCharToInt(key[20],key[21],key[22],key[23]);
        mat.mat[3][1] = FourCharToInt(key[24],key[25],key[26],key[27]);
        mat.mat[3][2] = FourCharToInt(key[28],key[29],key[30],key[31]);
        mat.mat[3][3] = FourCharToInt(s3);
        matdef = mat;

        Run();

        for (int i = pos; i < pos+16; ++i) {
            if (i < textToIntVec.size()) {
                std::cout<< i/4 << " " << i-(i/4)*4 << " !!\n";
                std::cout<< textToIntVec[i] << " " << mat.mat[i/4][i-(i/4)*4] << " " << (textToIntVec[i]^mat.mat[i/4][i-(i/4)*4]) << " !!\n";
                textToIntVec[i] ^= mat.mat[i/4][i-(i/4)*4];
                //std::cout << (int)IntToFourChar(textToIntVec[i])[0] << " ";
                //std::cout << (int)IntToFourChar(textToIntVec[i])[1] << " ";
                //std::cout << (int)IntToFourChar(textToIntVec[i])[2] << " ";
                //std::cout << (int)IntToFourChar(textToIntVec[i])[3] << " ";
            } else {
                break;
            }
        }
    }

    uint32_t LeftShift(uint32_t a, unsigned int n) {
        uint32_t t1, t2;
        n = n % (sizeof(a)*8);
        t1 = a << n;
        t2 = a >> (sizeof(a)*8 - n);
        return t1 | t2;
    }

    uint32_t RightShift(uint32_t a, unsigned int n) {
        int t1, t2;
        n = n % (sizeof(a)*8);
        t1 = a >> n;
        t2 = a << (sizeof(a)*8 - n);
        return t1 | t2;
    }

    uint32_t FourCharToInt(std::vector<unsigned char> chars) {
        uint32_t res = 0;
        res = chars[0];
        res |=  chars[1] << 8;
        res |=  chars[2] << 16;
        res |=  chars[3] << 24;
        return res;
    }

    uint32_t FourCharToInt(unsigned char char0, unsigned char char1,
                           unsigned char char2, unsigned char char3) {
        uint32_t res = 0;
        res = char0;
        res |=  char1 << 8;
        res |=  char2 << 16;
        res |=  char3 << 24;
        return res;
    }

    std::vector<unsigned char> IntToFourChar(uint32_t ints) {
        std::vector<unsigned char> chars(4);
        chars[0] = (unsigned char)ints;
        chars[1] = (unsigned char)(ints >> 8);
        chars[2] = (unsigned char)(ints >> 16);
        chars[3] = (unsigned char)(ints >> 24);
        return chars;
    }
};

#endif // SALSA20_H_INCLUDED
