#ifndef PAGE_H
#define PAGE_H

#include "MyDB_PageHandle.h"
#include "MyDB_Page.h"

using namespace std;

class MyDB_Page {
    
public:

    MyDB_Page(int index, bool isPinned, bool isAnon, int pageId);

    bool getPinned();

    bool getAnon();

    void setPinned(bool newStatus);

    int getId();

    MyDB_Page();

    void addRef();

    void decRef();

    int getRef();

    void setIndex(int newIndex);

    int getIndex();


    
private:

    int index;
    bool isPinned;
    bool isAnon;
    int pageId;
    int refCount;

};

#endif