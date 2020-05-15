#include <bplus/pagemanager.hpp>
#include <hash/dynamic_hash.hpp>
#include <bplus/bptree.hpp>
#include <random_file/random_file.hpp>
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


void btree_test(std::vector<Person> &vec) {
	auto pm_ptr_1 = std::make_shared<pagemanager>("btree_data_1.bin");
	auto pm_ptr_2 = std::make_shared<pagemanager>("btree_data_2.bin");
	btree<Person> tree_1(std::move(pm_ptr_1));
	btree<Person> tree_2(std::move(pm_ptr_2));

	
	Person tmp;
	for (Person &p : vec) {
		tree_1.insert(p);
	}
}

void insert_dyn_hash(const std::string& dataset1) {
}

void insert_random_file_test(std::vector<Person> vec) {
	RandomFile<Person> rf("data_rf.bin", "index_rf.bin");

	for (Person &p : vec) {
		rf.add(p);
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
	const std::string data_2 = "generator/register-dataset.txt";

	std::vector<Person> test_registers;
	load_registers(test_registers, data_1);

	// Time for B+
	auto start = std::chrono::steady_clock::now();
	btree_test(test_registers);
	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "B+ Tree took "	<< duration << "ms" << std::endl;

	//Time for RandomFile
	auto start_2 = std::chrono::steady_clock::now();
	insert_random_file_test(test_registers);
	auto end_2 = std::chrono::steady_clock::now();
	auto duration_2 = std::chrono::duration_cast<std::chrono::milliseconds>(end_2 - start_2).count();
	std::cout << "Random File took "	<< duration_2 << "ms" << std::endl;

	return 0; 
}
