#include <algorithm>
#include <bplus/bptree.hpp>
#include <bplus/pagemanager.hpp>
#include <chrono>
#include <fstream>
#include <generator.hpp>
#include <hash/dynamic_hash.hpp>
#include <iostream>
#include <memory>
#include <random_file/random_file.hpp>
#include <string>
#include <cassert>

void safeStrCopy(char *to, std::string from, size_t size) {
  for (size_t idx = 0; idx < std::min(size, from.length()); idx++) {
    to[idx] = from[idx];
  }
}

struct Person {
  using IndexType = int;

  int dni;
  char name[20];
  char lastname[20];
  int age;
  char birth_date[10];

  bool operator<(const Person &p) { return dni < p.dni; }

  Person() = default;

  Person(int dni, std::string name, std::string lastname, int age,
         std::string birth_date) {
    this->dni = dni;
    this->age = age;
    ::safeStrCopy(this->name, name, 20);
    ::safeStrCopy(this->lastname, lastname, 20);
    ::safeStrCopy(this->birth_date, birth_date, 10);
  }

  bool operator==(const Person &other) { return dni == other.dni; }

  friend std::ostream &operator<<(std::ostream &stream, Person &record) {
    stream.write((char *)&record.dni, sizeof(int));
    stream.write((char *)&record.name, 20);
    stream.write((char *)&record.lastname, 20);
    stream.write((char *)&record.age, sizeof(int));
    stream.write((char *)&record.birth_date, sizeof(int));

    stream << "\n";
    stream << std::flush;
    return stream;
  }

  friend std::istream &operator>>(std::istream &stream, Person &record) {
    stream.read((char *)&record.dni, sizeof(int));
    stream.read((char *)&record.name, 20);
    stream.read((char *)&record.lastname, 20);
    stream.read((char *)&record.age, sizeof(int));
    stream.read((char *)&record.birth_date, sizeof(int));
    stream.get();
    return stream;
  }
};

int t = 0; // Carries a counter according to size so files of different
           // testcases dont overlap

int insert_btree_test(std::vector<Person> &vec) {
  auto pm_ptr_1 = std::make_shared<pagemanager>("test/btree_data_1" +
                                                std::to_string(t) + ".bin");
  btree<Person> tree_1(std::move(pm_ptr_1));

  Person tmp;
  for (Person &p : vec) {
    tree_1.insert(p);
  }
	return tree_1.get_disk_access();
}

int search_btree_test(std::vector<Person> &vec) {
  auto pm_ptr_1 = std::make_shared<pagemanager>("test/btree_data_1" +
                                                std::to_string(t) + ".bin");
  btree<Person> tree_1(std::move(pm_ptr_1));

  Person tmp;
  for (Person &p : vec) {
    tree_1.search(p);
  }
	return tree_1.get_disk_access();
}

int rsearch_btree_test(std::vector<Person> &vec) {
  auto pm_ptr_1 = std::make_shared<pagemanager>("test/btree_data_1" +
                                                std::to_string(t) + ".bin");
  btree<Person> tree_1(std::move(pm_ptr_1));
  tree_1.range_search(vec[0], vec[vec.size() - 1]);

	return tree_1.get_disk_access();
}

int insert_dyn_hash_test(std::vector<Person> &vec) {
  DinHash<Person, 10> dhash(5, "test/data_hash" + std::to_string(t) + ".bin");

  for (Person &p : vec) {
    dhash.insert(p.dni, p);
  }
	return dhash.get_disk_access();
}

int search_dyn_hash_test(std::vector<Person> &vec) {
  DinHash<Person, 10> dhash(5, "test/data_hash" + std::to_string(t) + ".bin");

  for (Person &p : vec) {
    dhash.search(p.dni);
  }
	return dhash.get_disk_access();
}
int rsearch_dyn_hash_test(std::vector<Person> &vec) {
  DinHash<Person, 10> dhash(5, "test/data_hash" + std::to_string(t) + ".bin");

  dhash.range_search(vec[0].dni, vec[vec.size()-1].dni);
	return dhash.get_disk_access();
}

int insert_random_file_test(std::vector<Person> vec) {
  RandomFile<Person> rf("test/data_rf" + std::to_string(t) + ".bin",
                        "test/index_rf" + std::to_string(t) + ".bin");

  for (Person &p : vec) {
    rf.insert(p);
  }
	return rf.get_disk_access();
}

int search_random_file_test(std::vector<Person> vec) {
  RandomFile<Person> rf("test/data_rf" + std::to_string(t) + ".bin",
                        "test/index_rf" + std::to_string(t) + ".bin");

  for (Person &p : vec) {
    Person p2 = *rf.search(p.dni).record;
    assert(p2.age == p.age);
  }
	return rf.get_disk_access();
}
int rsearch_random_file_test(std::vector<Person> vec) {
  RandomFile<Person> rf("test/data_rf" + std::to_string(t) + ".bin",
                        "test/index_rf" + std::to_string(t) + ".bin");

	rf.range_search(vec[0].dni, vec[vec.size() - 1].dni);

	return rf.get_disk_access();
}

void load_registers(std::vector<Person> &vec, const std::string &dataset_name) {
  std::ifstream stream_1(dataset_name, std::ifstream::in);

  int dni, age;
  std::string name, lastname, birth_date;
  int counter = 0;
  while (not stream_1.eof() and counter < 10) {
    stream_1 >> dni >> name >> lastname >> age >> birth_date;
    Person reg(dni, name, lastname, age, birth_date);
    vec.emplace_back(reg);
		counter++;
  }
}

int main() {
  const std::string data_1 = "generator/register-dataset.txt";
  std::string namesPath = "generator/datasets/firstnames.txt";
  std::string lastnamesPath = "generator/datasets/lastnames.txt";

  std::ofstream results("results.csv");

  results << "N" << ',' << "BTree Ins Time" << ',' << "BTree Ins DA" << ','
												<<  "BTree Sear" << ',' << "Btree Sear DA" << ','
												<< "BTree RSear Time" << ',' << "BTree RSear DA" << ','

												<< "Random File Ins Time" << ',' << "Random File Ins DA" << ','
												<< "Random File Sear Time" << ',' << "Random File Sear DA" << ','
												<< "Random File RSear Time" << ',' << "Random File RSear DA" << ','

												<< "Dynamic Hash Ins Time" << ','<< "Dynamic Hash Ins DA" << ','
												<< "Dynamic Hash Sear Time" << ',' << "Dynamic Hash Sear DA" << ','         
												<< "Dynamic Hash RSear Time" << ',' << "Dynamic Hash Sear DA" << std::endl;

  for (int N = 10000; N <= 100000; N += 10000) {
    t++;
    std::cout << N << std::endl;
    results << N << ',';

    RegisterGenerator gen(namesPath, lastnamesPath, data_1);
    gen(N);

    std::vector<Person> test_registers;
    load_registers(test_registers, data_1);

		int disk_acc = 0;
    // Time for Btree
    auto start = std::chrono::steady_clock::now();
    disk_acc = insert_btree_test(test_registers);
    auto end = std::chrono::steady_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    results << duration << ',' << disk_acc << ',';

    start = std::chrono::steady_clock::now();
    disk_acc = search_btree_test(test_registers);
    end = std::chrono::steady_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    results << duration << ',' << disk_acc << ',';

    start = std::chrono::steady_clock::now();
    disk_acc = rsearch_btree_test(test_registers);
    end = std::chrono::steady_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    results << duration << ',' << disk_acc << ',';

    // Time for RandomFile
    start = std::chrono::steady_clock::now();
    disk_acc = insert_random_file_test(test_registers);
    end = std::chrono::steady_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    results << duration << ',' << disk_acc << ',';

    start = std::chrono::steady_clock::now();
    disk_acc = search_random_file_test(test_registers);
    end = std::chrono::steady_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    results << duration << ',' << disk_acc << ',';

    start = std::chrono::steady_clock::now();
    disk_acc = rsearch_random_file_test(test_registers);
    end = std::chrono::steady_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    results << duration << ',' << disk_acc << ',';

    // Time for DynamicHash
    start = std::chrono::steady_clock::now();
    disk_acc = insert_dyn_hash_test(test_registers);
    end = std::chrono::steady_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    results << duration << ',' << disk_acc << ',';

    start = std::chrono::steady_clock::now();
    search_dyn_hash_test(test_registers);
    end= std::chrono::steady_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    results << duration << ',' << disk_acc << ',';

    start = std::chrono::steady_clock::now();
    disk_acc = rsearch_dyn_hash_test(test_registers);
    end = std::chrono::steady_clock::now();
    duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    results << duration << ',' << disk_acc << ',' << std::endl;
  }

  results.close();

  return 0;
}
