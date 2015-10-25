#pragma once

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

using ReadWriteMutex = boost::shared_mutex;
using WriterLockGuard = boost::unique_lock<ReadWriteMutex>;
using ReaderLockGuard = boost::shared_lock<ReadWriteMutex>;
