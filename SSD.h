#include <fstream>
#include "Memory.h"

using namespace std;

class SSD : public Memory
{
public:
    int read(int partition) override;
    SSD(int NWAY);
    SSD();
    ~SSD();
    void clearSSD();

private:
    int _NWAY;
    streamoff *readOffsets;
    uint32_t *records_in_partition;
    uint64_t max_partition_size;
};
