#include "../include/pagemanager.hpp"

pagemanager::pagemanager(std::string file_name, bool trunc)
    : std::fstream(file_name.data(),
                   std::ios::in | std::ios::out | std::ios::binary) {
  empty = false;
  fileName = file_name;
  if (!good() || trunc) {
    empty = true;
    open(file_name.data(),
         std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);
  }
}

pagemanager::~pagemanager() { close(); }

template <class Register> 
void pagemanager::save(const long &n, Register &reg) {
    clear();
    seekp(n * sizeof(Register), std::ios::beg);
    write(reinterpret_cast<const char *>(&reg), sizeof(reg));
}
template <class Register>
bool pagemanager::recover(const long &n, Register &reg) {
    clear();
    seekg(n * sizeof(Register), std::ios::beg);
    read(reinterpret_cast<char *>(&reg), sizeof(reg));
    return gcount() > 0;
}

template <class Register>
void pagemanager::erase(const long &n) {
    char mark;
    clear();
    mark = 'N';
    seekg(n * sizeof(Register), std::ios::beg);
    write(&mark, 1);
}
