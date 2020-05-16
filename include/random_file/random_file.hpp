#ifndef RANDOM_FILE_HPP
#define RANDOM_FILE_HPP

#include <index.hpp>
#include <sys/stat.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <algorithm>

template <class T>
class RandomFile: public IndexingStructure<T> {
	private:
		using IndexType = typename IndexingStructure<T>::IndexType;
		using ResponseCode = typename IndexingStructure<T>::ResponseCode;
		using FileResponse = typename IndexingStructure<T>::FileResponse;

		struct RandomIndex {
			struct IndexRecord {IndexType key; int pos;};
			std::vector<IndexRecord> index_list;
			RandomIndex(){}
			RandomIndex(const std::string& index_file);
			RandomIndex(RandomIndex& ind) = delete;
			RandomIndex& operator=(RandomIndex& ind) = delete;

		};

		int disk_acceses = 0;
		std::string main_name;
		std::string index_name;
		std::unique_ptr<RandomIndex> index;


		void save_index();
		void load_index();

	public:
		RandomFile(std::string main_name, std::string index_name): 
			main_name(main_name), index_name(index_name){
				load_index();
			}

		int get_disk_access(){
			return disk_acceses;
		}
		FileResponse insert(T record);
		FileResponse search(IndexType key);
		std::vector<T> range_search(IndexType start_key, IndexType end_key);	
		~RandomFile();
};

bool file_exists(const std::string& filename) {
	struct stat buffer;
	return (stat (filename.c_str(), &buffer) == 0);
}

template <class T>
RandomFile<T>::RandomIndex::RandomIndex(const std::string& index_file){
	if (file_exists(index_file)) {
		std::ifstream idx_stream(index_file, std::ios::binary);
		while(true){
			int key, pos;	
			idx_stream.read((char*)&key, sizeof(int));
			idx_stream.read((char*)&pos, sizeof(int));

			if (idx_stream.eof()) {
				break;
			}

			IndexRecord r {.key = key, .pos = pos};
			index_list.push_back(r);
		}
	}
}

template <class T>
typename RandomFile<T>::FileResponse RandomFile<T>::insert(T record){
	typename RandomFile::RandomIndex::IndexRecord idx_rec{};
	
	std::fstream main_stream(main_name, std::ios_base::app | std::ios::binary);
	std::size_t main_size = main_stream.tellp();	

	idx_rec.key = record.dni;
	idx_rec.pos = main_size / sizeof(T);

	main_stream.write((char*)&record, sizeof(T));
	index->index_list.push_back(idx_rec);							
	main_stream.close();
	disk_acceses++;

	FileResponse res {.code = ResponseCode::SUCCESS, .pos = idx_rec.pos};
	return res;
}

template <class T>
typename RandomFile<T>::FileResponse RandomFile<T>::search(IndexType key) {
	auto it = index->index_list.begin();

	while (it != index->index_list.end()) {
		if (it->key == key) break;
		it++;
	}

	if (it == index->index_list.end()){
		FileResponse res {.code = ResponseCode::NOT_FOUND};
		return res;
	}
	
	std::ifstream main_stream(main_name, std::ios::in | std::ios::binary);
	T obj;
	main_stream.seekg((*it).pos * sizeof(T), std::ios::beg);
	main_stream.read((char*)&obj, sizeof(T));
	FileResponse res{.code = ResponseCode::SUCCESS, .pos = (*it).pos, .record = std::make_optional(obj)}; 
	disk_acceses++;
	return res;
}
template <class T>
std::vector<T> RandomFile<T>::range_search(IndexType start_key, IndexType end_key) {
	std::vector<T> vec;
	std::ifstream main_stream(main_name, std::ios::in | std::ios::binary);

	auto it = index->index_list.begin();
	T obj;
	while (it != index->index_list.end()) {
		if(it->key >= start_key && it->key <= end_key){
			main_stream.seekg((*it).pos * sizeof(T), std::ios::beg);
			main_stream.read((char*)&obj, sizeof(T));
			vec.push_back(obj);
			disk_acceses++;
		}
		it++;
	}

	return vec;
}

template <class T>
void RandomFile<T>::load_index(){
	index = std::make_unique<RandomFile::RandomIndex>(index_name);		
}

template <class T>
void RandomFile<T>::save_index() {
	std::ofstream index_stream(index_name, std::ios::out | std::ios::binary);

	std::for_each(index->index_list.begin(), 
			index->index_list.end(), 
			[&index_stream, this](const auto& rec){
				index_stream.write((char*) &rec, sizeof rec);
				disk_acceses++;
			});

	index_stream.close();
}

template <class T> RandomFile<T>::~RandomFile(){
	save_index();
}

#endif
