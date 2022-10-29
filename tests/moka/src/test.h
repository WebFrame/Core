#ifndef MOKA_TEST_H
#define MOKA_TEST_H

#include "must.h"
#include "util.h"

#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace Moka
{
  class Report {
    struct Item {
      int         id;
      std::string name;
      Failure     error;
      bool        failed;
    public:
      Item(int i, std::string s, const Failure& f): id(i), name(s), error(f), failed(true) {}
      Item(int i, std::string s): id(i), name(s), failed(false) {}
    };
  protected:
    std::vector<Item> mItems;
    std::vector<std::string> mNames;
  protected:
    std::string prefix(const Item& i) const {
      if(!i.failed) {
        return cli::g("+ ", true); //✔
      }
      if(!i.error.is_fail()) {
        return cli::y("? ", true); //?
      }
      else if(i.error.file() == nullptr) {
        return cli::y("^ ", true); //▲
      }
      else {
        return cli::r("x ", true); //✘
      }
    }
  public:
    void enter(std::string name) {
      indent();
      std::cout << cli::bold(name) << std::endl;
      mNames.push_back(name);
    }

    int id() const {
      return mItems.size() + 1;
    }

    void indent() const {
      for(int i = mNames.size(); i > 0; --i) {
        std::cout << "  ";
      }
    }

    const Item& item(int index) const {
      return mItems[index];
    }

    const std::vector<Item>& items() const {
      return mItems;
    }

    void leave() {
      mNames.pop_back();
    }

    int level() const {
      return mNames.size();
    }

    int print() const {
      std::cout << std::endl;
      unsigned int fails = 0;
      for(const Item& i: mItems) {
          std::cout << "there is an item" << std::endl;
        if(!i.failed) continue;
        fails += i.error.is_fail();
        std::cout << prefix(i) << i.id << ") " << i.name << ":\n";
        std::cout << "  " << i.error.what() << std::endl;

        if(i.error.file()) {
          std::cout << "  in " << cli::bold(i.error.file());
          std::cout << ':' << i.error.line() << std::endl;
        }

        std::cout << std::endl;
      }
      return fails;
    }

    void push(std::string testname) {
      std::stringstream stream;
      for(auto& name: mNames) stream << name << " ";
      stream << testname;

      mItems.push_back(Item(id(), stream.str()));
      summarize(mItems.back(), testname);
    }

    void push(std::string testname, const Failure& f) {
      std::stringstream stream;
      for(auto& name: mNames) stream << name << " ";
      stream << testname;

      mItems.push_back(Item(id(), stream.str(), f));
      summarize(mItems.back(), testname);
    }

    void summarize(const Item& i, const std::string& name) const {
      indent();
      std::cout << prefix(i) << i.id;
      std::cout << ") " << name << std::endl;
    }
  };

  class Base {
  public:
    virtual void test(Report& report) const = 0;

    void indent(int level) {
      while(level --> 0) std::cout << "  ";
    }

    bool run() {
      Report report;
      this->test(report);
      report.print();

      return report.items().empty();
    }

    Report test() const {
      Report report;
      this->test(report);
      return report;
    }
  };

  class Test: public Base {
  protected:
    std::string           mName;
    std::function<void()> mFunction;
  public:
    Test(std::string name, std::function<void()> fn): mName(name), mFunction(fn) {
      // All done.
    }

    void test(Report& report) const {
      try {
        mFunction();
        report.push(mName);
      }
      catch(const Failure& error) {
        report.push(mName, error);
      }
      catch(const std::exception& error) {
        std::string prefix("Unexpected exception: ");
        Failure e = Failure(prefix + cli::y(error.what()));
        report.push(mName, e);
      }
    }
  };

  class Context: public Base {
  protected:
    std::string           mPrefix;
    std::string           mName;
    std::vector<const Base*>    mMembers;
    std::function<void()> mSetup;
    std::function<void()> mTeardown;
    bool                  mHasSetup;
    bool                  mHasTeardown;

    Context(std::string name, std::string prefix, std::function<void(Context&)> fn) {
      mName = std::string(name);
      mMembers = std::vector<const Base*>();
      mPrefix = std::string(prefix + " ");
      mHasTeardown = false;
      mHasSetup = false;
      fn(*this);
    }
  public:
    Context(std::string name) {
      mName = std::string(name);
      mMembers = std::vector<const Base*>();
      mPrefix = std::string("");
      mHasTeardown = false;
      mHasSetup = false;
    }

    Context(std::string name, std::function<void(Context&)> fn) {
      mName = std::string(name);
      mPrefix = std::string("");
      mMembers = std::vector<const Base*>();
      mHasTeardown = false;
      mHasSetup = false;
      fn(*this);
    }

    void describe(std::string name, std::function<void(Context&)> fn) {
      const Context* child = new Context("", name, fn);
      mMembers.push_back(child);
    }

    void setup(std::function<void()> fn) {
      mHasSetup = true;
      mSetup = fn;
    }

    void should(std::string name, std::function<void()> fn) {
      const Test* test = new Test("should " + name, fn);
      mMembers.push_back(test);
    }

    void teardown(std::function<void()> fn) {
      mHasTeardown = true;
      mTeardown = fn;
    }

    void test(Report& report) const {
      report.enter(mPrefix + mName);
      std::cout << mPrefix + mName << std::endl;
      if(mHasSetup) mSetup();
      std::cout << "Setup Done" << mHasSetup << " | " << (this) << std::endl;
      for(const Base* m: mMembers) m->test(report);
      if(mHasTeardown) mTeardown();
      report.leave();
    }
  };
}

#endif
