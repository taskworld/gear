#include <mutex>

using namespace std;

class Semaphore {
 public:
  Semaphore(int count = 0) : _count(count) {}

  void notify() {
    unique_lock<mutex> lock(_mutex);
    _count++;
    _conditionVar.notify_one();
  }

  void wait() {
    unique_lock<mutex> lock(_mutex);
    _conditionVar.wait(lock, [this]() { return _count > 0; });
  }

  void waitFor(const chrono::seconds& duration) {
    unique_lock<mutex> lock(_mutex);

    auto done = _conditionVar.wait_for(lock, duration, [this]() { return _count > 0; });
    if (done) {
      _count--;
    }
  }

 private:
  mutex _mutex;
  condition_variable _conditionVar;
  int _count;
};
