#include <fstream>
#include <string>
#include <iostream>
#include "constants.h"
#include "defs.h"
#include "DRAM.h"
using namespace std;

// check if file is open if not exit
template <typename StreamType>
void fileOpenCheck(StreamType &file, string fileName)
{
    if (!file || !file.is_open())
    {
        printf("cannot open the file %s exiting...\n", fileName.c_str());
        exit(EXIT_FAILURE);
    }
}

int DRAM::read(int partition)
{
    uint32_t records_to_consume = max_partition_size / recordsize;
    // cout<<"loading into dram: "<<partition<<endl;
    // check the records count left in SSD in this partition
    if (records_in_partition[partition] == 0) {
        // cout<<"dram cannt comsume from partition: "<<partition<<"records_to_consume "<<records_to_consume<<endl;
        // cout<<max_partition_size<<"  :  "<<readOffsets[partition]<<endl;
        traceprintf("exhausted current batch of records from ssd in partition %d\n", partition);
        return -1; // indicating end of partition

    }

    // records left in the partition is less than the partition size
    else if (records_in_partition[partition] < records_to_consume)
    {
        records_to_consume = records_in_partition[partition];
    }
    uint32_t block_size = records_to_consume*recordsize;

    streamoff dram_seek  = (uint64_t)max_partition_size * partition;
    traceprintf("loading %d records into dram from ssd for partition %d\n", records_to_consume, partition);
    // cout<<"records left "<<records_in_partition[partition]<<endl;
    // cout<<"seek offsets: dram "<<  dram_seek<< " ssd offset "<<readOffsets[partition]<<endl;

    ifstream inputFile(SSD_FILE_NAME);
    fstream dramFile(DRAM_FILE_NAME, ios::in | ios::out);
    fileOpenCheck(inputFile, SSD_FILE_NAME);
    fileOpenCheck(dramFile, DRAM_FILE_NAME);
    dramFile.seekp(dram_seek, ios::beg);
    inputFile.seekg(readOffsets[partition], ios::beg);

    /* Loading into ram in the 1MB chunks due to os limitations
    can be done in larger chunks if os supports */
    const int load_size = 1 << 20;
    char *readBuffer= new char[load_size];
    while (block_size > 0 && !inputFile.eof())
    {
        int read_block = block_size > load_size ? load_size : block_size;
        inputFile.read(readBuffer, read_block);

        dramFile.write(readBuffer, read_block);
        block_size -= read_block;
    }
    delete[] readBuffer;
  
    readOffsets[partition] = inputFile.tellg();
    // cout<<"ssd end point is "<<readOffsets[partition]<<endl;
    records_in_partition[partition] = records_in_partition[partition] - records_to_consume;
    dramFile.close();
    inputFile.close();
    return records_to_consume;
}

void DRAM::clearRam()
{
    ofstream clearRAM(DRAM_FILE_NAME, ofstream::trunc);
    clearRAM.close();
}
DRAM::DRAM() {
    readOffsets = NULL;
    records_in_partition = NULL;
}

DRAM::DRAM(streamoff hddOffset)
{
    hddSortOffset = hddOffset;
}

DRAM::DRAM(int NWAY) : _NWAY(NWAY)
{
    readOffsets = new streamoff[NWAY];
    records_in_partition = new uint32_t[NWAY];
    // initializing offsets for ssd
    partition_size = RoundDown(DRAM_SIZE_IN_BYTES, recordsize);
    uint32_t max_records = partition_size / recordsize;
    max_partition_size = RoundDown(DRAM_SIZE_IN_BYTES / NWAY, recordsize);
    TRACE(true);
    // cout << "Max partition size of DRAM firsr step = " << max_partition_size << endl;

    for (int i = 0; i < NWAY; i++)
    {
        readOffsets[i] = i * partition_size;
        records_in_partition[i] = max_records;
        if (i == NWAY - 1)
        {
            // last partition
            ifstream inputFile(SSD_FILE_NAME, ios::binary | ios::ate);
            streamoff ssd_size = inputFile.tellg();
            records_in_partition[i] = (ssd_size - readOffsets[i]) / recordsize;
            inputFile.close();
        }
    }
    // DRAM();
}

DRAM::DRAM(int NWAY, bool is_external_sort) : _NWAY(NWAY)
{
    if(!is_external_sort) {
        DRAM(NWAY);
        return;
    }

    readOffsets = new streamoff[NWAY];
    records_in_partition = new uint32_t[NWAY];
    // initializing offsets for ssd
    //leave out some space for output buffer in SSD 
    partition_size = RoundDown((0.999*SSD_SIZE_IN_BYTES) / NWAY, recordsize); 
    // cout<<"max partition size ssd in dram  "<<partition_size<<" final step"<<endl;
    uint32_t max_records = partition_size / recordsize;
    max_partition_size = RoundDown(DRAM_SIZE_IN_BYTES / NWAY, recordsize);
    // cout<<"max partition size dram "<<max_partition_size<<" final step"<<endl;
    // cout << "Max partition size of DRAM = " << max_partition_size << endl;

    for (int i = 0; i < NWAY; i++)
    {
        readOffsets[i] = i * partition_size;
        records_in_partition[i] = max_records;
        if (i == NWAY - 1)
        {
            // last partition
            ifstream inputFile(SSD_FILE_NAME, ios::binary | ios::ate);
            streamoff ssd_size = inputFile.tellg();
            records_in_partition[i] = (ssd_size - readOffsets[i]) / recordsize;
            inputFile.close();
        }
    }
    // DRAM();
}

DRAM::~DRAM()
{
    // delete readOffsets;
    // delete records_in_partition;
}

/* This function loads data from HDD in 100MB into DRAM for internal sorting*/
void DRAM::loadFromHDD(uint recordsToConsume)
{
    ifstream HDDFile(HDD_FILE_NAME, ios::binary);
    fileOpenCheck(HDDFile, HDD_FILE_NAME);
    HDDFile.seekg(hddSortOffset, ios::beg);
    ofstream DRAMFile(DRAM_FILE_NAME, ios::app);
    fileOpenCheck(DRAMFile, DRAM_FILE_NAME);
    const int blockSize = 1 << 20;
    char readBuffer[blockSize];

    uint32_t block_left = recordsToConsume * recordsize;
    while (block_left > 0 && !HDDFile.eof())
    {
        int read_block = block_left > blockSize ? blockSize : block_left;
        HDDFile.read(readBuffer, read_block);
        DRAMFile.write(readBuffer, read_block);
        block_left -= read_block;
    }

    hddSortOffset = HDDFile.tellg();
    // cout<<"Dram Hdd offset is "<<hddSortOffset<<endl;
    HDDFile.close();
    DRAMFile.close();
}

void DRAM::setRecordsInPartition(int partition, uint32_t records_count)
{
    this->records_in_partition[partition] = records_count;
}

void DRAM:: resetReadOffset(int partition) {
    this->readOffsets[partition] = partition_size*partition;
}