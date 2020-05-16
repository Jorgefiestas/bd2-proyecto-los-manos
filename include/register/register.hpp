#ifndef REGISTER_HPP 
#define REGISTER_HPP

#include <algorithm>

void safeStrCopy(char *to, std::string from, size_t size) {
  for (size_t idx = 0; idx < std::min(size, from.length()); idx++) {
    to[idx] = from[idx];
  }
}

struct Person {
  using IndexType = int;

  int dni;
  char name[20];
  char lastname[20];
  int age;
  char birth_date[10];

  bool operator<(const Person &p) { return dni < p.dni; }

  Person() = default;

  Person(int dni, std::string name, std::string lastname, int age,
         std::string birth_date) {
    this->dni = dni;
    this->age = age;
    ::safeStrCopy(this->name, name, 20);
    ::safeStrCopy(this->lastname, lastname, 20);
    ::safeStrCopy(this->birth_date, birth_date, 10);
  }

  bool operator==(const Person &other) { return dni == other.dni; }

  friend std::ostream &operator<<(std::ostream &stream, Person &record) {
    stream.write((char *)&record.dni, sizeof(int));
    stream.write((char *)&record.name, 20);
    stream.write((char *)&record.lastname, 20);
    stream.write((char *)&record.age, sizeof(int));
    stream.write((char *)&record.birth_date, sizeof(int));

    stream << "\n";
    stream << std::flush;
    return stream;
  }

  friend std::istream &operator>>(std::istream &stream, Person &record) {
    stream.read((char *)&record.dni, sizeof(int));
    stream.read((char *)&record.name, 20);
    stream.read((char *)&record.lastname, 20);
    stream.read((char *)&record.age, sizeof(int));
    stream.read((char *)&record.birth_date, sizeof(int));
    stream.get();
    return stream;
  }
};

#endif
