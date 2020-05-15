#ifndef RANDOM_FILE_HPP
#define RANDOM_FILE_HPP

#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>

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
			struct IndexRecord { char* key;int pos;};
			std::vector<IndexRecord> index_list;
			RandomIndex(){
				//TODO load
			};
			RandomIndex(RandomIndex& ind) = delete;
			RandomIndex& operator=(RandomIndex& ind) = delete;

		};
		std::string main_name;
		std::string index_name;
		std::unique_ptr<RandomIndex> index;

		void save_index();
		void load_index();

	public:
		RandomFile(std::string main_name, std::string index_name) : main_name(main_name), index_name(index_name){}
		FileResponse add(T record);
		FileResponse search(char* key);
		~RandomFile();
};
#endif

namespace fs = std::filesystem;

template <class T>
typename RandomFile<T>::FileResponse RandomFile<T>::add(T record){
	std::size_t main_size = fs::file_size(fs::path(main_name));	
	std::ofstream main_stream(main_name, std::ios::out | std::ios::binary);

	typename RandomFile::RandomIndex::IndexRecord idx_rec {.key = record.codigo, .pos = main_size / sizeof(T)};
	main_stream.seekp(0, std::ios::end);
	main_stream.write((char*)&record, sizeof(T));
	index->index_list.push_back(idx_rec);							

	main_stream.close();
	FileResponse res {.code = ResponseCode::success, .pos = idx_rec.pos};
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
	FileResponse res{.code = ResponseCode::success, .pos = (*it).pos, .record = std::make_optional(obj)}; 
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
