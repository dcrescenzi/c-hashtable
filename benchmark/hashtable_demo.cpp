/*
Author: Dante Crescenzi
Last Modif: 28 Mar 2024
Description: demo of hashtable usage

tests the insertion/lookup of 2^20 random 4 byte strings
ignore the errors in this file, they are not real.
*/

#include "../hashtable.h"
#include <unordered_map>
#include <chrono>
#include <string>
#include <iostream>

//NOTE: NOT MY CODE - SOURCED FROM https://codereview.stackexchange.com/questions/29198/random-string-generator-in-c
char *randstring(size_t length) {
    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";        
    char *randomString = NULL;
    if (length) {
        randomString = (char*)malloc(sizeof(char) * (length +1));
        if (randomString) {            
            for (int n = 0;n < length;n++) {            
                int key = rand() % (int)(sizeof(charset) -1);
                randomString[n] = charset[key];
            }
            randomString[length] = '\0';
        }
    }
    return randomString;
}
//END NOTE: NOT MY CODE

int main(int argc, char** argv)
{
    //init rand strings
    int strlen = 30;
    int numstr = 1 << 18;
    bool default_size = false;
    //definitley a better way to do this, but im lazy
    for(int i = 0; i < argc; i++)
    {
        switch (i)
        {
            case 1:
            {
                strlen = std::stoi(std::string(argv[i]));
                break;
            }
            case 2:
            {
                numstr = 1 << std::stoi(std::string(argv[i]));
                break;
            }
            case 3:
            {
                default_size = std::string(argv[i]) == "true";
                break;
            }
            default: break;
        }
    }

    char rand_keys[numstr][strlen+1];
    for(int i = 0; i < numstr; i++)
    {
        char* str = randstring(strlen);
        strcpy(rand_keys[i], str);
    }

    std::cout << "insertion/lookup of " << numstr << " random generated " << strlen << "-char strs\n";

    //HOMEMADE HASHTABLE ===============
    auto start = std::chrono::high_resolution_clock::now();
    hashtable_t* c_htb = hashtable_init(default_size ? 1 : numstr);
    for(int i = 0; i < numstr; i++)
    {
        cell_info_t lookup = hashtable_lookup(c_htb, rand_keys[i]);
        if(lookup.status == OK)
        {
            lookup.cell->value++;
            continue;
        }
        hashtable_insert(c_htb, rand_keys[i], 0);
    }
    auto end = std::chrono::high_resolution_clock::now();

    hashtable_cleanup(c_htb);
    //==================================

    double time_taken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    time_taken *= 1e-9;
    std::cout << "time taken by C hashtable:\t" << time_taken << " sec\n";

    //C++ HASHTABLE ====================
    start = std::chrono::high_resolution_clock::now();
    std::unordered_map<std::string, int> cpp_htb;
    cpp_htb.reserve(default_size ? 1 : numstr);
    for(int i = 0; i < numstr; i++)
    {
        cpp_htb[std::string(rand_keys[i])]++;
    }
    end = std::chrono::high_resolution_clock::now();
    //==================================

    time_taken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    time_taken *= 1e-9;
    std::cout << "time taken by C++ hashtable:\t" << time_taken << " sec\n";

    return 0;
}