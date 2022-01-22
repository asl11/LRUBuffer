
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include "MyDB_Page.h"
#include <string>

using namespace std;

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr, long) {
	int freeIndex = get_free();
	MyDB_Page newPage(freeIndex, false, false, pageCount);
	allPages[pageCount] = newPage;
	LRU.push_front(pageCount);
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
	int freeIndex = get_free();
	MyDB_Page newPage(freeIndex, false, true, pageCount);	
	allPages[pageCount] = newPage;
	LRU.push_front(pageCount);


}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr, long) {
	int freeIndex = get_free();
	MyDB_Page newPage(freeIndex, true, false, pageCount);
	allPages[pageCount] = newPage;
	LRU.push_front(pageCount);


}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
	int freeIndex = get_free();
	MyDB_Page newPage(freeIndex, true, true, pageCount);
	allPages[pageCount] = newPage;
	LRU.push_front(pageCount);

	
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
}

int MyDB_BufferManager :: get_free () {

	pageCount++;

	if (!isFull) {
		for (int i = 0; i < numPages; i++) {
			if (freePages[i]) {
				freePages[i] = false;
				return i;
			}
		}
		isFull = true;
	}



	// Eviction logic needs to remove the evicted page's pageId from allPages
	

}

void* MyDB_BufferManager :: get_bytes(MyDB_Page page) {
	int pageId = page.getId();
	if (allPages.find(pageId) == allPages.end()) {
		// The case that pageId is associated with a page that is currently in the buffer
		
		// update LRU object 
		for (auto iter = LRU.begin(); *iter != pageId; iter++) {
			if (iter == LRU.end()) {
				// ERROR CASE pageId is in allPages but not in LRU
				cout << "Tried to get a pageID in allPages but not LRU";
			}
		}
		LRU.erase(iter);
		

	}

	bool isAnon = page.getAnon();

	if (isAnon) {

	}
}

MyDB_BufferManager :: MyDB_BufferManager (size_t pageSize, size_t numPages, string tempFile) {

	this->pageSize = pageSize;
	this->numPages = numPages;
	this->tempFile = tempFile;

	Buffer = malloc(pageSize * numPages);
	freePages = (bool*) malloc(numPages * sizeof(bool));
	for (int i = 0; i < numPages; i ++ ) {
		freePages[i] = true;
	}
	isFull = false;

	pageCount = 0;
}

MyDB_BufferManager :: ~MyDB_BufferManager () {
}
	
#endif


