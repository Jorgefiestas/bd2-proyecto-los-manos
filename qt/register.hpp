#ifndef REGISTER_HPP 
#define REGISTER_HPP

#include <algorithm>

void safeStrCopy(char* to, std::string from, size_t n) {
	for (int i = 0; i < std::min(n, from.length()); i++) {
		to[i] = from[i];
	}
}

struct Register {
	int id;
	char name[20];
	char lastname[20];
	int age;
	char birthDate[20];

	Register(int id, std::string name, std::string lastname, int age, std::string birthDate): 
		id(id), age(age) {
                        ::safeStrCopy(this->name, name, 20);
                        ::safeStrCopy(this->lastname, lastname, 20);
		}
};

#endif
