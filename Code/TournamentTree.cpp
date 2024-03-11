#include <queue>
#include <vector>
#include <unistd.h>
#include <optional>
#include "Sort.h"
#include "RecordStructure.h"
#include "TournamentTree.h"
#include "constants.h"
#include "defs.h"
#define OVC_MULTIPLICATION_FACTOR_1 10000
#define OVC_MULTIPLICATION_FACTOR_2 10

using namespace std;

TournamentTree::TournamentTree() {}

TournamentTree::TournamentTree(TournamentTreeNode a[], int size, SSD *ssd, DRAM *dram, Cache cache)
{
    this->num_nodes = size;
    this->tournamentTree = a;
    this->cache = cache;
    this->dram = dram;
    this->ssd = ssd;
    this->fieldLength = (inputrecordsize)/3;
}

TournamentTreeNode *TournamentTree::getTree()
{
    return this->tournamentTree;
}
int TournamentTree::left(int i) { return (2 * i); }
int TournamentTree::right(int i) { return (2 * i + 1); }
int TournamentTree::getNodeCount() { return num_nodes; }
// TournamentTreeNode TournamentTree::getMin() { return tournamentTree[0]; }

bool TournamentTree::isLeafNode(int index)
{
    return left(index) >= num_nodes || right(index) >= num_nodes;
}

int TournamentTree::getParentIndex(int i)
{
    return floor(i / 2);
}

void TournamentTree::printTreeOfLosers()
{
    for (int i = 0; i < num_nodes; i++)
    {
        printf("%d\t%s\t%d\t%d\n", i, tournamentTree[i].element.members[0].c_str(),tournamentTree[i].offsetValueCode, tournamentTree[i].offset);
    }
}

void TournamentTree::computeOffsetUtil(int nodeIndex, int offset){
    tournamentTree[nodeIndex].offsetValueCode = (fieldLength - offset) * OVC_MULTIPLICATION_FACTOR_1;
    if(offset < fieldLength){
        tournamentTree[nodeIndex].offsetValueCode += offset * OVC_MULTIPLICATION_FACTOR_2;
        tournamentTree[nodeIndex].offsetValueCode += (int)tournamentTree[nodeIndex].element.members[0][offset] - 48;
    }
}

void TournamentTree::computeOffsetForLoser(int loserIndex, int winnerIndex)
{
    int offset = 0;
    fullComparison(loserIndex, winnerIndex, offset);
    computeOffsetUtil(loserIndex, offset);
}

void TournamentTree::computeOffsetForLoser(TournamentTreeNode &loser, TournamentTreeNode &winner)
{
    int offset = 0;
    fullComparison(loser, winner, offset);

    loser.offsetValueCode = (fieldLength - offset) * OVC_MULTIPLICATION_FACTOR_1;
    if(offset < fieldLength){
        loser.offsetValueCode += offset * OVC_MULTIPLICATION_FACTOR_2;
        loser.offsetValueCode += (int)loser.element.members[0][offset] - 48;
    }
}

void TournamentTree::computeOffsetValueCode(int node_index)
{
    int offset = 0;
    while(offset < fieldLength){
        if(tournamentTree[node_index].element.members[0][offset] != '0')
            break;
        offset++;
    }

    tournamentTree[node_index].offsetValueCode = (fieldLength - offset) * OVC_MULTIPLICATION_FACTOR_1;
    if(offset < fieldLength) {
        tournamentTree[node_index].offsetValueCode += offset * OVC_MULTIPLICATION_FACTOR_2;
        tournamentTree[node_index].offsetValueCode += (int)tournamentTree[node_index].element.members[0][offset] - 48;
    }
}

void TournamentTree::compareNodeValues(int left, int right)
{
    int parent_index = getParentIndex(left);
    bool computeNewOffsetValueCode = false;
    if (isLeftLesserThanRight(left, right, computeNewOffsetValueCode)){
         
        if (computeNewOffsetValueCode)
            computeOffsetForLoser(right, left);
        tournamentTree[parent_index] = tournamentTree[left];
    }
    else
    {
        computeOffsetForLoser(left, right);
        tournamentTree[parent_index] = tournamentTree[right];
    }
}

void TournamentTree::fullComparison(TournamentTreeNode &left, TournamentTreeNode &right, int& offset)
{
    while (offset < fieldLength && left.element.members[0][offset] == right.element.members[0][offset])
    {
        offset++;
    }
}

void TournamentTree::fullComparison(int left, int right, int& offset)
{
    while (offset < fieldLength && tournamentTree[left].element.members[0][offset] == tournamentTree[right].element.members[0][offset])
    {
        offset++;
    }
}

bool TournamentTree::isLeftLesserThanRight(TournamentTreeNode &left, TournamentTreeNode &right, bool &computeNewOffsetValueCode){
    if (left.offsetValueCode != -1 && right.offsetValueCode != -1){
        computeNewOffsetValueCode = false;
        if (left.offsetValueCode == right.offsetValueCode){
            // do full comparison starting at offset
            int offset = 0;
            fullComparison(left, right, offset);
            computeNewOffsetValueCode = true;
            return offset < fieldLength && ((int)left.element.members[0][offset] < (int)right.element.members[0][offset]);
        }
        else
        {
            full_string_comparisons_avoided++;
            return left.offsetValueCode < right.offsetValueCode;
        }
        
    }
    else{
        int offset = 0;
        fullComparison(left, right, offset);
        computeNewOffsetValueCode = false;
        bool ans = offset < fieldLength && ((int)left.element.members[0][offset] < (int)right.element.members[0][offset]);
        if(ans){
            right.offsetValueCode = (fieldLength - offset) * OVC_MULTIPLICATION_FACTOR_1;
            if(offset < fieldLength){
                right.offsetValueCode += offset * OVC_MULTIPLICATION_FACTOR_2;
                right.offsetValueCode += (int)right.element.members[0][offset] - 48;
            }        
        } else {
            left.offsetValueCode = (fieldLength - offset) * OVC_MULTIPLICATION_FACTOR_1;
            if(offset < fieldLength){
                left.offsetValueCode += offset * OVC_MULTIPLICATION_FACTOR_2;
                left.offsetValueCode += (int)left.element.members[0][offset] - 48;
            }  
        }
        return ans;
    }
}

bool TournamentTree::isLeftLesserThanRight(int left, int right, bool &computeNewOffsetValueCode)
{
    // check if nodes have the offsets computed
    // case 1: Yes
    
    if (tournamentTree[left].offsetValueCode != -1 && tournamentTree[right].offsetValueCode != -1){
        if (tournamentTree[left].offsetValueCode == tournamentTree[right].offsetValueCode){
            // do full comparison
            int offset = 0;
            fullComparison(left, right, offset);
            computeNewOffsetValueCode = true;
            return offset < fieldLength && (tournamentTree[left].element.members[0][offset] < tournamentTree[right].element.members[0][offset]);
        }
        else
        {
            full_string_comparisons_avoided++;
            return tournamentTree[left].offsetValueCode < tournamentTree[right].offsetValueCode;
        }
        computeNewOffsetValueCode = false;
    }
    else {
        computeNewOffsetValueCode = true;

        int offset = 0;
        fullComparison(left, right, offset); 
        computeNewOffsetValueCode = false;
        bool ans = offset < fieldLength && ((int)tournamentTree[left].element.members[0][offset] < (int)tournamentTree[right].element.members[0][offset]);
        if(ans){
            tournamentTree[right].offsetValueCode = (fieldLength - offset) * OVC_MULTIPLICATION_FACTOR_1;
            if(offset < fieldLength){
                tournamentTree[right].offsetValueCode += offset * OVC_MULTIPLICATION_FACTOR_2;
                tournamentTree[right].offsetValueCode += (int)tournamentTree[right].element.members[0][offset] - 48;
            }  
        } else {
           tournamentTree[left].offsetValueCode = (fieldLength - offset) * OVC_MULTIPLICATION_FACTOR_1;
            if(offset < fieldLength){
                tournamentTree[left].offsetValueCode += offset * OVC_MULTIPLICATION_FACTOR_2;
                tournamentTree[left].offsetValueCode += (int)tournamentTree[left].element.members[0][offset] - 48;
            }
        }
        return ans;
    }
    /*
        If a.OVC is computed && b.OVC is computed:
            Compare a and b using OVC
        a.OVC == b.OVC
                    Compare values starting at a.offset for both; return the smaller as winner
        a.OVC != b.OVC
        Return the smaller as the winner
    */

    // case 2:
    /*
        One of the nodes dont have the OVC computed
        Do full comparison
        Compute OVC of loser w.r.t winner
    */
   return false;
}

void TournamentTree::compareInitialNodes()
{
    for (int i = this->num_nodes - 1; i > 1; i -= 2)
    {
        compareNodeValues(i, i - 1);
    }
}

void TournamentTree::setRootNode()
{
    tournamentTree[0] = tournamentTree[1];
}

bool TournamentTree::areNodesEqual(int a, int b)
{
    return (tournamentTree[a].runId == tournamentTree[b].runId && tournamentTree[a].element.members[0] == tournamentTree[b].element.members[0]);
}

void TournamentTree::initialPop(int index)
{
    if (isLeafNode(index))
        return;
    tournamentTree[index] = (areNodesEqual(index, left(index))) ? tournamentTree[right(index)] : tournamentTree[left(index)];
    initialPop(left(index));
    initialPop(right(index));
}

void TournamentTree::updateTreeStructure(TournamentTreeNode &node, int parent_index, TournamentTreeNode &parent)
{
    if (parent_index == 0)
    {
        tournamentTree[0] = node;
        return;
    }
    TournamentTreeNode larger, smaller, grandParent;
    int grand_parent_index = getParentIndex(parent_index), smaller_index;
    bool computeNewOffsetValueCode = false;
    if (isLeftLesserThanRight(node, parent, computeNewOffsetValueCode))
    {
        
        larger = parent;
        smaller = node;
    }
    else
    {
        larger = node;
        smaller = parent;
    }
    if (computeNewOffsetValueCode)
        computeOffsetForLoser(larger, smaller);
  
    grandParent = tournamentTree[grand_parent_index];
    tournamentTree[parent_index] = larger;

    updateTreeStructure(smaller, grand_parent_index, grandParent);
}

FILE *openFile(char *fileName, char *mode)
{
    FILE *fp = fopen(fileName, mode);
    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    return fp;
}

void TournamentTree::readNextValueFromRunUtil(vector< queue<RecordStructure> > &in, int k, int run_id)
{
    if (!in[run_id].empty()){
        tournamentTree[k + run_id].element = in[run_id].front();
        tournamentTree[k + run_id].runId = run_id;
        TRACE(true);
	    traceprintf("Loading cache array for run:%d with %d elements.\n", run_id, in[run_id].size());
        in[run_id].pop();
    }
    else
    {
        // load data into this cache_array for run i
        // check the DRAM offsets if data needs to be loaded from SSD
        if (cache.read(run_id) == -1){
            // Data read from DRAM is not successful
            // no records in DRAM

            if(dram == nullptr) {
                pushLateFence(in, k, run_id);
                return;
            }

            int records_loaded_into_dram = dram->read(run_id);
            if (records_loaded_into_dram == -1){

                // Partition - run_id in SSD is sorted!
                // All the data in this run has been read.
                // Pass late fence - NULL vector/ empty vector for termination of the run
                if(ssd == nullptr) {
                    return pushLateFence(in, k, run_id);
                }

                int records_loaded_into_ssd = ssd->read(run_id);
                if(records_loaded_into_ssd == -1) {
                    return pushLateFence(in, k, run_id);
                }
                dram->resetReadOffset(run_id);
                dram->setRecordsInPartition(run_id, records_loaded_into_ssd);
                records_loaded_into_dram = dram->read(run_id);
            }
            
            cache.setRecordsInPartition(run_id, records_loaded_into_dram);
            cache.resetReadOffset(run_id);
            cache.read(run_id); 
            // refilling the cache from DRAM since it failed earlier
        }
        in[run_id] = cache.loadDataForRun(run_id);

        if(in[run_id].size() == 0){
            RecordStructure lateFence = {LATE_FENCE_VALUE, LATE_FENCE_VALUE, LATE_FENCE_VALUE, "9"}; 
            in[run_id].push(lateFence);
            tournamentTree[k + run_id].element = in[run_id].front();
            tournamentTree[k + run_id].runId = run_id;
            in[run_id].pop();
            // cout<<"Pushed late fence for run "<<run_id<<endl;
        } else {
            tournamentTree[k + run_id].element = in[run_id].front();
            tournamentTree[k + run_id].runId = run_id;
            in[run_id].pop();
            // cout<<"queue size of partition: "<< run_id<<" :"<<in[run_id].size()<<endl;
        }

    }
}

void TournamentTree:: pushLateFence(vector< queue<RecordStructure> > &in, int k, int run_id)
{
    RecordStructure lateFence = {LATE_FENCE_VALUE, LATE_FENCE_VALUE, LATE_FENCE_VALUE};
    in[run_id].push(lateFence);
    tournamentTree[k + run_id].element = in[run_id].front();
    tournamentTree[k + run_id].runId = run_id;
    in[run_id].pop();
}

void TournamentTree::readNextValueFromRun(vector<queue<RecordStructure> > &in, int k, int run_id)
{
    // Note the runId needs to be set explicitly here
    tournamentTree[k + run_id].offsetValueCode = -1;
    tournamentTree[k + run_id].offset = 0;
    readNextValueFromRunUtil(in, k, run_id);

    TRACE(true);
    traceprintf("New value read = %s \n", tournamentTree[k + run_id].element.members[0].c_str());
    printf("Its offset = %d", tournamentTree[k + run_id].offsetValueCode);
}

void appendDataToDestination(string sourceFilePath, string destinationFilePath)
{
    ifstream inputFile(sourceFilePath);
	ofstream outputFile(destinationFilePath, ios::app);
	if (inputFile.is_open() && outputFile.is_open()){
		outputFile << inputFile.rdbuf();
		inputFile.close();
		outputFile.close();
	}
	else
	{
		traceprintf("cannot open files to evict output data\n");
	}
}

bool isFileFull(string filePath, long sizeToCheck)
{
    FILE *file = fopen(filePath.c_str(), "r");
    if (file)
    {
        fseek(file, 0, SEEK_END); // Move to the end of the file
        long size = ftell(file); // Get the current position, which is the size of the file

        if (size >= sizeToCheck){
            fclose(file);
            return true;
        }
        fclose(file);
    }
    else
    {
        printf("Error opening the file.\n");
    }

    return false;
}

void truncateFile(string sourceFile)
{
    ofstream clearFile(sourceFile, ofstream::trunc);
    clearFile.close();
}

void TournamentTree::writeSortedRecordToFile(RecordStructure rs)
{
    /*
    Check if DRAM_OUT has 8KB of data.
        If yes,
            check if SSD_OUT has 1 MB of data
            If yes,
                append the contents of SSD_OUT to HDD_OUT
                Truncate SSD_OUT data
            Append the contents of DRAM_OUT to SSD_OUT
            Truncate DRAM_OUT data
        Append this new record to DRAM_OUT
    */
    if (isFileFull(DRAM_OUT_FILE_NAME, RoundDown(16 * 1024, recordsize))){
        if (isFileFull(SSD_OUT_FILE_NAME, RoundDown(4*1024 * 1024, recordsize))){

            if (ssd != nullptr){
                appendDataToDestination(SSD_OUT_FILE_NAME, HDD_OUT2_FILE_NAME);
            }
            else
            {
                appendDataToDestination(SSD_OUT_FILE_NAME, HDD_OUT_FILE_NAME);
            }

            TRACE(true);
	        traceprintf("Spilling SSD data to Hard disk.\n");
            truncateFile(SSD_OUT_FILE_NAME.c_str());
        }
        appendDataToDestination(DRAM_OUT_FILE_NAME, SSD_OUT_FILE_NAME);
        TRACE(true);
	    traceprintf("Spilling DRAM data to SSD.\n");
        truncateFile(DRAM_OUT_FILE_NAME);
    }
    FILE *file = fopen(DRAM_OUT_FILE_NAME.c_str(), "a");
    
    fprintf(file, "%s,%s,%s", rs.members[0].c_str(), rs.members[1].c_str(), rs.members[2].c_str());
    if(!rs.members[3].empty()){
        fprintf(file, ",%s", rs.members[3].c_str());
    }
    fprintf(file, "\n");
    fclose(file);
}

void TournamentTree::performTreeOfLosersSort(vector<queue<RecordStructure> > &in, int k) {

	    traceprintf("Starting Tree of losers comparison.\n");
    while (tournamentTree[0].element.members[0] != LATE_FENCE_VALUE) {
        writeSortedRecordToFile(tournamentTree[0].element);
        readNextValueFromRun(in, k, tournamentTree[0].runId);

        updateTreeStructure(
            tournamentTree[k + tournamentTree[0].runId],
            getParentIndex(k + tournamentTree[0].runId),
            tournamentTree[getParentIndex(k + tournamentTree[0].runId)]);

    }
    cout<<"Total full string comparisons avoided = "<<full_string_comparisons_avoided<<endl;
}

void mergeFiles(SSD *ssd, DRAM *dram, Cache cache, vector<queue<RecordStructure> > &cache_array, int k)
{
    // Create a tournament tree with k nodes. Every leaf node
    // has first element of a run

    // k -> denotes the #runs
    // There are 2k nodes in the tree
    TournamentTreeNode *tournamentTree = new TournamentTreeNode[2 * k];
    int i;

    TournamentTree tt(tournamentTree, 2 * k, ssd, dram, cache);
    for (i = 0; i < k; i++){
        tt.readNextValueFromRunUtil(cache_array, k, i);
    }

    for (int i = k; i < 2 * k; i++){
        tt.computeOffsetValueCode(i);
    }
    tt.compareInitialNodes();
    tt.setRootNode();
    tt.initialPop(1);
    

    tt.performTreeOfLosersSort(cache_array, k);
    // cout<<"\n***********************\n";
    // tt.printTreeOfLosers();
    // cout<<"\n***********************\n";

    appendDataToDestination(DRAM_OUT_FILE_NAME, SSD_OUT_FILE_NAME);
    truncateFile(DRAM_OUT_FILE_NAME);

    if (ssd != nullptr)
    {
        appendDataToDestination(SSD_OUT_FILE_NAME, HDD_OUT2_FILE_NAME);
    }
    else
    {
        appendDataToDestination(SSD_OUT_FILE_NAME, HDD_OUT_FILE_NAME);
    }
    truncateFile(SSD_OUT_FILE_NAME);

    // TRACE(true);
    traceprintf("Yaay! Merge is done!\n");

    
}

void externalSort(SSD *ssd, DRAM *dram, Cache cache, uint num_ways)
{
    // Convert the cache.txt file to array and pass to below function
    vector<queue<RecordStructure> > cache_array;
    for (int i = 0; i < num_ways; i++)
    {
        cache_array.push_back(cache.loadDataForRun(i));
    }
    		// TRACE(true);
	    traceprintf("Runs are generated for external merge.\n");
    /*
        cache_array[i] = vector<RecordStructure> -> stores records of the ith partition
        cache_array[i][j] -> stores an individual record
    */
    mergeFiles(ssd, dram, cache, cache_array, num_ways);
}
