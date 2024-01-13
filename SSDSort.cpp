
#include "defs.h"
#include "SSDSort.h"
#include "unistd.h"
#include "constants.h"
#include "internal_sort.h"
#include "TournamentTree.h"

using namespace std;

SSDSortPlan::SSDSortPlan(RowCount produced, RowCount consumed) : _produced(produced), _consumed(consumed)
{
	TRACE(true);
} // SortPlan::SortPlan

SSDSortPlan::~SSDSortPlan()
{
	TRACE(true);
} // SortPlan::~SortPlan

Iterator *SSDSortPlan::init() const
{
	TRACE(true);
	return new SSDSortIterator(this);
} // SortPlan::init

SSDSortIterator::SSDSortIterator(SSDSortPlan const *const plan) : _plan(plan), _produced(plan->_produced),_consumed(plan->_consumed), batches(0)
{
	TRACE(true);

} // SortIterator::SortIterator

SSDSortIterator::~SSDSortIterator()
{
	TRACE(true);

	traceprintf("produced %lu of %lu rows\n",
				(unsigned long)(_produced),
				(unsigned long)(_consumed));
} // SortIterator::~SortIterator

void appendDataAndTruncate(string sourceFilePath, string destinationFilePath)
{
    ifstream inputFile(sourceFilePath);
	ofstream outputFile(destinationFilePath, ios::app);
	if (inputFile.is_open() && outputFile.is_open())
	{
		outputFile << inputFile.rdbuf();
		inputFile.close();
		outputFile.close();
	}
	else
	{
		traceprintf("cannot open files to evict output data\n");
	}
	ofstream clearFile(sourceFilePath, ofstream::trunc);
    clearFile.close();
}

// external sort
bool SSDSortIterator::next()
{
	TRACE(true);
	if (_produced >= _plan->_consumed)
		return false;
	internalSort();
	if(batches > 1) {
		traceprintf("spilling to ssd as data doesnt fit in dram\n");
		externalMerge();
	}
	else {
		// move SSD.txt to HDD_out.TXT
		traceprintf("moving data to output file HDD_OUT\n");
		appendDataAndTruncate(SSD_FILE_NAME, HDD_OUT_FILE_NAME);
	}
	batches = 0;
	return true;
} // SortIterator::next

uint getMaxSSDBatches()
{
	uint partitions = (0.999*SSD_SIZE_IN_BYTES) / RoundDown(DRAM_SIZE_IN_BYTES, recordsize);
	return partitions;
}

bool SSDSortIterator::internalSort()
{
	streamoff hddOffset = _produced*recordsize;
	DRAM dram(hddOffset);
	uint max_batches = getMaxSSDBatches();

	traceprintf("sorting DRAM size runs internally\n");
	// cout<<"_produced "<<_produced<<"_consumed "<<_consumed<<"batches "<<batches<<" max_batches "<<max_batches<<endl;
	while ((_produced < _plan->_consumed) && (batches < max_batches))
	{
		std::vector<RecordStructure> arr;
		uint dramRecords = divide(RoundDown(DRAM_SIZE_IN_BYTES, recordsize), (size_t)recordsize);
		uint recordsToConsume = (_consumed - _produced) > dramRecords ? dramRecords : (_consumed - _produced);

		// cout << "dram records " << dramRecords << "rec to consusc " << recordsToConsume << endl;
		dram.loadFromHDD(recordsToConsume);
		// cout << "loading ram with rec " << recordsToConsume << endl;
		traceprintf("start sorting dram batch %d\n", batches+1);

		arr = read_ramfile();
		quickSort(arr, customComparator);
		write_ramfile(arr);
		arr.clear(); // Remove all elements

		traceprintf("end sorting dram batch %d with records in batch \n", batches+1, recordsToConsume);
		_produced = _produced + recordsToConsume;
		if (!copyRamToHDD())
			exit(1);
		dram.clearRam();

		batches++;
		// cout << "current batch " << batches << endl;
	}
	return true;
}

bool SSDSortIterator::copyRamToHDD()
{
	ifstream inputFile(DRAM_FILE_NAME);
	ofstream outputHDDFile(SSD_FILE_NAME, ios::app);
	if (inputFile.is_open() && outputHDDFile.is_open())
	{
		outputHDDFile << inputFile.rdbuf();
		inputFile.close();
		outputHDDFile.close();
		// clearRam();
	}
	else
	{
		cout << "cannot open files to evict ram" << endl;
		return false;
	}
	return true;
}

int SSDSortIterator::externalMerge()
{
	TRACE(true);
	traceprintf("start merging dram size runs of %d to ssd \n", batches);

	SSD *ssd = NULL;
	//load dram all partitions
	DRAM dram_merge(batches);
	for (int i = 0; i < batches; i++)
	{
		dram_merge.read(i);
	}

	//load cache all partitions
	Cache cache_merge(batches);
	for (int i = 0; i < batches; i++)
	{
		cache_merge.read(i);
	}

	externalSort(ssd, &dram_merge, cache_merge, batches);

	//clear memory for next batch 
	SSD ssd_temp;
	ssd_temp.clearSSD();
	cache_merge.clearCache();
    dram_merge.clearRam();

	traceprintf("end merging dram size runs of %d to ssd \n", batches);
	traceprintf("written output to file %s\n", HDD_OUT_FILE_NAME.c_str());
	return 0;
}