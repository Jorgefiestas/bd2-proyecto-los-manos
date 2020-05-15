#include <list>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

template <class T, int fd> struct Bucket {
  std::map<int, T> values;
  std::vector<T> overflow;
  int depth;
  int size;

  Bucket(int depth, int size);
  int insert(int key, T obj, bool force);
  std::optional<T> search(int key);
};

template <class T, int fd = 2> class DinHash {
  using Bucket = Bucket<T, fd>;
  int max_depth;
  std::list<std::shared_ptr<Bucket>> buckets;
  unsigned int make_mask(int n) { return (1 << n) - 1; }

public:
  DinHash(int max_depth) : max_depth(max_depth) {
    auto b0 = std::make_shared<Bucket>();
    auto b1 = std::make_shared<Bucket>();
  }

  void insert(int key, T obj);
  std::optional<T> search(int key);
  bool split_bucket(int hash, const T &obj);
};
