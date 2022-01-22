#ifndef PAGE_C
#define PAGE_C

#include "MyDB_PageHandle.h"
#include "MyDB_Page.h"
#include <iostream>

MyDB_Page :: MyDB_Page() {
    cout << " ERROR : Tried to call default constructor";
}

MyDB_Page :: MyDB_Page(int index, bool isPinned, bool isAnon, int pageId) : index(index), 
        isPinned(isPinned), isAnon(isAnon), pageId(pageId) {
            refCount = 0;
        };

bool MyDB_Page :: getPinned() {return isPinned;}

bool MyDB_Page :: getAnon() {return isAnon;}

void MyDB_Page :: setPinned(bool newStatus) {isPinned = newStatus;}

int MyDB_Page :: getId() {return pageId;}

void MyDB_Page :: addRef() {
    refCount += 1;
}

void MyDB_Page :: decRef() {
    refCount -= 1;
}

int MyDB_Page :: getRef() {
    return refCount;
}

void MyDB_Page :: setIndex(int newIndex) {
    index = newIndex;
}

int MyDB_Page :: getIndex() {
    return index;
}
#endif