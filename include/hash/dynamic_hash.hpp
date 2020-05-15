#ifndef DYNAMIC_HASH_HPP
#define DYNAMIC_HASH_HPP

#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

template <class T, int fd> struct BaseBucket {
  std::vector<std::pair<int, T>> values;
  // std::vector<T> overflow;
  int depth;

  BaseBucket(int size);
  int insert(int key, T obj, bool force);
  std::optional<T> search(int key);
};

template <class T, int fd = 2> class DinHash {
  using Bucket = BaseBucket<T, fd>;
  std::string filename;
  int max_depth;
  int current_depth;

  unsigned int make_mask(int n) { return (1 << n) - 1; }
  Bucket read_bucket(int pos);
	void write_bucket(int hash, Bucket b);

public:
  DinHash(int max_depth, const std::string &filename)
      : max_depth(max_depth), filename(filename) {
    if (not fs::exists(fs::path(filename))) {
      current_depth = 1;
      std::ofstream out_stream(filename, std::ios::binary | std::ios::trunc);

      Bucket b0(1);
      Bucket b1(1);

      out_stream.write((char *)&current_depth, sizeof(int));
      out_stream.write((char *)&b0, sizeof(Bucket));
      out_stream.write((char *)&b1, sizeof(Bucket));
      out_stream.close();
      return;
    }
    this->filename = filename;
    std::ifstream in_stream(filename, std::ios::binary | std::ios::in);
    in_stream.read((char *)&current_depth, sizeof(int));
		in_stream.close();
  }

  void insert(int key, T obj);
  std::optional<T> search(int key);
  bool split_bucket(int hash);
};
#endif
template <class T, int fd> BaseBucket<T, fd>::BaseBucket(int depth) {
  depth = depth;
}

template <class T, int fd>
int BaseBucket<T, fd>::insert(int key, T obj, bool force) {
  if (values.size() == fd and not force)
    return -1;
  if (values.size() == fd and force) {
    // overflow.push_back(obj);
    return 0;
  }
  values.push_back(std::make_pair(key, obj));
  return 1;
}
template <class T, int fd> std::optional<T> BaseBucket<T, fd>::search(int key) {

  auto it = values.find(key);
  if (it != values.end())
    return std::make_optional(*it);
  return std::nullopt;
}
// Dynamic Hash

template <class T, int fd> typename DinHash<T, fd>::Bucket DinHash<T, fd>::read_bucket(int hash) {
	std::ifstream in_stream(filename, std::ios::binary | std::ios::in);
	in_stream.seekg(hash * sizeof(Bucket) + sizeof(int), std::ios::beg);
	Bucket b(1);
	in_stream.read((char*)&b, sizeof(Bucket));
	in_stream.close();
	return b;
}

template <class T, int fd> void DinHash<T, fd>::write_bucket(int hash, Bucket b) {
	std::ofstream out_stream(filename, std::ios::binary | std::ios::out);
	out_stream.seekp(hash * sizeof(Bucket) + sizeof(int), std::ios::beg);
	out_stream.write((char*)&b, sizeof(Bucket));
	out_stream.close();
}
template <class T, int fd> void DinHash<T, fd>::insert(int key, T obj) {
  unsigned int hash = key & make_mask(current_depth);
	Bucket b = read_bucket(hash);
  int result = b.insert(key, obj, false);
  if (result == 1 || result == 0)
    write_bucket(hash, b);
  bool force = split_bucket(key);
  insert(key, obj);
}

template <class T, int fd> std::optional<T> DinHash<T, fd>::search(int key) {
  unsigned int hash = key & make_mask(current_depth);
	Bucket b = read_bucket(hash);
  return b->search(key);
}
template <class T, int fd>
bool DinHash<T, fd>::split_bucket(int key) {
	int hash = key & make_mask(current_depth);
  Bucket buck = read_bucket(hash);
  if (buck.depth == max_depth) {
    return false;
  }
  buck.depth++;

  // Copy values from bucket and clear
  auto vec_copy = buck.values;
  buck.values.clear();

  // Create new buckets if they don't exist
  if (current_depth != buck.depth) {
		current_depth = buck.depth;
    for (int x = (1 << current_depth) - 1; x > (1 << (current_depth - 1)) - 1;
         x--) {
			Bucket tmp(1);
			write_bucket(x, tmp);
    }
		std::ofstream out_file(filename, std::ios::binary | std::ios::out);
		out_file.write((char*)&current_depth, sizeof(int));
		out_file.close();
  }

  int pairHash = (1 << (buck.depth - 1)) | hash;
	Bucket lh = read_bucket(hash);
	Bucket rh = read_bucket(pairHash);

	lh.values.clear();
	rh.values.clear();

  for (auto const& p : vec_copy) {
		int new_hash = p.first & make_mask(current_depth);
		if(new_hash ^ pairHash){ // Is different
			lh.insert(p.first, p.second, false);
		} else{
			rh.insert(p.first, p.second, false);
		}
  }
	write_bucket(hash, lh);
	write_bucket(pairHash, rh);
  return true;
}
