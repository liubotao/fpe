#include <iostream>

#include "fpe.h"

int main() {
    std::cout << "new day" << std::endl;

    std::string key = "abcdefghijk12345abcdefghijk12345";;

    std::string num_str = "123123.0102";
    std::cout << "original_str:" << num_str << std::endl;

    std::string encrypt_str = FPE::encrypt_num(num_str, key);
    std::cout << "encrypt_str:" << std::endl;
    std::cout << encrypt_str << std::endl;

    std::string decrypt_str = FPE::decrypt_num(encrypt_str, key);
    std::cout << "decrypt_str:" << decrypt_str << std::endl;

    return 0;
}
