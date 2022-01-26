#ifndef PAGE_C
#define PAGE_C

#include "MyDB_Page.h"
#include <iostream>

MyDB_Page :: MyDB_Page() {
    cout << " ERROR : Tried to call default constructor\n";
}

MyDB_Page :: MyDB_Page(int index, bool isPinned, bool isAnon, int pageId, int tempFileIndex = -1) : index(index), 
        isPinned(isPinned), isAnon(isAnon), pageId(pageId), tempFileIndex(tempFileIndex) {
            refCount = 0;
            dirty = false;
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

int MyDB_Page :: getTempFileIndex() {
    return tempFileIndex;
}

string MyDB_Page :: getTableFileName() {
    return tableFileName;
}

long MyDB_Page :: getTableIndex() {
    return tableIndex;
}

void MyDB_Page :: setTableLoc(string fileName, long i) {
    tableFileName = fileName;
    tableIndex = i;
}

void MyDB_Page :: setDirty() {
    dirty = true;
}

bool MyDB_Page :: getDirty() {
    return dirty;
}


#endif