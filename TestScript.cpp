#include <iostream>
#include <fstream>
#include <string>

#include <stdlib.h>
#include <set>
#include <unordered_set>
#include <limits>
#include "constants.h"
#include "Sort.h"
#include "internal_sort.h"

using namespace std;


//Verification of sets of rows and values
bool verifySets(string inputFile, string outputFile) {
    std::ifstream input(inputFile);
    std::ifstream output(outputFile);

    if (!input.is_open() || !output.is_open()) {
        std::cerr << "Error opening files." << std::endl;
        return false;
    }

    std::set<std::string> inputRows;

    // Read rows from input file and insert into the set
    std::string inputRow;
    // cout<<"Input Row count = "<<inputRows.size();
    while (std::getline(input, inputRow)) {
        inputRows.insert(inputRow);
    }
    cout<<"Input Row count = "<<inputRows.size();

    // Check if each row in the output file exists in the set
    std::string outputRow;
    while (std::getline(output, outputRow)) {
        if (inputRows.find(outputRow) == inputRows.end()) {
            std::cerr << "Mismatch found: Row in output file not present in input file." << std::endl;
            return false;
        } else {
            // cout<<"Output row is present\n";
        }
    }

    std::cout << "Sets of rows and values verified successfully." << std::endl;
    return true;
}

//Verification of Sort order
bool verifySortOrder(string outputFile) {
    std::ifstream output(outputFile);

    if (!output.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return false;
    }

    string previousValue = ".";//std::numeric_limits<int>::min();
    string currentValue;

    // Check if a[i] >= a[i-1] for each value in the output file
    int i = 0;
    while (output >> currentValue) {
        i++;
        if (currentValue < previousValue) {
            std::cerr << "Sort order mismatch found at "<<i<<"\t"<<currentValue << std::endl;
            return false;
        }
        previousValue = currentValue;
    }

    std::cout << "Sort order verified successfully." << std::endl;
    return true;
}

int main(){
    verifySortOrder(HDD_OUT_FILE_NAME);
    verifySets(HDD_FILE_NAME, HDD_OUT_FILE_NAME);
    return 0;
}
