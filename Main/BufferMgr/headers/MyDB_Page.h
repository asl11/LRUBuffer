#ifndef PAGE_H
#define PAGE_H

#include "MyDB_Table.h"
#include <string>

using namespace std;

class MyDB_Page {
    
public:

    MyDB_Page(int index, bool isPinned, bool isAnon, int pageId, int tempFileIndex);

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

    int getTempFileIndex();

    void setTableLoc(string tableFileName, long tableIndex);

    long getTableIndex();

    string getTableFileName();

    void setDirty();

    bool getDirty();


    
private:

    int index;
    bool isPinned;
    bool isAnon;
    int pageId;
    int refCount;
    int tempFileIndex;
    bool dirty;
    long tableIndex;
    string tableFileName;

};

#endif