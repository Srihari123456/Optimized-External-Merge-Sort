#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
const std::string CACHE_FILE_NAME = "./in/CACHE.txt";
const std::string DRAM_FILE_NAME = "./in/DRAM.txt";
const std::string SSD_FILE_NAME = "./in/SSD.txt";
const std::string HDD_FILE_NAME = "./in/HDD.txt";
const std::string DRAM_OUT_FILE_NAME = "./out/DRAM_OUT.txt";
const std::string SSD_OUT_FILE_NAME = "./out/SSD_OUT.txt";
const std::string HDD_OUT_FILE_NAME = "./out/HDD_OUT.txt";
const std::string HDD_OUT2_FILE_NAME = "./out/HDD_OUT2.txt";
const std::string LATE_FENCE_VALUE = "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
const uint64_t CACHE_SIZE_IN_BYTES = 1 << 20; // 10<<10; 1 << 20 = 1 MB
const uint64_t DRAM_SIZE_IN_BYTES = 100 << 20; // 100 MB
const uint64_t SSD_SIZE_IN_BYTES = ((uint64_t)10) << 30; // 10 GB
const std::string test = "tst";
#endif