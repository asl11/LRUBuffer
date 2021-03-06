
#ifndef BUFFER_MGR_H
#define BUFFER_MGR_H

// slightly odd ordering here is meant to avoid circular includes
class MyDB_PageHandleBase;
#include <memory>
typedef std::shared_ptr <MyDB_PageHandleBase> MyDB_PageHandle;

#include "MyDB_Table.h"
#include "MyDB_Page.h"

#include <cstring>
#include <unordered_map>
#include <list>
#include <string>

using namespace std;

class MyDB_BufferManager {


public:

	// gets the i^th page in the table whichTable... note that if the page
	// is currently being used (that is, the page is current buffered) a handle 
	// to that already-buffered page should be returned
	MyDB_PageHandle getPage (MyDB_TablePtr whichTable, long i);

	// gets a temporary page that will no longer exist (1) after the buffer manager
	// has been destroyed, or (2) there are no more references to it anywhere in the
	// program.  Typically such a temporary page will be used as buffer memory.
	// since it is just a temp page, it is not associated with any particular 
	// table
	MyDB_PageHandle getPage ();

	// gets the i^th page in the table whichTable... the only difference 
	// between this method and getPage (whicTable, i) is that the page will be 
	// pinned in RAM; it cannot be written out to the file
	MyDB_PageHandle getPinnedPage (MyDB_TablePtr whichTable, long i);

	// gets a temporary page, like getPage (), except that this one is pinned
	MyDB_PageHandle getPinnedPage ();

	// un-pins the specified page
	void unpin (MyDB_PageHandle unpinMe);

	// creates an LRU buffer manager... params are as follows:
	// 1) the size of each page is pageSize 
	// 2) the number of pages managed by the buffer manager is numPages;
	// 3) temporary pages are written to the file tempFile
	MyDB_BufferManager (size_t pageSize, size_t numPages, string tempFile);

	// when the buffer manager is destroyed, all of the dirty pages need to be
	// written back to disk, any necessary data needs to be written to the catalog,
	// and any temporary files need to be deleted
	~MyDB_BufferManager ();

	// Called by page handle when bytes are dirty
	void markDirty(int pageId);

	// Called by page handle destructor
	void freeHandleBase(int pageId);

	// Internal helper method to get bytes. Needs to reload from disk to memory if page has been evicted
	void * getBytes(int pageId);

private:

	// size of all pages in the buffer
	size_t pageSize;

	// number of pages that can be stored at once by the buffer
	size_t numPages;

	// path of temp file where anonymous pages will be written when evicted
	string tempFile;

	// Buffer is the chunk of memory allocated for buffer storage. *Bytes should point an index * pageSize + Buffer 
	void *Buffer;

	// Lookup table contains a map of EVERY filename to offset to pageId, since you need to lookup pageIds that may not be in the buffer
	unordered_map <string, unordered_map <long, int> > lookup;

	// Map of pageID to every page object
	unordered_map <int, MyDB_Page> allPages;

	// LRU is the only data structure of these three which has a max size of numPages
	list <int> LRU;

	// List of free temp file indeces. Used so that we don't have a continuously increasing temp file and instead mark offsets 
	// in the temp file as free when the associated page is deleted. 
	list <int> freeTempfileIndex;
	
	// pageSize-sized array indicating whether the respective page is currently free
	// or not in the buffer (true = free, false = occupied)
	bool *freePages;

	// whether the buffer is currently full (usually true!)
	bool isFull;

	// monotonically increasing int, used to generate new page IDs whenever
	// a page is created
	int pageCount;

	// highest index used up to this point for anonymous pages in the temp file
	int tempFileIndex;

	// file descriptor for the open temp file
	int tempFd;

	// Gets the next free page index of the buffer, or evicts one if they're all full
	int getFree();

	// Creates a new page object and returns the handle base associated. 
	int getNewPage(bool isPinned, bool isAnon);

	// Checks if a nonAnon page is in the lookup table, if not, creates new handle base
	int getHandleLookup(MyDB_TablePtr whichTable, long i, bool isPinned);

	// Gets the next free index for temp file storage, adds one if none are available
	int getFreeTempIndex();

	// Helper method to read from a filename into the bufferlocation
	void readFromFile(void* bufferLoc, int offset, string fileName);

	// Helper method to write from bufferloc into the file
	void writeToFile(void* bufferLoc, int offset, string fileName);

	// Helper method to "kill" pages
	void deletePage(int pageId);

};

#endif


