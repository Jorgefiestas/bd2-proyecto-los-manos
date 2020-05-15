#include <hash/dynamic_hash.hpp>
template <class T, int fd> Bucket<T, fd>::Bucket(int depth, int size) {
  depth = depth;
  size = size;
}

template <class T, int fd> int Bucket<T, fd>::insert(int depth, int size) {
  if (values.size() == size and not force)
    return 0;
  if (values.size() == size and force) {
    overflow.push_back(obj);
    return 0;
  }
  values[key] = obj;
  return 1;
}
template <class T, int fd> std::optional<T> Bucket<T, fd>::search(int key) {

  auto it = values.find(key);
  if (it != values.end())
    return std::make_optional(values[key]);
  return std::nullopt;
}
// Dynamic Hash

template <class T, int fd> void DinHash<T, fd>::insert(int key, T obj) {
  unsigned int hash = obj & make_mask(max_depth);
  int result = buckets[hash]->insert(hash, obj);
  if (result == 1 || result == 0)
    return;
  bool force = split_bucket(hash, obj);
  insert(key, obj, force);
}
template <class T, int fd> std::optional<T> Bucket<T, fd>::search(int key) {
  unsigned int hash = key & make_mask(max_depth);
  return buckets[hash]->search(key);
}
template <class T, int fd>
bool DinHash<T, fd>::split_bucket(int hash, const T &obj) {
  auto buck = buckets[hash];
  if (buck->depth == max_depth) {
    return false;
  }
  buck->depth++;

  // Copy values from bucket and clear
  auto vec_copy = buck->values.copy();
  vec_copy[hash] = obj;
  buck->values.clear();

  // Create new buckets if they don't exist
  if (buckets.size() != pow(2, buck->depth)) {
    for (int x = (1 << buck->depth) - 1; x > (1 << (buck->depth - 1)) - 1;
         x--) {
      buckets[x] = nullptr;
    }
    buckets.resize(pow(2, buck->depth));
  }

  int pairHash = (1 << (buck->depth - 1)) | hash;
  for (const auto &[k, v] : vec_copy) {
    buckets[k]->insert(k, v);
  }
  return true;
}
