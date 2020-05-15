#include <bplus/pagemanager.hpp>
#include <hash/dynamic_hash.hpp>
#include <bplus/bptree.hpp>
#include <random_file/random_file.hpp>
#include <generator.hpp>
#include <iostream>
#include <chrono>
#include <memory>
#include <fstream>
#include <string>
#include <algorithm>

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

	bool operator <(const Person& p){
		return dni < p.dni;
	}

	Person() = default;

	Person(int dni, std::string name, std::string lastname, int age, std::string birth_date) {
		this->dni = dni;
		this->age = age;
		::safeStrCopy(this->name, name, 20);
		::safeStrCopy(this->lastname, lastname, 20);
		::safeStrCopy(this->birth_date, birth_date, 10);
	}

	bool operator ==(const Person &other) {
		return dni == other.dni;
	}

	friend std::ostream &operator<<(std::ostream& stream, Person& record) {
		stream.write((char *)&record.dni, sizeof(int));
		stream.write((char *)&record.name, 20);
		stream.write((char *)&record.lastname, 20);
		stream.write((char *)&record.age, sizeof(int));
		stream.write((char*)&record.birth_date, sizeof(int));

		stream << "\n";
		stream << std::flush;
		return stream;
	}

	friend std::istream &operator>>(std::istream &stream, Person& record) {
		stream.read((char*)&record.dni, sizeof(int));
		stream.read((char*)&record.name, 20);
		stream.read((char*)&record.lastname, 20);
		stream.read((char*)&record.age, sizeof(int));
		stream.read((char*)&record.birth_date, sizeof(int));
		stream.get();
		return stream;
	}
};


int t = 0; // Carries a counter according to size so files of different testcases dont overlap

void insert_btree_test(std::vector<Person> &vec) {
	auto pm_ptr_1 = std::make_shared<pagemanager>("test/btree_data_1" + std::to_string(t) + ".bin");
	btree<Person> tree_1(std::move(pm_ptr_1));

	
	Person tmp;
	for (Person &p : vec) {
		tree_1.insert(p);
	}
}

void search_btree_test(std::vector<Person> &vec) {
	auto pm_ptr_1 = std::make_shared<pagemanager>("test/btree_data_1" + std::to_string(t) + ".bin");
	btree<Person> tree_1(std::move(pm_ptr_1));

	
	Person tmp;
	for (Person &p : vec) {
		tree_1.search(p);
	}
}

void insert_dyn_hash_test(std::vector<Person> &vec) {
	DinHash<Person, 10> dhash(5, "test/data_hash" + std::to_string(t) + ".bin");

	for(Person &p : vec) {
		dhash.insert(p.dni, p);
	}
}

void search_dyn_hash_test(std::vector<Person> &vec) {
	DinHash<Person, 10> dhash(5, "test/data_hash" + std::to_string(t) + ".bin");

	for(Person &p : vec) {
		dhash.search(p.dni);
	}
}

void insert_random_file_test(std::vector<Person> vec) {
	RandomFile<Person> rf("test/data_rf" + std::to_string(t) + ".bin", "test/index_rf" + std::to_string(t) + ".bin");

	for (Person &p : vec) {
		rf.insert(p);
	}
}

void search_random_file_test(std::vector<Person> vec) {
	RandomFile<Person> rf("test/data_rf" + std::to_string(t) + ".bin", "test/index_rf" + std::to_string(t) + ".bin");

	int i = 0;
	for (Person &p : vec) {
		Person p2 = *rf.search(p.dni).record;
		assert(p2.age == p.age);
	}
}

void load_registers(std::vector<Person> &vec, const std::string &dataset_name) {
	std::ifstream stream_1(dataset_name, std::ifstream::in);

	int dni, age;
	std::string name, lastname, birth_date;
	while (not stream_1.eof()) {
		stream_1 >> dni >> name >> lastname >> age >> birth_date;
		Person reg(dni, name, lastname, age, birth_date);
		vec.emplace_back(reg);
	}
}

int main() {
	const std::string data_1 = "generator/register-dataset.txt";
	std::string namesPath = "generator/datasets/firstnames.txt";
	std::string lastnamesPath = "generator/datasets/lastnames.txt";


	std::ofstream results("results.csv");

	results << "N" << ',' << "BTree Ins" << ',' << "BTree Sear" << ',' << "Random File Ins" << ',' << "Random File Sear" << ',' << "Dynamic Hash Ins" << ',' << "Dynamic Hash Ins" << std::endl;

	for (int N = 10000; N <= 100000; N += 10000) {
		t++;
		std::cout << N << std::endl;
		results << N << ',';

		RegisterGenerator gen(namesPath, lastnamesPath, data_1);
		gen(N);

		std::vector<Person> test_registers;
		load_registers(test_registers, data_1);

		// Time for Btree
		auto start = std::chrono::steady_clock::now();
		insert_btree_test(test_registers);
		auto end = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		results << duration << ',';

		start = std::chrono::steady_clock::now();
		search_btree_test(test_registers);
		end = std::chrono::steady_clock::now();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		results << duration << ',';

		//Time for RandomFile
		auto start_2 = std::chrono::steady_clock::now();
		insert_random_file_test(test_registers);
		auto end_2 = std::chrono::steady_clock::now();
		auto duration_2 = std::chrono::duration_cast<std::chrono::milliseconds>(end_2 - start_2).count();
		results << duration_2 << ',';

		start_2 = std::chrono::steady_clock::now();
		search_random_file_test(test_registers);
		end_2 = std::chrono::steady_clock::now();
		duration_2 = std::chrono::duration_cast<std::chrono::milliseconds>(end_2 - start_2).count();
		results << duration_2 << ',';

		//Time for DynamicHash
		auto start_3 = std::chrono::steady_clock::now();
		insert_dyn_hash_test(test_registers);
		auto end_3 = std::chrono::steady_clock::now();
		auto duration_3 = std::chrono::duration_cast<std::chrono::milliseconds>(end_3 - start_3).count();
		results << duration_3 << ',';

		start_3 = std::chrono::steady_clock::now();
		search_dyn_hash_test(test_registers);
		end_3 = std::chrono::steady_clock::now();
		duration_3 = std::chrono::duration_cast<std::chrono::milliseconds>(end_3 - start_3).count();
		results << duration_3 << std::endl;
	}

	results.close();

	return 0; 
}
