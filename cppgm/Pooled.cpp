#include "std.pch"

#include "Pooled.h"

map<void*, PooledBase::AllocationInfo, greater<void*>> PooledBase::allocations;
