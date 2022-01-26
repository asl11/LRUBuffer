
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include "MyDB_PageHandle.h"
#include "MyDB_Page.h"
#include <string>
#include <iostream>
#include <list>
#include <fcntl.h>  
#include <unistd.h>

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
	int freeIndex = getFree();
	int freeTempIndex = -1;

	if (isAnon == true) {
		freeTempIndex = getFreeTempIndex();
	} 

	MyDB_Page newPage(freeIndex, isPinned, isAnon, pageCount, freeTempIndex);

	// Update page's refcount and turn it into a handlebase
	newPage.addRef();
	MyDB_PageHandleBase newHandleBase(pageCount, this);

	// Add new page to data structures to keep track
	allPages[pageCount] = newPage;
	LRU.push_front(pageCount);

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
		allPages[pageId].addRef();
		allPages[pageId].setTableLoc(key);

		string fileName = whichTable->getStorageLoc();
		readFromFile(Buffer + allPages[pageId].getIndex() * pageSize, index, fileName);

		return make_shared <MyDB_PageHandleBase> (handleBase);
	} else {
		int pageId = lookup[key];
		allPages[pageId].addRef();

		MyDB_PageHandleBase newHandleBase(pageId, this);
		return make_shared <MyDB_PageHandleBase> (newHandleBase);
	}
}


void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
	int pageId = unpinMe->getPageId();
	allPages[pageId].setPinned(false);
	LRU.push_back(pageId);

}


int MyDB_BufferManager :: getFree () {

	if (!isFull) {
		for (int i = 0; i < numPages; i++) {
			if (freePages[i]) {
				freePages[i] = false;
				return i;
			}
		}
		isFull = true;
	}

	// In case where no free pages (nearly all cases after initial numPages pages)	

	if (LRU.size() == 0) {
		// ERROR case either was initialized with 0 free pages or all pinned
		cout << "Don't know what to do - LRU is empty and called get_free \n";
	}

	int evictedId = LRU.back();
	LRU.pop_back();
	
	int bufIndex = allPages[evictedId].getIndex();
	void* bufferLoc = Buffer + bufIndex * pageSize;

	// Anon case needs to write to the temp file
	if (allPages[evictedId].getAnon()) {
		// Write to the temp file at the page's given location

		int tempFileIndex = allPages[evictedId].getTempFileIndex();
		int fileWriteLoc = tempFileIndex * pageSize;
		if (lseek(tempFd, fileWriteLoc, SEEK_SET) == -1) {
			// Error case failed to lseek
			cout << "Failed to lseek! \n";
		}
		if (write(tempFd, bufferLoc, pageSize) == -1) {
			// Erro case failed to write to file
			cout << "Failed to write to temp file! \n";
		}
		
	} else if (allPages[evictedId].getDirty() == true){
		pair <MyDB_TablePtr, long> location = allPages[evictedId].getTableLoc();
		MyDB_TablePtr tablePtr = location.first;
		long offset = location.second;
		string fileName = tablePtr->getStorageLoc();
		int fd = open(fileName.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
		if (lseek(fd, offset * pageSize, SEEK_SET) == -1) {
			cout << "Failed to lseek \n";
		}
		if (write(fd, bufferLoc, pageSize) == -1) {
			cout << "Failed to write \n";
		}
		close(fd);
	}

	// Shouldn't need the first if check, since nonAnon pages should get kicked from LRU as soon as refCount is 0 
	if (allPages[evictedId].getAnon() == false && allPages[evictedId].getRef() == 0) {
		deletePage(evictedId);
	}
	
	// Clear the buffer at this index and return it for new page to use
	memset(bufferLoc, 0, pageSize);
	return bufIndex;

}

int MyDB_BufferManager :: getFreeTempIndex() {
	if (freeTempfileIndex.size() == 0) {
		int result = tempFileIndex;
		tempFileIndex += 1;
		return result;
	} else {
		int result = freeTempfileIndex.back();
		freeTempfileIndex.pop_back();
		return result;
	}
}

void* MyDB_BufferManager :: getBytes(int pageId) {
	
	void* bufLoc = Buffer + pageSize * allPages[pageId].getIndex();
	if (allPages[pageId].getPinned() == true) {
		return bufLoc;
	}

	// update LRU object 
	auto iter = LRU.begin();
	for (; *iter != pageId && iter!= LRU.end(); iter++); 
	if (iter != LRU.end()) {
		// page is in the LRU, update its position and return buffer index
		LRU.erase(iter);
		LRU.push_front(pageId);
		return bufLoc;
	} else {
		int newIndex = getFree();
		allPages[pageId].setIndex(newIndex);
		LRU.push_front(pageId);
		
		if (allPages[pageId].getAnon() == true) {
			int tempFileIndex = allPages[pageId].getTempFileIndex();
			readFromFile(bufLoc, tempFileIndex);
		} else {
			string fileName = allPages[pageId].getTableLoc().first->getStorageLoc();
			int offset = (int) allPages[pageId].getTableLoc().second;
			readFromFile(bufLoc, offset, fileName);
		}

		return bufLoc;
	}
}

void MyDB_BufferManager :: readFromFile(void* bufferLoc, int offset, string fileName = "") {
	// Read it from tableLoc into buffer
		int fd;
		if (fileName == "") {
			fd = tempFd;
		} else {
			int fd = open(fileName.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
		}
		if (lseek(fd, offset * pageSize, SEEK_SET) == -1) {
			cout << "Failed to lseek \n";
		}
		if (read(fd, bufferLoc, pageSize) == -1) {
			cout << "Failed to read \n";
		}

		if (fileName != "") {
			close(fd);
		}
}

void MyDB_BufferManager :: markDirty(int pageId) {
	allPages[pageId].setDirty();
}

void MyDB_BufferManager :: freeHandleBase(int pageId) {
	allPages[pageId].decRef();

	int refCount = allPages[pageId].getRef();

	// Kill conditions are : 0 refcount and anon, or 0 refcount + evicted and nonAnon
	if (refCount == 0 && allPages[pageId].getAnon() == true) {
		deletePage(pageId);
	}
}

void MyDB_BufferManager :: deletePage(int pageId) {
	
	if (allPages[pageId].getAnon() == true) {
		// Deleting an anonymous page

		// Delete from LRU if inside
		for (auto iter = LRU.begin(); iter!= LRU.end(); iter++) {
			if (*iter == pageId) {
				LRU.erase(iter);
				break;
			}
		}

		// Allow the free temp file index to be reused
		freeTempfileIndex.push_front(allPages[pageId].getTempFileIndex());	
	} 

	// nonAnon pages only need their pageId erased from the allPages map
	allPages.erase(pageId);
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
	tempFileIndex = 0;

	tempFd = open(tempFile.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (tempFd == -1) {
		// Error case for failed to open file
		cout << "Failed to open file, probably going to crash";
	}
}

MyDB_BufferManager :: ~MyDB_BufferManager () {
	close(tempFd);
}
	
#endif


