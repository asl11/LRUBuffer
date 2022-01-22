#ifndef PAGE_H
#define PAGE_H

#include "MyDB_PageHandle.h"
#include "MyDB_Page.h"

using namespace std;

class MyDB_Page {
    
public:

    MyDB_Page(int index, bool isPinned, bool isAnon, int pageId) : index(index), 
        isPinned(isPinned), isAnon(isAnon), pageId(pageId) {};

    bool getPinned() {return isPinned;}

    bool getAnon() {return isAnon;}

    void setPinned(bool newStatus) {isPinned = newStatus;}

    int getId() {return pageId;}

private:

    int index;
    bool isPinned;
    bool isAnon;
    int pageId;
};

#endif