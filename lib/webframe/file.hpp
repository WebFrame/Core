/** 
 *  @file   file.hpp 
 *  @brief  Multi-thread-safe file structure
 *  @author Alex Tsvetanov
 *  @date   2022-03-07
 ***********************************************/

#pragma once

#include <mutex>
#include <string>
#include <iomanip>
#include <cstring>

/** 
 *  @brief   Multi-thread-safe file class
 *  @details This type handle multithreading write requests to a given output stream (inc. files)
 ***********************************************/
class SynchronizedFile {
public:
    SynchronizedFile (std::basic_ostream<char>& path) : _path(&path) {
    }

    SynchronizedFile () : _path(nullptr) {
    }

    SynchronizedFile& operator = (SynchronizedFile&& sf) {
        _path = sf._path;
        return *this;
    }

    template<typename T>
    friend SynchronizedFile& operator << (SynchronizedFile&, T);
private:
    std::ostream* _path;
    std::mutex _writerMutex;
};

template<typename T>
SynchronizedFile& operator << (SynchronizedFile& file,  T val) {
    file._writerMutex.lock();

    (*file._path) << val;
    (*file._path).flush();

    file._writerMutex.unlock();

    return file;
}
