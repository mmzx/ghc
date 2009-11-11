/* -----------------------------------------------------------------------------
 *
 * (c) The GHC Team, 1995-2009
 *
 * The RTS stores some "global" values on behalf of libraries, so that
 * some libraries can ensure that certain top-level things are shared
 * even when multiple versions of the library are loaded.  e.g. see
 * Data.Typeable and GHC.Conc.
 *
 * If/when we switch to a dynamically-linked GHCi, this can all go
 * away, because there would be just one copy of each library.
 *
 * ---------------------------------------------------------------------------*/

#include "PosixSource.h"
#include "Rts.h"

#include "Globals.h"
#include "Stable.h"

typedef enum {
    TypeableStore,
    GHCConcSignalHandlerStore,
    GHCConcPendingEventsStore,
    GHCConcPendingDelaysStore,
    GHCConcIOManagerThreadStore,
    GHCConcProddingStore,
    MaxStoreKey
} StoreKey;

#ifdef THREADED_RTS
Mutex globalStoreLock;
#endif

StgStablePtr store[MaxStoreKey];

void
initGlobalStore(void)
{
    nat i;
    for (i=0; i < MaxStoreKey; i++) {
        store[i] = 0;
    }
#ifdef THREADED_RTS
    initMutex(&globalStoreLock);
#endif
}

void
exitGlobalStore(void)
{
    nat i;
#ifdef THREADED_RTS
    closeMutex(&globalStoreLock);
#endif
    for (i=0; i < MaxStoreKey; i++) {
        if (store[i] != 0) {
            freeStablePtr(store[i]);
            store[i] = 0;
        }
    }
}

static StgStablePtr getOrSetKey(StoreKey key, StgStablePtr ptr)
{
    StgStablePtr ret = store[key];
    if(ret==0) {
#ifdef THREADED_RTS
        ACQUIRE_LOCK(&globalStoreLock);
        ret = store[key];
        if(ret==0) {
#endif
            store[key] = ret = ptr;
#ifdef THREADED_RTS
        }
        RELEASE_LOCK(&globalStoreLock);
#endif
    }
    return ret;
}    


StgStablePtr
getOrSetTypeableStore(StgStablePtr ptr)
{
    return getOrSetKey(TypeableStore,ptr);
}

StgStablePtr
getOrSetGHCConcSignalHandlerStore(StgStablePtr ptr)
{
    return getOrSetKey(GHCConcSignalHandlerStore,ptr);
}

StgStablePtr
getOrSetGHCConcPendingEventsStore(StgStablePtr ptr)
{
    return getOrSetKey(GHCConcPendingEventsStore,ptr);
}

StgStablePtr
getOrSetGHCConcPendingDelaysStore(StgStablePtr ptr)
{
    return getOrSetKey(GHCConcPendingDelaysStore,ptr);
}

StgStablePtr
getOrSetGHCConcIOManagerThreadStore(StgStablePtr ptr)
{
    return getOrSetKey(GHCConcIOManagerThreadStore,ptr);
}

StgStablePtr
getOrSetGHCConcProddingStore(StgStablePtr ptr)
{
    return getOrSetKey(GHCConcProddingStore,ptr);
}
