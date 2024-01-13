#pragma once
#include <fstream>
#include "Memory.h"

using namespace std;

class DRAM : public Memory
{
public:
    int read(int partition) override;
    void clearRam();
    DRAM(int NWAY, bool is_external_sort);
    DRAM(int NWAY);
    DRAM(streamoff hddOffset);
    DRAM();
    ~DRAM();
    void setRecordsInPartition(int partition, uint32_t records_count);
    void resetReadOffset(int partition);
    void loadFromHDD(uint recordsToConsume);

private:
    int _NWAY;
    streamoff *readOffsets;
    streamoff hddSortOffset; // for internal sorting
    uint32_t *records_in_partition;
    streamoff partition_size; //partition size in SSD
    int max_partition_size;
};
