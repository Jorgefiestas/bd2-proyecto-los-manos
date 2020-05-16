#include <iostream>
#include <memory>

#include <register/register.hpp>
#include <random_file/random_file.hpp>
#include <bplus/bptree.hpp>
#include <bplus/pagemanager.hpp>
#include <hash/dynamic_hash.hpp>

int main() {
	const std::string hash_file = "store/hash.bin";
	const std::string rf_data = "store/rf_data.bin";
	const std::string rf_index = "store/rf_index.bin";
	const std::string bptree_file = "store/bptree.bin";

	int structure, dni_1, age_1, dni_2, age_2;
  std::string name_1, name_2, lastname_1, lastname_2, birth_date_1, birth_date_2;

	std::cin >> structure;

  std::cin >> dni_1 >> name_1 >> lastname_1 >> age_1 >> birth_date_1;
  std::cin >> dni_2 >> name_2 >> lastname_2 >> age_2 >> birth_date_2;

	Person reg_1(dni_1, name_1, lastname_1, age_1, birth_date_1);
	Person reg_2(dni_2, name_2, lastname_2, age_2, birth_date_2);

	switch (structure) {
		case 0:
			{
			DinHash<Person, 10> dh(5, hash_file);
			auto vec = dh.range_search(dni_1, dni_2);
			for (auto& e : vec){
				std::cout << e << std::endl;
			}
			break;
			}
		case 1:
			{
			RandomFile<Person> rf(rf_data, rf_index);
			auto vec =  rf.range_search(dni_1, dni_2);
			for (auto& e : vec){
				std::cout << e << std::endl;
			}
			break;
			}
		case 2:
			{
		  auto pm_ptr_1 = std::make_shared<pagemanager>(bptree_file);
			btree<Person> bt(std::move(pm_ptr_1));
			auto vec = bt.range_search(reg_1, reg_2);
			for (auto& e : vec){
				std::cout << e << std::endl;
			}
			break;
			}
	}

	return 0;
}
