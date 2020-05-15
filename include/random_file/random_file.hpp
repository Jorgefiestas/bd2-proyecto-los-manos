#ifndef RANDOM_FILE_HPP
#define RANDOM_FILE_HPP

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
class RandomFile {
	public:
  enum class ResponseCode { SUCCESS, NOT_FOUND, RECORD_INVALID};
  struct FileResponse {
		ResponseCode code;
		std::optional<int> pos;
		std::optional<T> record;
  };
	private:
		struct RandomIndex {
			struct IndexRecord { int key;int pos;};
			std::vector<IndexRecord> index_list;
			RandomIndex(){}
			RandomIndex(const std::string& index_file);
			RandomIndex(RandomIndex& ind) = delete;
			RandomIndex& operator=(RandomIndex& ind) = delete;

		};
		std::string main_name;
		std::string index_name;
		std::unique_ptr<RandomIndex> index;

		void save_index();
		void load_index();

	public:
		RandomFile(std::string main_name, std::string index_name) : main_name(main_name), index_name(index_name){load_index();}
		FileResponse add(T record);
		FileResponse search(char* key);
		~RandomFile();
};
#endif

size_t file_size(const std::string& filename) {
	std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
	return in.tellg();
}

bool file_exists(constconst  std::string& filename) {
	struct stat buffer;
	return (stat (name.c_str(), &buffer) == 0);
}

template <class T>
RandomFile<T>::RandomIndex::RandomIndex(const std::string& index_file){
	if (not file_exists(index_file))
		return;
	std::ifstream idx_stream(index_file, std::ios::binary);
	while(not idx_stream.eof()){
		int key, pos;	
		idx_stream.read((char*)&key, sizeof(int));
		idx_stream.read((char*)&pos, sizeof(int));
		idx_stream.get();
		IndexRecord r {.key = key, .pos = pos};
		index_list.push_back(r);
	}
}

template <class T>
typename RandomFile<T>::FileResponse RandomFile<T>::add(T record){
	typename RandomFile::RandomIndex::IndexRecord idx_rec{};
	if (exists(main_name)){
		std::size_t main_size = file_size(main_name);	
		std::ofstream main_stream(main_name, std::ios::out | std::ios::binary);

		idx_rec.key = record.dni;
		idx_rec.pos = main_size / sizeof(T);

		main_stream.seekp(0, std::ios::end);
		main_stream.write((char*)&record, sizeof(T));
		index->index_list.push_back(idx_rec);							
		main_stream.close();
	} else {
		std::ofstream main_stream(main_name, std::ios::out | std::ios::binary | std::ios::trunc);

		idx_rec.key = record.dni;
		idx_rec.pos = 0;

		main_stream.write((char*)&record, sizeof(T));
		index->index_list.push_back(idx_rec);
		main_stream.close();
	}

	FileResponse res {.code = ResponseCode::SUCCESS, .pos = idx_rec.pos};
	return res;
}

template <class T>
typename RandomFile<T>::FileResponse RandomFile<T>::search(char* key){
	std::string str_key(key);
	auto it = std::find_if(index->index_list.begin(), index->index_list.end(), [&](const auto& rec){str_key == rec.key;});		

	if (it == index->index_list.end()){
		FileResponse res {.code = ResponseCode::not_found};
		return res;
	}
	
	std::ifstream main_stream(main_name, std::ios::in | std::ios::binary);
	T obj;
	main_stream.seekg((*it).pos * sizeof(T), std::ios::beg);
	main_stream.read((char*)&obj, sizeof(T));
	FileResponse res{.code = ResponseCode::SUCCESS, .pos = (*it).pos, .record = std::make_optional(obj)}; 
	return res;
}

template <class T>
void RandomFile<T>::load_index(){
	index = std::make_unique<RandomFile::RandomIndex>(index_name);		
}
template <class T>
void RandomFile<T>::save_index(){
	std::ofstream index_stream(index_name, std::ios::out | std::ios::binary);
	std::for_each(index->index_list.begin(), index->index_list.end(), [&index_stream](const auto& rec){ 
			index_stream.write((char*)&rec, sizeof(decltype(rec)));});
	index_stream.close();
}
template <class T>
RandomFile<T>::~RandomFile(){
	save_index();
}
