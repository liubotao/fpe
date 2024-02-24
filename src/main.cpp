#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>

#include <ubiq/fpe/ff1.h>
#include <iomanip>

class FPETool {

public:
    static std::string encrypt(const std::string& num_str, const std::string& key) {
        int num_str_length = num_str.length();

        std::string fixed_num_str = num_str;
        if (num_str_length < MIN_LENGTH) {
            for (int i = 0 ; i < MIN_LENGTH - num_str_length; i++) {
                fixed_num_str.push_back('0');
            }
            num_str_length = MIN_LENGTH;
        }

        const std::string fpe_key_str = key.empty() ? FPETool::DEFAULT_KEY : key;
        int fpe_key_length = fpe_key_str.length();

        std::vector<uint8_t> fpe_key(fpe_key_length);
        for (size_t i = 0; i < fpe_key_length; ++i) {
            fpe_key[i] = static_cast<uint8_t>(fpe_key_str[i]);
        }


        int res;
        struct ff1_ctx* ctx;
        char* out;

        out = (char *) calloc(num_str_length + 1, sizeof(char));
        res = ff1_ctx_create(&ctx, fpe_key.data(), fpe_key_length, FPETool::TWEAK, sizeof(FPETool::TWEAK), 0, SIZE_MAX, 10);
        if (res == 0) {
            ff1_encrypt(ctx, out, fixed_num_str.c_str(), NULL, 0);
            ff1_ctx_destroy(ctx);
        }

        std::string result(out);
        free(out);

        return result;
    }

    static std::string encrypt_num(double number, const std::string& key) {

        // 数字转换为字符串时，避免精度的丢失
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(PRECISION) << number;
        std::string num_str = stream.str();

//        std::string num_str;
//        if (std::floor(number) == number) {
//            num_str = std::to_string(static_cast<long long>(number));
//        } else {
//            num_str = std::to_string(number);
//        }

        std::string num_flag;
        if (num_str[0] == '-') {
            num_flag = "-";
            num_str = num_str.substr(1);
        }

        size_t dot_pos = num_str.find('.');
        std::string int_part = num_str;
        std::string dec_part;

        if (dot_pos != std::string::npos) {
            int_part = num_str.substr(0, dot_pos);
            dec_part = num_str.substr(dot_pos + 1);
            std::cout << "1. dec_part1 length: " << dec_part.length() << std::endl;
            //long long dec_part_num = std::stoll(dec_part) * std::pow(10, 9);
            //dec_part = std::to_string(dec_part_num);
             dec_part = dec_part + std::string(9 - dec_part.length(), '0');
             std::cout << "2. dec_part2 length: " << dec_part.length() << std::endl;
        }

        std::string encrypted_int_Part = encrypt(int_part, key);
        std::string encrypted_dec_part = dec_part.empty() ? "" : "." + encrypt(dec_part, key);

        return num_flag + encrypted_int_Part + encrypted_dec_part;
    }

    static std::string decrypt(const std::string& num_str, const std::string& key) {
        int num_str_length = num_str.length();

        const std::string fpe_key_str = key.empty() ? FPETool::DEFAULT_KEY : key;
        int fpe_key_length = fpe_key_str.length();

        std::vector<uint8_t> fpe_key(fpe_key_length);
        for (size_t i = 0; i < fpe_key_length; ++i) {
            fpe_key[i] = static_cast<uint8_t>(fpe_key_str[i]);
        }

        int res;
        struct ff1_ctx* ctx;
        char* out;

        out = (char *) calloc(num_str_length + 1, sizeof(char));
        res = ff1_ctx_create(&ctx, fpe_key.data(), fpe_key_length, FPETool::TWEAK, sizeof(FPETool::TWEAK), 0, SIZE_MAX, 10);
        if (res == 0) {
            ff1_decrypt(ctx, out, num_str.c_str(), NULL, 0);
            ff1_ctx_destroy(ctx);
        }

        std::string result(out);
        free(out);

        return result;
    }

    static std::string decrypt_num(double number , const std::string& key) {
        std::string num_str;
        if (std::floor(number) == number) {
            num_str = std::to_string(static_cast<long long>(number));
        } else {
            num_str = std::to_string(number);
        }

        std::string num_flag;
        if (num_str[0] == '-') {
            num_flag = "-";
            num_str = num_str.substr(1);
        }

        size_t dot_pos = num_str.find('.');
        std::string int_part = num_str;
        std::string dec_part;
        if (dot_pos != std::string::npos) {
            int_part = num_str.substr(0, dot_pos);
            dec_part = num_str.substr(dot_pos + 1);
            long long dec_part_num = std::stoll(decrypt(dec_part, key));
            dec_part = std::to_string(static_cast<double>(dec_part_num) / std::pow(10, 9));
        }

        std::string decrypted_int_part = decrypt(int_part, key);
        std::string decrypted_num = num_flag + decrypted_int_part + (dec_part.empty() ? "" : "." + dec_part);

        return decrypted_num;
    }

private:
    static const int PRECISION = 15;
    static const std::string DEFAULT_KEY;
    static const int MIN_LENGTH = 6;
    constexpr static const uint8_t TWEAK[] = {};
};

const std::string FPETool::DEFAULT_KEY = "abcdefghijk12345abcdefghijk12345";

int main() {

    std::cout << "Liuruirui" << std::endl;

    std::string key = "abcdefghijk12345abcdefghijk12345";

    std::string value = "123";
    std::string encrypt_str = FPETool::encrypt(value, key);
    std::cout << "encrypt_str:" << encrypt_str << std::endl;
    std::string decrypt_str = FPETool::decrypt(encrypt_str, key);
    std::cout << "decrypt_str:" << decrypt_str << std::endl;

//    double num = 1234567890.12;
//    std::cout << "original_str:" << std::to_string(num) << std::endl;
//
//    std::string encrypt_str = FPETool::encrypt_num(num, key);
//    std::cout << "encrypt_str:" << encrypt_str << std::endl;
//
//    std::string decrypt_str = FPETool::decrypt_num(std::stod(encrypt_str), key);
//    std::cout << "decrypt_str:" << decrypt_str << std::endl;

    return 0;
}
