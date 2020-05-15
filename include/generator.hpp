#include <iostream>
#include <fstream>
#include <random>
#include <set>

class RegisterGenerator {
	private:
		std::string outfilePath;
		std::ifstream namefile;
		std::ifstream lastnamefile;
		std::random_device rd;

		const size_t nNames = 5494;
		const size_t nLastnames = 20000;

		size_t rand(int min, int max) {
			return min + rd() % (max - min + 1);
		}

		std::string randomName() {
			int idx = rand(0, nNames - 2);
			namefile.seekg(16 * idx, std::ios_base::beg);

			std::string randomName;
			namefile >> randomName;

			return randomName;
		}
		
		std::string randomLastname() {
			int idx = rand(0, nLastnames - 2);
			lastnamefile.seekg(16 * idx, std::ios_base::beg);

			std::string randomLastname;
			namefile >> randomLastname;

			return randomLastname;
		}

		int randomAge() {
			return rand(15, 90);
		}

		int randomId() {
			return rand(100000000, 999999999);
		}

		std::string randomBirthDate(int age) {
			std::string d = std::to_string(rand(1, 28));
			std::string m = std::to_string(rand(1, 12));
			std::string y = std::to_string(2020 - age);
			return d + '/' + m + '/' + y;
		}

	public:
		RegisterGenerator(std::string namePath, std::string lastnamePath, std::string outfilePath): 
			namefile(namePath), lastnamefile(lastnamePath), outfilePath(outfilePath) {}

		void operator ()(size_t size) {
			std::ofstream outfile(outfilePath);

			int id;
			std::set<int> seenIds;
			for (size_t i = 0; i < size; i++) {
				do {
					id = randomId();
				} while(seenIds.count(id));
				seenIds.insert(id);

				outfile << id << ' ';
				outfile << randomName() << ' ';
				outfile << randomLastname() << ' ';

				int age = randomAge();
				outfile << age << ' ';
				outfile << randomBirthDate(age) << std::endl;
			}

			outfile.close();
		}
};
