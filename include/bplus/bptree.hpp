#ifndef BPTREE_HPP
#define BPTREE_HPP

#include "pagemanager.hpp"
#include <iterator>
#include <memory>
#include <optional>
#include <string>

enum state { BT_OVERFLOW, BT_UNDERFLOW, NORMAL };

template <class T, int BTREE_ORDER = 3> class btree {
public:
	struct node {
		bool leaf = true;
		long page_id{-1};
		long next{-1};
		long prev{-1};
		long count{0};

		T data[BTREE_ORDER + 1];
		long children[BTREE_ORDER + 2];

		node(long page_id);

		void insert_in_node(int pos, const T &value);
		bool is_overflow();

		bool operator==(const node &n) const {
			return page_id == n.page_id && count == n.count;
		}

		bool operator!=(const node &n) const { return !(*this == n); }
	};

	struct Metadata {
		long root_id{1};
		long count{0};
	} header;

	class iterator
			: public std::iterator<std::forward_iterator_tag, T, T, T *, T &> {
	private:
		std::optional<node> root;
		std::optional<node> end;
		int count = 0;
		btree<T, BTREE_ORDER> *instance;

	public:
		iterator(btree *inst) : instance(inst){};
		iterator(btree *inst, node beg) : instance(inst), root(beg){};
		iterator(btree *inst, node beg, int count)
				: instance(inst), root(beg), count(count){};
		iterator(btree *inst, node beg, node end)
				: instance(inst), root(beg), end(end){};

		iterator &operator++();
		iterator operator++(int);

		bool operator==(iterator it) const;
		bool operator!=(iterator it) const;

		T operator*();
	};

private:
	std::shared_ptr<pagemanager> pm;

public:
	btree(std::shared_ptr<pagemanager> pm);

	iterator begin();
	iterator end();

	node new_node();

	node read_node(long page_id);
	void write_node(long page_id, node n);

	void insert(const T &value);
	int insert(node &ptr, const T &value);

	void split(node &parent, int pos);
	void split_root();

	iterator find(const T &value);
	iterator find(node &ptr, const T &value);

	std::optional<node> find_node(const T &value);
	std::optional<node> find_node(node &ptr, const T &value);

	iterator range_search(const T &begin, const T &end);

	void print(std::ostream &out, std::string separator);
	void print(node &ptr, int level, std::ostream &out, std::string separator);

	void print_tree();
	void print_tree(node &ptr, int level);
};
#endif
