#include "../include/random-file/randomfile.hpp"
#include "../include/bplus/bptree.hpp"

int main() {
	const std::string hash_file = "store/hash.bin";
	const std::string rf_data = "store/rf_data.bin";
	const std::string rf_index = "store/rf_index.bin";
	const std::string bptree_file = "store/bptree.bin";

	int structure, dni, age;
	std::string structure;
	std::string name, lastname, birth_date;

	std::cin >> structure;
	std::cin >> dni >> name >> lastname >> age >> birth_date;

	Person reg(dni, name, lastname, age, birth_date);

	switch (structure) {
		case 0:
			DinHash<Person, 10> dh(5, hash_file);
			dh.insert(reg.dni, reg);
			break;
		case 1:
			RandomFile<Person> rf(rf_data, rf_index);
			rf.insert(reg);
			break;
		case 2:
			btree<Person> bt(bptree_file);
			dh.insert(reg);
			break;
	}

	return 0;
}
