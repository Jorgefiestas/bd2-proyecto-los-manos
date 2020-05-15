#ifndef INDEX_HPP
#define INDEX_HPP

template <class T>
class IndexingStructure {
	protected:
		using IndexType = typename T::IndexType;

		enum class ResponseCode {SUCCESS, NOT_FOUND, RECORD_INVALID};

		struct FileResponse {
			ResponseCode code;
			std::optional<int> pos;
			std::optional<T> record;
		};


	public:
		virtual FileResponse insert(T) = 0;
		virtual FileResponse search(IndexType) = 0;
};

#endif
