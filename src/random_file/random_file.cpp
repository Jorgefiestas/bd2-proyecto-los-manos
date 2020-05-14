#include <random_file/random_file.hpp>
#include <algorithm>
#include <optional>
#include <filesystem>

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
	FileResponse res {.code = ResponseCode::SUCCESS, .pos = idx_rec.pos};
	return res;
}

template <class T>
typename RandomFile<T>::FileResponse RandomFile<T>::search(char* key){
	std::string str_key(key);
	auto it = std::find_if(index->index_list.begin(), index->index_list.end(), [&](const auto& rec){str_key == rec.key;});		

	if (it == index->index_list.end()){
		FileResponse res {.code = ResponseCode::NOT_FOUND};
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
