#ifndef PAGE_H
#define PAGE_H

#include "MyDB_Table.h"
#include <string>

using namespace std;

// simple record class for data about a single loaded page in
// the buffer manager
class MyDB_Page {
    
public:

    // basic constructor
    MyDB_Page(int index, bool isPinned, bool isAnon, int pageId, int tempFileIndex);

    // whether this page is currently pinned in the buffer mgr
    bool getPinned();

    // whether this is an anonymous/temporary page
    bool getAnon();

    // change the pinned status of this page
    void setPinned(bool newStatus);

    // get the internal numerical ID of this page
    int getId();

    // default constructor for use in container classes
    MyDB_Page();

    // increment this page's reference count
    // -- should call whenever a new PageHandleBase is created for this page
    void addRef();

    // decrement this page's reference count
    // -- should call whenever a PageHandleBase for this page goes
    // out of scope and is destructed
    void decRef();

    // get this page's reference count value
    int getRef();

    // change the index of this page in the buffer pool
    void setIndex(int newIndex);

    // get this page's buffer index
    int getIndex();

    // if anonymous, get this page's offset in the temp file
    int getTempFileIndex();

    // set this page's "table location" string (eviction output file)
    void setTableLoc(string tableFileName, long tableIndex);

    // get this page's offset in its table file
    long getTableIndex();

    // get the filename of this page's table
    string getTableFileName();

    // mark this page as having had data written into it since last eviction
    void setDirty();

    // whether this page has had data written- since last eviction
    bool getDirty();


    
private:

    // index of this page in the buffer pool -- that is, the page-sized offset
    // into the buffer's memory array
    int index;

    // whether this page is pinned and immune to eviction
    bool isPinned;

    // whether this is an anonymous/temporary page
    bool isAnon;

    // internal ID of this page among all loaded pages in the buffer mgr
    int pageId;

    // number of active user references to this page
    int refCount;

    // if anonymous, page-sized offset of this page's data in the temp file
    int tempFileIndex;

    // whether this page has been dirtied by writes since last eviction
    bool dirty;

    // page-sized offset of this page's data in its table file
    long tableIndex;

    // filename of this page's table file
    string tableFileName;

};

#endif
