#include <mutex>
#include <string>
#include <iomanip>
#include <cstring>

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
    // Ensure that only one thread can execute at a time
    file._writerMutex.lock();

    (*file._path) << val;
    (*file._path).flush();

    file._writerMutex.unlock();

    return file;
}
