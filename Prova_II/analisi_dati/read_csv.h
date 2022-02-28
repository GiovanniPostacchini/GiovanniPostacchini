#ifndef READ_CSV
#define READ_CSV
#include "read_csv.C"
std::string handle_string(std::string in, std::vector<std::string> vec);
float handle_string(std::string in, std::vector<float> vec);
int handle_string(std::string in, std::vector<int> vec);
double handle_string(std::string in, std::vector<double> vec);

template <class TP>
std::vector<TP> read_csv(std::string &filename, std::string const colname,
                         char const sep);
#endif