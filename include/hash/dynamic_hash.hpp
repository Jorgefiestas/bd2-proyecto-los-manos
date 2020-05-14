#include <memory>
#include <stdexcept>
#include <vector>
#include <list>
#include <map>
#include <optional>

template <class T, int fd> struct Bucket {
  std::map<int, T> values;
  std::vector<T> overflow;
	int depth;
	int size;
	Bucket(int depth, int size){
		depth = depth;
		size = size;
	}

	int insert(int key, T obj, bool force){
		auto it = values.find(key);
		if (values.size() == size and not force)
			return -1;
		if (values.size() == size and force){
			overflow.push_back(obj);		
			return 0;
		}
		values[key] = obj;
		return 1;	
	}
	std::optional<T> search(int key){
		auto it = values.find(key);
		if (it != values.end())
			return std::make_optional(values[key]);
		return std::nullopt;
	}
};

template <class T, int fd = 2> class DinHash {
  using Bucket = Bucket<T, fd>;
  int max_depth;
  std::list<std::shared_ptr<Bucket>> buckets;
	unsigned int make_mask(int n){
		return (1 << n) - 1;
	}

public:
  DinHash(int max_depth) : max_depth(max_depth) {
    auto b0 = std::make_shared<Bucket>();
    auto b1 = std::make_shared<Bucket>();
  }

  void insert(int key, T obj) {
		unsigned int hash = obj & make_mask(max_depth);
    int result = buckets[hash]->insert(hash, obj);
		if (result == 1 || result == 0)
			return;
    bool force = split_bucket(hash, obj);
		insert(key, obj, force);
  }
	std::optional<T> search(int key){
		unsigned int hash = key & make_mask(max_depth);
		return buckets[hash]->search(key);
	}
	bool split_bucket(std::shared_ptr<Bucket> buck, const T& obj){			
		if (buck->depth == max_depth){
			return true;	
		}
		buck->depth++;
		auto vec_copy = buck->values.copy();
		vec_copy.resize(pow(2, buck->depth));
		buckets.resize(pow(2, buck->depth));		
		for(int x = (1 << buck->depth) -1; x > (1 << (buck->depth -1)) -1;x--){
			buckets[x] = nullptr;
		}
		//TODO finish
	}
