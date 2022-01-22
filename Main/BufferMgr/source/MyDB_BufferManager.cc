
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include "MyDB_Page.h"
#include <string>
#include <iostream>
#include <list>

using namespace std;


MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr table, long index) {
	return getHandleLookup(table, index, false);
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
	return make_shared <MyDB_PageHandleBase> (getNewPage(false, true));
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr table, long index) {
	return getHandleLookup(table, index, true);
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
	return make_shared <MyDB_PageHandleBase> (getNewPage(true, true));
}

MyDB_PageHandleBase MyDB_BufferManager :: getNewPage(bool isPinned, bool isAnon) {
	// Free page and creating new page object
	int freeIndex = get_free();
	MyDB_Page newPage(freeIndex, isPinned, isAnon, pageCount);

	// Add new page to data structures to keep track
	allPages[pageCount] = newPage;
	LRU.push_front(pageCount);

	// Update page's refcount and turn it into a handlebase
	newPage.addRef();
	MyDB_PageHandleBase newHandleBase(pageCount);

	pageCount += 1;
	return newHandleBase;
}

MyDB_PageHandle MyDB_BufferManager :: getHandleLookup(MyDB_TablePtr whichTable, long index, bool isPinned) {
	// Check the lookup table for the offset + table. Lookup table will always contain a page if its been created before
	pair<MyDB_TablePtr, long> key = pair <MyDB_TablePtr, long> (whichTable, index);
	if (lookup.count(key) == 0) {
		// not in look up table
		MyDB_PageHandleBase handleBase = getNewPage(isPinned, false);
		int pageId = handleBase.getPageId();
		lookup[key] = pageId;
		return make_shared <MyDB_PageHandleBase> (handleBase);
	} else {
		int pageId = lookup[key];
		MyDB_Page page = allPages[pageId];
		page.addRef(); // Note, I could probably put this in constructor for handlebase if I added helper methods

		MyDB_PageHandleBase newHandleBase(pageId);
		return make_shared <MyDB_PageHandleBase> (newHandleBase);
	}
}


void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
}

int MyDB_BufferManager :: get_free () {

	if (!isFull) {
		for (int i = 0; i < numPages; i++) {
			if (freePages[i]) {
				freePages[i] = false;
				return i;
			}
		}
		isFull = true;
	}

	// TODO: Eviction logic only needs to update the LRU, not lookup/allpages
	

}

void* MyDB_BufferManager :: get_bytes(int pageId) {
	
	MyDB_Page page = allPages[pageId];
	// update LRU object 
	auto iter = LRU.begin();
	for (; *iter != pageId && iter!= LRU.end(); iter++); 
	if (iter != LRU.end()) {
		// page is in the LRU, update its position and return buffer index
		LRU.erase(iter);
		LRU.push_front(pageId);
		return Buffer + pageSize * page.getIndex(); // Check this
	} else {
		int newIndex = get_free();
		page.setIndex(newIndex);
		LRU.push_front(pageId);
		
		// TODO: Need to load content from disk into the new index of buffer.

		return Buffer + pageSize * page.getIndex(); // Check this
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


