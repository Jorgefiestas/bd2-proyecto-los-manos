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

    template <class Register> 
    void save(const long &n, Register &reg);

  // template<class Register>
  // long save(Register &reg)
  // {
  //     clear();
  //     seekp(0, std::ios::end);
  //     write(reinterpret_cast<const char *> (&reg), sizeof(reg));

  //     return page_id_count - 1;
  // }

    template <class Register>
    bool recover(const long &n, Register &reg);

    // Marca el registro como borrado:
    template <class Register>
    void erase(const long &n);

    private:
        std::string fileName;
        int pageSize;
        bool empty;
        long page_id_count;
    };
#endif
