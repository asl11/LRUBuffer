
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C

#include <memory>
#include "MyDB_PageHandle.h"
#include "MyDB_Page.h"

void *MyDB_PageHandleBase :: getBytes () {
	return nullptr;
}

void MyDB_PageHandleBase :: wroteBytes () {
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
}

MyDB_PageHandleBase :: MyDB_PageHandleBase(int pageId) {
	this->pageId = pageId;
}

int MyDB_PageHandleBase :: getPageId() {
	return pageId;
}




#endif

