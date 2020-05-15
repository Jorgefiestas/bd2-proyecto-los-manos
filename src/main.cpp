#include <bplus/pagemanager.hpp>
#include <hash/dynamic_hash.hpp>
#include <bplus/bptree.hpp>
#include <random_file/random_file.hpp>
#include <iostream>
#include <chrono>
#include <memory>
#include <fstream>
#include <string>


struct Person {
  int dni;
  char name[20];
  char lastname[20];
  int age;
  char birth_date[10];

	bool operator <(const Person& p){
		return dni < p.dni;
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


void btree_test(const std::string& dataset1, const std::string& dataset2) {
	auto pm_ptr_1 = std::make_shared<pagemanager>("btree_data_1.bin");
	auto pm_ptr_2 = std::make_shared<pagemanager>("btree_data_2.bin");
	btree<Person> tree_1(std::move(pm_ptr_1));
	btree<Person> tree_2(std::move(pm_ptr_2));

	std::ifstream stream_1(dataset1);
	
	Person tmp;
	while (not stream_1.eof()){
		stream_1 >> tmp;
		tree_1.insert(tmp);
	}
		
	//TODO LOAD FILE WITH DATA	
	//TODO INSERT IN TREE	
}
void insert_dyn_hash_test(const std::string& dataset1) {
	DinHash<Person, 3> dhash(5, "data_hash.bin");
	std::ifstream stream_1(dataset1);

	Person tmp;
	while(not stream_1.eof()){
		stream_1 >> tmp;
		dhash.insert(tmp.dni, tmp);
	}
}
void insert_random_file_test(const std::string& dataset1) {
	RandomFile<Person> rf("data_rf.bin", "index_rf.bin");
	std::ifstream stream_1(dataset1);

	Person tmp;
	while(not stream_1.eof()){
		stream_1 >> tmp;
		rf.add(tmp);
	}
}

int main() {
	const std::string data_1 = "../generator/register-dataset.txt";
	const std::string data_2 = "../generator/register-dataset.txt";


	// Time for B+
	auto start = std::chrono::steady_clock::now();
	btree_test(data_1, data_2);
	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "B+ Tree took "  << duration << "ms" << std::endl;

	//Time for RandomFile
	auto start_2 = std::chrono::steady_clock::now();
	insert_random_file_test(data_1);
	auto end_2 = std::chrono::steady_clock::now();
	auto duration_2 = std::chrono::duration_cast<std::chrono::milliseconds>(end_2 - start_2).count();
	std::cout << "Random File took "  << duration_2 << "ms" << std::endl;

	//Time for DynamicHash
	auto start_3 = std::chrono::steady_clock::now();
	insert_dyn_hash_test(data_1);
	auto end_3 = std::chrono::steady_clock::now();
	auto duration_3 = std::chrono::duration_cast<std::chrono::milliseconds>(end_3 - start_3).count();
	std::cout << "Dynamic hash took "  << duration_2 << "ms" << std::endl;
	return 0; 
}
