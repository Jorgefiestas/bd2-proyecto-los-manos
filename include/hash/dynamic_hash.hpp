#ifndef DYNAMIC_HASH_HPP
#define DYNAMIC_HASH_HPP

#include <fstream>
#include <iostream>
#include <list>
#include <set>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>
#include <sys/stat.h>

//TODO: Join this with file exist in random file. (Maybe utilit.hpp?)
bool exists(const std::string& filename) {
	struct stat buffer;
	return (stat (filename.c_str(), &buffer) == 0);
}

template <class T, int fd> 
struct BaseBucket {
	T values[fd];
	int keys[fd];
	int depth = 0;
	size_t size = 0;

	BaseBucket() = default;
	BaseBucket(int depth);
	int insert(int key, T obj, bool force);
	std::optional<T> search(int key);
};

template <class T, int fd = 2> 
class DinHash {
	private:
		using Bucket = BaseBucket<T, fd>;
		std::string filename;
		int max_depth;
		std::set<int> index;

		unsigned int make_mask(int n) { return (1 << n) - 1; }
		Bucket read_bucket(int pos);
		void write_bucket(int hash, Bucket b);

	public:
		DinHash(int max_depth, const std::string &filename)
				: max_depth(max_depth), filename(filename) {
			std::ofstream out_stream(filename, std::ios::binary | std::ios::trunc);

			Bucket b0(0);
			Bucket b1(0);
			write_bucket(0, b0);
			write_bucket(1, b1);

			index.insert(0);
			index.insert(1);

			out_stream.close();
		}

		void insert(int key, T obj);
		std::optional<T> search(int key);
		std::vector<T> range_search(int start_key, int end_key); 
		bool split_bucket(int hash);
};

template <class T, int fd>
BaseBucket<T, fd>::BaseBucket(int depth) {
	this->depth = depth;
}

template <class T, int fd>
int BaseBucket<T, fd>::insert(int key, T obj, bool force) {
	if (size == fd and not force) {
		return -1;
	}
	if (size == fd and force) {
		// Eviction policy
		return 0;
	}
	values[size] = obj;
	keys[size] = key;
	size++;
	return 1;
}

template <class T, int fd> 
std::optional<T> BaseBucket<T, fd>::search(int key) {
	for (size_t idx = 0; idx < size; idx++) {
		if (keys[idx] == key) {
			return std::make_optional(values[idx]);
		}
	}

	return std::nullopt;
}
// Dynamic Hash

template <class T, int fd> 
typename DinHash<T, fd>::Bucket DinHash<T, fd>::read_bucket(int hash) {
	std::ifstream in_stream(filename, std::ios::binary | std::ios::in);
	in_stream.seekg(hash * sizeof(Bucket) + sizeof(int), std::ios::beg);
	Bucket b;
	in_stream.read((char*) &b, sizeof(Bucket));
	in_stream.close();
	return b;
}

template <class T, int fd>
void DinHash<T, fd>::write_bucket(int hash, Bucket b) {
	std::fstream out_stream(filename);
	out_stream.seekp(hash * sizeof(Bucket) + sizeof(int), std::ios::beg);
	out_stream.write((char*)&b, sizeof(Bucket));
	out_stream.close();
}

template <class T, int fd> 
void DinHash<T, fd>::insert(int key, T obj) {
	for (int depth = max_depth; depth >= 0; depth--) {
		unsigned int hash = key & make_mask(depth);

		if (!index.count(key & make_mask(hash))) continue;

		Bucket b = read_bucket(hash);
		int result = b.insert(key, obj, false);
		if (result == 1 || result == 0) {
			write_bucket(hash, b);
			return;
		}

		bool force = split_bucket(hash);

		if (!force) {
			insert(key, obj);
		}

		return;
	}
}

template <class T, int fd> 
std::optional<T> DinHash<T, fd>::search(int key) {
	for (int depth = max_depth; depth >= 0; depth--) {
		unsigned int hash = key & make_mask(depth);

		if (!index.count(key & make_mask(hash))) continue;

		Bucket b = read_bucket(hash);
		return b.search(key);
	}
	return std::nullopt;
}	

template <class T, int fd> 
std::vector<T> DinHash<T, fd>::range_search(int key_start, int key_end) {
	std::vector<T> vec;
	int hash_start = key_start & make_mask(max_depth);
	int hash_end = key_end& make_mask(max_depth);
	for(int x = hash_start;x <= hash_end;x++){
		auto val = search(x);
		if(val)
			vec.push_back(val.value());
	}
	return vec;
}	

template <class T, int fd>
bool DinHash<T, fd>::split_bucket(int hash) {
	Bucket buck0 = read_bucket(hash);
	int depth = buck0.depth;

	if (depth == max_depth) {
		return true;
	}
	depth++;

	Bucket buck1(depth);
	buck0.depth = depth;

	// Update Index
	int hash1 = hash | (1 << depth);
	index.insert(hash1);

	// Copy values from bucket and clear
	int i1 = 0;
	int d0 = 0;
	for (int i = 0; i < fd; i++) {
		if (buck0.keys[i] & (1 << depth)) {
			buck1.values[i1] = buck0.values[i];
			buck1.keys[i1] = buck0.keys[i];
			i1++;
			d0++;

			buck0.size--;
			buck1.size++;
		}
		else {
			buck0.keys[i - d0] = buck0.keys[i];
			buck0.values[i - d0] = buck0.values[i];
		}
	}

	write_bucket(hash, buck0);
	write_bucket(hash1, buck1);

	return false;
}

#endif
