
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include "MyDB_PageHandle.h"
#include "MyDB_Page.h"
#include "MyDB_BufferManager.h"

void *MyDB_PageHandleBase :: getBytes () {
	return bufferMan->getBytes(pageId);
}

void MyDB_PageHandleBase :: wroteBytes () {
	bufferMan->markDirty(pageId);
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
	// need to update the tempFileIndex if temp page is getting killed
	bufferMan->freeHandleBase(pageId);
}

MyDB_PageHandleBase :: MyDB_PageHandleBase(int pageId, MyDB_BufferManager *bufferMan) {
	this->pageId = pageId;
	this->bufferMan = bufferMan;
}

int MyDB_PageHandleBase :: getPageId() {
	return pageId;
}




#endif

