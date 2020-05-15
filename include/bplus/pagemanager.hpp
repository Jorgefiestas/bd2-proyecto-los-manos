#ifndef PAGEMANAGER_HPP
#define PAGEMANAGER_HPP

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

class pagemanager : protected std::fstream {
public:
  pagemanager(std::string file_name, bool trunc = false);

  ~pagemanager();

  inline bool is_empty() { return empty; }

  template <class Register> void save(const long &n, Register &reg);

  // template<class Register>
  // long save(Register &reg)
  // {
  //     clear();
  //     seekp(0, std::ios::end);
  //     write(reinterpret_cast<const char *> (&reg), sizeof(reg));

  //     return page_id_count - 1;
  // }

  template <class Register> bool recover(const long &n, Register &reg);

  // Marca el registro como borrado:
  template <class Register> void erase(const long &n);

private:
  std::string fileName;
  int pageSize;
  bool empty;
  long page_id_count;
};

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

template <class Register> void pagemanager::save(const long &n, Register &reg) {
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

template <class Register> void pagemanager::erase(const long &n) {
  char mark;
  clear();
  mark = 'N';
  seekg(n * sizeof(Register), std::ios::beg);
  write(&mark, 1);
}
#endif
