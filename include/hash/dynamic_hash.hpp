#ifndef DYNAMIC_HASH_HPP
#define DYNAMIC_HASH_HPP

#include <fstream>
#include <iostream>
#include <list>
#include <map>
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

template <class T, int fd> struct BaseBucket {
	std::map<int, T> values;
	// std::vector<T> overflow;
	int depth;
	int size;

	BaseBucket(int depth, int size);
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
		if (not exists(filename)) {
			current_depth = 1;
			std::ofstream out_stream(filename, std::ios::binary | std::ios::trunc);

			Bucket b0(1, 1);
			Bucket b1(1, 1);

			out_stream.write((char *)&current_depth, sizeof(int));
			out_stream.write((char *)&b0, sizeof(Bucket));
			out_stream.write((char *)&b1, sizeof(Bucket));
			out_stream.close();
			return;
		}
		this->filename = filename;
		std::ifstream in_stream(filename, std::ios::binary | std::ios::in);
		in_stream.read((char *)&current_depth, sizeof(int));
	}

		void insert(int key, T obj);
		std::optional<T> search(int key);
		bool split_bucket(int hash, const T &obj);
};


template <class T, int fd> BaseBucket<T, fd>::BaseBucket(int depth, int size) {
	depth = depth;
	size = size;
}

template <class T, int fd>
int BaseBucket<T, fd>::insert(int key, T obj, bool force) {
	if (values.size() == size and not force)
		return 0;
	if (values.size() == size and force) {
		// overflow.push_back(obj);
		return 0;
	}
	values[key] = obj;
	return 1;

}
template <class T, int fd> std::optional<T> BaseBucket<T, fd>::search(int key) {

	auto it = values.find(key);
	if (it != values.end())
		return std::make_optional(values[key]);
	return std::nullopt;
}
// Dynamic Hash

template <class T, int fd> typename DinHash<T, fd>::Bucket DinHash<T, fd>::read_bucket(int hash) {
	std::ifstream in_stream(filename, std::ios::binary | std::ios::in);
	in_stream.seekg(hash * sizeof(Bucket) + sizeof(int), std::ios::beg);
	Bucket b;
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
	unsigned int hash = obj & make_mask(current_depth);
	Bucket b = read_bucket(hash);
	int result = b->insert(hash, obj);
	if (result == 1 || result == 0)
		write_bucket(hash, b);
	bool force = split_bucket(hash, obj);
	insert(key, obj, force);
}

template <class T, int fd> std::optional<T> DinHash<T, fd>::search(int key) {
	unsigned int hash = key & make_mask(current_depth);
	Bucket b = read_bucket(hash);
	return b->search(key);
}
template <class T, int fd>
bool DinHash<T, fd>::split_bucket(int hash, const T &obj) {
	Bucket buck = read_bucket(hash);
	if (buck->depth == max_depth) {
		return false;
	}
	buck->depth++;


	// Copy values from bucket and clear
	auto vec_copy = buck->values.copy();
	vec_copy[hash] = obj;
	buck->values.clear();


	// Create new buckets if they don't exist
	if (current_depth != buck->depth) {
		current_depth = buck->depth;
		for (int x = (1 << current_depth) - 1; x > (1 << (current_depth - 1)) - 1;
				 x--) {
			Bucket tmp;
			write_bucket(x, tmp);
		}
		std::ofstream out_file(filename, std::ios::binary | std::ios::out);
		out_file.write((char*)&current_depth, sizeof(int));
	}

	int pairHash = (1 << (buck->depth - 1)) | hash;
	for (const auto &[k, v] : vec_copy) {
		Bucket b = read_bucket(k);
		b->insert(k, v);
		write_bucket(k,	b);
	}
	return true;

}

#endif
