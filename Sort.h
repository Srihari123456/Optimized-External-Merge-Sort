#include "Iterator.h"
typedef uint32_t uint;

class SortPlan : public Plan
{
	friend class SortIterator;

public:
	SortPlan(Plan *const input);
	~SortPlan();
	Iterator *init() const;

private:
	Plan *const _input;
}; // class SortPlan

class SortIterator : public Iterator
{
public:
	SortIterator(SortPlan const *const plan);
	~SortIterator();
	bool next();
	void ssdSort();
	void externalMerge();
	void clearTempFile();

private:
	SortPlan const *const _plan;
	Iterator *const _input;
	RowCount _consumed, _produced;
	int _recsize;
	uint runs;
}; // class SortIterator
