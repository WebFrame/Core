#ifndef MOKA_MUST_H
#define MOKA_MUST_H

#include "util.h"

#include <cstring>
#include <exception>
#include <functional>
#include <sstream>
#include <string>

#define must_contain(a, b, msg)       Moka::must::contain(__FILE__, __LINE__, a, b, msg)
#define must_be_equal(a, b, msg)      Moka::must::be_equal(__FILE__, __LINE__, a, b, msg)
#define must_be_less(a, b, msg)       Moka::must::be_less(__FILE__, __LINE__, a, b, msg)
#define must_be_greater(a, b, msg)    Moka::must::be_greater(__FILE__, __LINE__, a, b, msg)
#define must_be_not_equal(a, b, msg)  Moka::must::be_not_equal(__FILE__, __LINE__, a, b, msg)
#define must_throw(T, f, msg)         Moka::must::throoow<T>(__FILE__, __LINE__, #T, f, msg)
#define must_fail(m, msg)             Moka::must::fail(__FILE__, __LINE__, m, msg)

namespace Moka
{
  class Failure: public std::exception {
    const char* mFile;
    const int   mLine;
    std::string mWhat;
  public:
    Failure(const std::string& m): mFile(nullptr), mLine(0), mWhat(m) {}
    Failure(const char* f, int l, const std::string& m): mFile(f), mLine(l), mWhat(m) {}
    ~Failure() throw() {}

    const char* file() const {
      return mFile;
    }

    int line() const {
      return mLine;
    }

    const char* what() const throw() {
      return mWhat.c_str();
    }
  };

  namespace must {
    void contain(const char* f, int l, const std::string& a, const std::string& b, std::string msg = "") {
      if (msg.size() > 0) msg = msg + " | ";
      if(b.find(a) == std::string::npos) {
        std::stringstream message;
        message << msg << "Expected " << cli::g(b) << " to contain " << cli::r(a);
        throw new Failure(f, l, message.str());
      }
    }
    
    template <class A, class B>
    void be_equal(const char* f, int l, const A& a, const B& b, std::string msg = "") {
      if (msg.size() > 0) msg = msg + " | ";
      if(a != b) {
        std::stringstream message;
        message << msg << "Expected " << cli::g(b) << " but got " << cli::r(a);
        throw new Failure(f, l, message.str());
      }
    }   

    template <class A, class B>
    void be_less(const char* f, int l, const A& a, const B& b, std::string msg = "") {
      if (msg.size() > 0) msg = msg + " | ";
      if(!(a < b)) {
        std::stringstream message;
        message << msg << "Expected " << cli::g(b) << " but got " << cli::r(a);
        throw new Failure(f, l, message.str());
      }
    }

    template <class A, class B>
    void be_greater(const char* f, int l, const A& a, const B& b, std::string msg = "") {
      if (msg.size() > 0) msg = msg + " | ";
      if(!(a > b)) {
        std::stringstream message;
        message << msg << "Expected " << cli::g(b) << " but got " << cli::r(a);
        throw new Failure(f, l, message.str());
      }
    }

    void be_equal(const char* f, int l, const char* a, const char* b, std::string msg = "") {
      if (msg.size() > 0) msg = msg + " | ";
      if(strcmp(a, b) != 0) {
        std::stringstream message;
        message << msg << "Expected " << cli::g(b) << " but got " << cli::r(a);
        throw new Failure(f, l, message.str());
      }
    }

    void fail(const char* f, int l,  const char* m, std::string msg = "") {
      if (msg.size() > 0) msg = msg + " | ";
      std::stringstream message;
      message << cli::r(m);
      throw new Failure(f, l, message.str());
    }

    template <class A, class B>
    void be_not_equal(const char* f, int l, const A& a, const B& b, std::string msg = "") {
      if (msg.size() > 0) msg = msg + " | ";
      if(a == b) {
        std::stringstream message;
        message << msg << "Expected anything but " << cli::r(b) << " but got " << cli::r(a);
        throw new Failure(f, l, message.str());
      }
    }

    template <class E>
    void throoow(const char* f, int l, const char* type, std::function<void()> fn, std::string msg = "") {
      if (msg.size() > 0) msg = msg + " | ";
      try {
        fn();
      }
      catch(E& e) {
        // Good catch!
        return;
      }
      catch(std::exception& e) {
        std::stringstream message;
        message << msg << "Expected to catch a " << cli::g(type) << " but got " << cli::r(e.what());
        throw new Failure(f, l, message.str());
      }

      std::stringstream message;
      message << msg << "Expected to catch a " << cli::g(type) << " but nothing was thrown!";
      throw new Failure(f, l, message.str());
    }
  }
}

#endif
