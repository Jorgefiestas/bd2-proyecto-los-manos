#include <bplus/bptree.hpp>
#include <optional>

template <class T, int BTREE_ORDER>
btree<T, BTREE_ORDER>::node::node(long page_id) : page_id{page_id} {
  count = 0;
  for (int i = 0; i < BTREE_ORDER + 2; i++) {
    children[i] = 0;
  }
}

template <class T, int BTREE_ORDER>
void btree<T, BTREE_ORDER>::node::insert_in_node(int pos, const T &value) {
  int j = count;
  while (j > pos) {
    data[j] = data[j - 1];
    children[j + 1] = children[j];
    j--;
  }
  data[j] = value;
  children[j + 1] = children[j];
  count++;
}

template <class T, int BTREE_ORDER>
bool btree<T, BTREE_ORDER>::node::is_overflow() {
  return count > BTREE_ORDER;
}

template <class T, int BTREE_ORDER>
btree<T, BTREE_ORDER>::btree(std::shared_ptr<pagemanager> pm) : pm{pm} {
  if (pm->is_empty()) {
    node root{header.root_id};
    pm->save(root.page_id, root);

    header.count++;

    pm->save(0, header);
  } else {
    pm->recover(0, header);
  }
}

template <class T, int BTREE_ORDER>
typename btree<T, BTREE_ORDER>::node btree<T, BTREE_ORDER>::new_node() {
  header.count++;
  node ret{header.count};
  pm->save(0, header);
  return ret;
}

template <class T, int BTREE_ORDER>
typename btree<T, BTREE_ORDER>::node
btree<T, BTREE_ORDER>::read_node(long page_id) {
  node n{-1};
  pm->recover(page_id, n);
  return n;
}

template <class T, int BTREE_ORDER>
void btree<T, BTREE_ORDER>::write_node(long page_id, node n) {
  pm->save(page_id, n);
}

template <class T, int BTREE_ORDER>
void btree<T, BTREE_ORDER>::insert(const T &value) {
  node root = read_node(header.root_id);
  int state = insert(root, value);

  if (state == BT_OVERFLOW) {
    split_root();
  }
}

template <class T, int BTREE_ORDER>
int btree<T, BTREE_ORDER>::insert(node &ptr, const T &value) {
  int pos = 0;
  while (pos < ptr.count && ptr.data[pos] < value) {
    pos++;
  }

  if (ptr.children[pos] != 0) {
    long page_id = ptr.children[pos];
    node child = read_node(page_id);
    int state = insert(child, value);
    if (state == BT_OVERFLOW) {
      split(ptr, pos);
    }

  } else {
    ptr.insert_in_node(pos, value);
    write_node(ptr.page_id, ptr);
  }
  return ptr.is_overflow() ? BT_OVERFLOW : NORMAL;
}

template <class T, int BTREE_ORDER>
void btree<T, BTREE_ORDER>::split(node &parent, int pos) {
  node ptr = this->read_node(parent.children[pos]);
  node left = this->new_node();
  node right = this->new_node();

  // set the leaf status upon updating the structure
  left.leaf = ptr.leaf;
  right.leaf = ptr.leaf;

  // assign the page_id as a sort of pointer for the linked list of leaves
  if (ptr.leaf) {
    ptr.next = -1;
    left.next = right.page_id;
    right.prev = left.page_id;
  }

  int iter = 0;
  int i;

  for (i = 0; iter < BTREE_ORDER / 2; i++) {
    left.children[i] = ptr.children[iter];
    left.data[i] = ptr.data[iter];
    left.count++;
    iter++;
  }
  left.children[i] = ptr.children[iter];

  parent.insert_in_node(pos, ptr.data[iter]);

  // If its not a leaf the value is not copied but moved
  if (!ptr.leaf)
    iter++;

  for (i = 0; iter < BTREE_ORDER + 1; i++) {
    right.children[i] = ptr.children[iter];
    right.data[i] = ptr.data[iter];
    right.count++;

    iter++;
  }
  right.children[i] = ptr.children[iter];

  // Update leaf status and childrens
  ptr.leaf = false;
  parent.leaf = false;
  parent.children[pos] = left.page_id;
  parent.children[pos + 1] = right.page_id;

  write_node(parent.page_id, parent);
  write_node(left.page_id, left);
  write_node(right.page_id, right);
}

template <class T, int BTREE_ORDER> void btree<T, BTREE_ORDER>::split_root() {
  node ptr = this->read_node(this->header.root_id);
  node left = this->new_node();
  node right = this->new_node();

  int pos = 0;
  int iter = 0;
  int i;

  // If the leaf is not a leaf then their children wont be leaves either
  // We set the linking of the leaves if they are created
  if (!ptr.leaf) {
    left.leaf = false;
    right.leaf = false;
  }

  for (i = 0; iter < BTREE_ORDER / 2; i++) {
    left.children[i] = ptr.children[iter];
    left.data[i] = ptr.data[iter];
    left.count++;
    iter++;
  }
  left.children[i] = ptr.children[iter];

  // If the node is not a leaf the data is moved not copied
  if (!ptr.leaf) {
    ptr.data[0] = ptr.data[iter];
    iter++;
  }

  for (i = 0; iter < BTREE_ORDER + 1; i++) {
    right.children[i] = ptr.children[iter];
    right.data[i] = ptr.data[iter];
    right.count++;
    iter++;
  }
  right.children[i] = ptr.children[iter];

  // Updating the children
  ptr.children[pos] = left.page_id;
  ptr.children[pos + 1] = right.page_id;
  ptr.count = 1;

  // If the node is a leaf then copy the data and set a pointer to next leaf
  if (ptr.leaf) {
    ptr.next = -1;
    ptr.data[0] = right.data[0];
    left.next = right.page_id;
    right.prev = left.page_id;
  }

  // Change to internal node because it has children now
  ptr.leaf = false;

  write_node(ptr.page_id, ptr);
  write_node(left.page_id, left);
  write_node(right.page_id, right);
}

template <class T, int BTREE_ORDER>
void btree<T, BTREE_ORDER>::print(std::ostream &out, std::string separator) {
  node root = read_node(header.root_id);
  print(root, 0, out, separator);
}

template <class T, int BTREE_ORDER>
void btree<T, BTREE_ORDER>::print(node &ptr, int level, std::ostream &out,
                                  std::string separator) {
  int i;
  for (i = 0; i < ptr.count; i++) {
    if (ptr.children[i]) {
      node child = read_node(ptr.children[i]);
      print(child, level + 1, out, separator);
    }
    if (ptr.leaf)
      out << ptr.data[i] << separator;
  }
  if (ptr.children[i]) {
    node child = read_node(ptr.children[i]);
    print(child, level + 1, out, separator);
  }
}

template <class T, int BTREE_ORDER> void btree<T, BTREE_ORDER>::print_tree() {
  node root = read_node(header.root_id);
  print_tree(root, 0);
  std::cout << "________________________\n";
}

template <class T, int BTREE_ORDER>
void btree<T, BTREE_ORDER>::print_tree(node &ptr, int level) {
  int i;
  for (i = ptr.count - 1; i >= 0; i--) {
    if (ptr.children[i + 1]) {
      node child = read_node(ptr.children[i + 1]);
      print_tree(child, level + 1);
    }

    for (int k = 0; k < level; k++) {
			std::cout << "    ";
    }
		std::cout << ptr.data[i] << std::endl;
  }
  if (ptr.children[i + 1]) {
    node child = read_node(ptr.children[i + 1]);
    print_tree(child, level + 1);
  }
}
template <class T, int BTREE_ORDER>
typename btree<T, BTREE_ORDER>::iterator btree<T, BTREE_ORDER>::begin() {
  node root = read_node(header.root_id);
  return iterator(this, root);
}

template <class T, int BTREE_ORDER>
typename btree<T, BTREE_ORDER>::iterator btree<T, BTREE_ORDER>::end() {
  return iterator(this);
}
template <class T, int BTREE_ORDER>
typename btree<T, BTREE_ORDER>::iterator
btree<T, BTREE_ORDER>::find(const T &value) {
  node root = read_node(header.root_id);
  return find(root, value);
}

template <class T, int BTREE_ORDER>
typename btree<T, BTREE_ORDER>::iterator
btree<T, BTREE_ORDER>::find(node &ptr, const T &value) {
  int pos = 0;
  while (pos < ptr.count && ptr.data[pos] < value) {
    pos++;
  }

  // Substracting 1 because of the <= which will land us 1 position ahead in
  // case we are on the leaf containing our value
  if (ptr.leaf && ptr.data[pos] == value) {
    return iterator(this, ptr, pos);
  }

  if (ptr.children[pos]) {
    node child = read_node(ptr.children[pos]);
    return find(child, value);
  }
  return iterator(this);
}

template <class T, int BTREE_ORDER>
std::optional<typename btree<T, BTREE_ORDER>::node>
btree<T, BTREE_ORDER>::find_node(const T &value) {
  node root = read_node(header.root_id);
  return find_node(root, value);
}

template <class T, int BTREE_ORDER>
std::optional<typename btree<T, BTREE_ORDER>::node>
btree<T, BTREE_ORDER>::find_node(node &ptr, const T &value) {
  int pos = 0;
  while (pos < ptr.count && ptr.data[pos] < value) {
    pos++;
  }

  // Substracting 1 because of the <= which will land us 1 position ahead in
  // case we are on the leaf containing our value
  if (ptr.leaf && ptr.data[pos] == value)
    return ptr;
  if (ptr.children[pos]) {
    node child = read_node(ptr.children[pos]);
    return find_node(child, value);
  }
  return std::nullopt;
}

template <class T, int BTREE_ORDER>
typename btree<T, BTREE_ORDER>::iterator
btree<T, BTREE_ORDER>::range_search(const T &begin, const T &end) {
	std::optional<typename btree<T, BTREE_ORDER>::node> start = find_node(begin);
	std::optional<typename btree<T, BTREE_ORDER>::node> finish = find_node(end);

  if (start)
    return finish ? iterator(this, start.value(), finish.value())
                  : iterator(this, start.value());
  return iterator(this);
}

template <class T, int BTREE_ORDER>
typename btree<T, BTREE_ORDER>::iterator &
btree<T, BTREE_ORDER>::iterator::operator++() {

  // Check wether the value is a null
  if (!root) {
    return *this;
  }

  // Traverse all the data in the node
  if (root.value().count - 1 > count) {
    count++;
    return *this;
  }

  // If its the final iterator
  if (end && root.value() == end.value()) {
    root = std::nullopt;
    return *this;
  }

  // If doesn't have another iterator
  if (root.value().next == -1) {
    root = std::nullopt;
    return *this;
  }

  root = make_optional(instance->read_node(root.value().next));
  count = 0;
  return *this;
}

template <class T, int BTREE_ORDER>
typename btree<T, BTREE_ORDER>::iterator
btree<T, BTREE_ORDER>::iterator::operator++(int) {
  iterator it = *this;
  ++(*this);
  return it;
}

template <class T, int BTREE_ORDER>
bool btree<T, BTREE_ORDER>::iterator::operator==(iterator it) const {
  if (!it.root && !root)
    return true;
  if (!it.root || !root)
    return false;
  return it.root.value() == root.value();
}

template <class T, int BTREE_ORDER>
bool btree<T, BTREE_ORDER>::iterator::operator!=(iterator it) const {
  return !((*this) == it);
}

template <class T, int BTREE_ORDER>
T btree<T, BTREE_ORDER>::iterator::operator*() {
  return root.value().data[count];
}
