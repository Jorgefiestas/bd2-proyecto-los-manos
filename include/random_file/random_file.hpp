#ifndef RANDOM_FILE_HPP
#define RANDOM_FILE_HPP

#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

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
