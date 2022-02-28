#include <cmath> // std::fabs
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>   // std::stringstream
#include <stdexcept> // std::runtime_error
#include <string>
#include <utility> // std::pair
#include <vector>

std::string handle_string(std::string in, std::vector<std::string> vec) {
  return in;
}
float handle_string(std::string in, std::vector<float> vec) {
  return std::stof(in);
}
int handle_string(std::string in, std::vector<int> vec) {
  return std::stoi(in);
}
double handle_string(std::string in, std::vector<double> vec) {
  return std::stod(in);
}

template <class TP>
std::vector<TP> read_csv(std::string &filename, std::string const colname,
                         char const sep = ';') {
  std::vector<TP> result;
  // Create an input filestream
  std::ifstream myFile(filename);
  // Make sure the file is open
  if (!myFile.is_open()) {
    throw std::runtime_error("Could not open file");
  }
  std::string line;
  std::string val;
  int index_column = -1;
  // Read the column names
  if (myFile.good()) {
    // Extract the first line in the file
    std::getline(myFile, line, '\n');
    // add sep character in the last column
    line.erase(line.find("\r"));
    line.append(1, sep);
    line.append(1, '\r');
    std::cout << line << '\n';
    // Create a stringstream from line
    std::stringstream ss(line);
    int colindex = 0;
    std::string title;
    while (std::getline(ss, title, sep)) {
      if (title == colname) {
        index_column = colindex;
        break;
      }
      ++colindex;
    }
    if (index_column == -1) {
      std::string message = "Could not find column " + colname;
      throw std::runtime_error(message);
    }
  }
  // Read data, line by line
  while (std::getline(myFile, line)) {
    // add sep character in the last column
    if (line.find("\r") != std::string::npos) {
      line.erase(line.find("\r"));
      line.append(1, sep);
      line.append(1, '\r');
    }
    std::cout << line << '\n';
    // Create a stringstream of the current line
    std::stringstream ss(line);
    int colIxd = 0;
    std::string val;
    while (std::getline(ss, val, sep)) {
      if (colIxd == index_column) {
        result.push_back(handle_string(val, result));
      }
      val = "";
      ++colIxd;
    }
  }
  // Close file
  myFile.close();
  return result;
}

/*
int main() {
  std::string file = "./dati_csv/germanio.csv";
  std::vector<std::vector<double>> tab = {
      read_csv<double>(file, "V"), read_csv<double>(file, "V_err"),
      read_csv<double>(file, "I"), read_csv<double>(file, "I_err")};
  std::string const &str = file_tab(tab, false);
  // std::cout << str;
  save_string(str, "./germanio_Latex.txt");
}
*/