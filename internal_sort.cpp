#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "constants.h"
#include "internal_sort.h"
#include "defs.h"
using namespace std;

bool customComparator(const RecordStructure &a, const RecordStructure &b)
{
    // if (a.members[0] != b.members[0])
    // {
        return a.members[0] < b.members[0];
    // }
}

std::vector<RecordStructure> read_ramfile()
{
    std::ifstream input_file(DRAM_FILE_NAME);

    if (!input_file)
    {
        std::cerr << "Error opening input file!" << std::endl;
    }
    input_file.seekg(0, ios::end);
    streampos end = input_file.tellg();
    
    input_file.seekg(0, ios::beg);
    // Read data from the file and store it in a vector of RecordStructure
    std::vector<RecordStructure> arr;
    std::string line, column;

    while (std::getline(input_file, line))
    {
        // cout<<line<<endl;
        std::istringstream ss(line);
        std::string token;
        RecordStructure record;

        int j = 0;
        while(std::getline(ss, column, ',')) {
            record.members[j] = column;//stoull(column);
            j++;
        }
        // char delimiter = ',';
        // ss >> record.members[0] >> delimiter;
        // ss >> record.members[1] >> delimiter;
        // ss >> record.members[2] >> delimiter;
        // ss >> record.members[3];
        // cout<<"Internal sort:: Record member[0].length = "<<record.members[0].length()<<endl;
        arr.push_back(record);
    }
    input_file.close();
    //cout << "array size after reading " << arr.size() << endl;
    return arr;
}
void write_ramfile(std::vector<RecordStructure> &arr)
{
    ofstream outFile(DRAM_FILE_NAME);

    if (!outFile.is_open())
    {
        std::cerr << "Error opening output file!" << std::endl;
    }

    //cout << "array size after internal sorting "<<arr.size() << endl;
    // Write the sorted data back to the file
    for (size_t i = 0; i < arr.size(); ++i)
    {
        const RecordStructure &record = arr[i];
        outFile << record.members[0] << "," << record.members[1] << "," << record.members[2];
        if(!record.members[3].empty()){
            outFile << "," << record.members[3];
        }
        outFile << "\n";
    }
    outFile.close();
    		TRACE(true);
	    traceprintf("All internally sorted records, written back to DRAM.\n");

}

void quickSort(std::vector<RecordStructure> &arr, bool (*comparator)(const RecordStructure &, const RecordStructure &))
{

    qSort(arr, 0, arr.size() - 1, comparator);
}

void qSort(std::vector<RecordStructure> &arr, int left, int right, bool (*comparator)(const RecordStructure &, const RecordStructure &))
{

    int pivot;

    if (left < right)
    {

        pivot = partition(arr, left, right, comparator);

        qSort(arr, left, pivot - 1, comparator);
        qSort(arr, pivot + 1, right, comparator);
    }
}

int partition(std::vector<RecordStructure> &arr, int left, int right, bool (*comparator)(const RecordStructure &, const RecordStructure &))
{

    std::string pivot = arr[right].members[0];
    int i = left - 1;

    for (int j = left; j < right; j++)
    {
        if (comparator(arr[j], arr[right]))
        {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }

    std::swap(arr[i + 1], arr[right]);

    return i + 1;
}