#include <iostream>
#include <memory>

#include <bplus/bptree.hpp>
#include <bplus/pagemanager.hpp>
#include <hash/dynamic_hash.hpp>
#include <random_file/random_file.hpp>
#include <register/register.hpp>

int main(int argc, char** argv) {
  const std::string hash_file = "store/hash.bin";
  const std::string rf_data = "store/rf_data.bin";
  const std::string rf_index = "store/rf_index.bin";
  const std::string bptree_file = "store/bptree.bin";

  int structure, dni, age;
  std::string name, lastname, birth_date;

  structure = atoi(argv[1]);
  dni = atoi(argv[2]);
  name = argv[3];
  lastname = argv[4];
  age = atoi(argv[5]);
  birth_date = argv[6];

  Person reg(dni, name, lastname, age, birth_date);

  switch (structure) {
  case 0: {
    DinHash<Person, 10> dh(5, hash_file);
    dh.insert(reg.dni, reg);
    break;
  }
  case 1: {
    RandomFile<Person> rf(rf_data, rf_index);
    rf.insert(reg);
    break;
  }
  case 2: {
    auto pm_ptr_1 = std::make_shared<pagemanager>(bptree_file);
    btree<Person> bt(std::move(pm_ptr_1));
    bt.insert(reg);
    break;
  }
  }

  return 0;
}
