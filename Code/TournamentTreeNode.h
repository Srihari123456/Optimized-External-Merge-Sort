#include "RecordStructure.h"

struct TournamentTreeNode
{
    RecordStructure element;
    int runId;
    int offsetValueCode = -1;
    // OVC = this.Offset*Domain + (Domain - element.members[0][offset])
    int offset = 0;
};