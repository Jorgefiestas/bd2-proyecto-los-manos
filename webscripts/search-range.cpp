#include "../include/random_file/random_file.hpp"
#include "../include/bplus/bptree.hpp"
#include "../include/bplus/pagemanager.hpp"
#include "../include/hash/dynamic_hash.hpp"
#include <iostream>
#include <memory>

int main() {
	const std::string hash_file = "store/hash.bin";
	const std::string rf_data = "store/rf_data.bin";
	const std::string rf_index = "store/rf_index.bin";
	const std::string bptree_file = "store/bptree.bin";

	int structure, dni;

	std::cin >> structure;
	std::cin >> dni;

	Person reg(dni, name, lastname, age, birth_date);

	switch (structure) {
		case 0:
			DinHash<Person, 10> dh(5, hash_file);
			std::cout << dh.rsearch(dni) << std::endl;
			break;
		case 1:
			RandomFile<Person> rf(rf_data, rf_index);
			std::cout << rf.rsearch(dni) << std::endl;
			break;
		case 2:
		  auto pm_ptr_1 = std::make_shared<pagemanager>(bptree_file);
			btree<Person> bt(std::move(pm_ptr_1));
			std::cout << dh.rsearch(dni) << std::endl;
			break;
	}

	return 0;
}
