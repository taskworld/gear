#include <mutex>

using namespace std;

class semaphore {
 public:
  semaphore(int count = 0) : _count(count) {}

  void notify() {
    unique_lock<mutex> lock(_mutex);
    _count++;
    condition_var.notify_one();
  }

  void wait() {
    unique_lock<mutex> lock(_mutex);
    condition_var.wait(lock, [this]() { return _count > 0; });
  }

  void wait_for(const chrono::seconds& duration) {
    unique_lock<mutex> lock(_mutex);

    auto done =
        condition_var.wait_for(lock, duration, [this]() { return _count > 0; });
    if (done) {
      _count--;
    }
  }

 private:
  mutex _mutex;
  condition_variable condition_var;
  int _count;
};
