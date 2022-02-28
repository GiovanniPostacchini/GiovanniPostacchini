#include "read_csv.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>   // std::stringstream
#include <stdexcept> // std::runtime_error
#include <string>
#include <vector>

std::string round_double(double d, int const precision) {
  std::ostringstream streamObj3;
  // Set Fixed -Point Notation
  streamObj3 << std::fixed;
  streamObj3 << std::setprecision(precision);
  // Add double to stream
  streamObj3 << d;
  // Get string from output string stream
  std::string strObj3 = streamObj3.str();
  return strObj3;
}

int significant_pos(double const d) {
  int n = 0;
  double d_copy = d;
  if (d > 0 && d < 1) {
    while (d_copy < 2.) {
      d_copy = d_copy * 10;
      ++n;
    }
  } else if (d >= 9.5) {
    while (d_copy >= 2.) {
      d_copy = d_copy / 10;
      --n;
    }
  }
  return n;
}

// vector measures = {values, errors, Fscal (if true)}
std::string file_tab(std::vector<std::vector<double>> measures,
                     double f_scal = true) {
  std::string file_tab{};
  // assuming vector measures arranged as v_x + v_ex + v_y + v_ey + ...
  int columns_data = measures.size();
  int lines_data = measures[0].size();
  int n_increm = 3;
  if (!f_scal) {
    n_increm = 2;
  }

  for (int i = 0; i < lines_data; ++i) {
    std::string line{};
    for (int n = 0; n < columns_data; n += n_increm) {
      if (n >= columns_data) {
        break;
      }
      int n_pos = significant_pos(measures[n + 1][i]);
      if (n_pos >= 0) {
        line += "$ " + round_double(measures[n][i], n_pos) + " \\pm " +
                round_double(measures[n + 1][i], n_pos) + " $\t&";
      } else {
        n_pos = std::abs(n_pos);
        double val = measures[n][i] / std::pow(10, n_pos);
        double err = measures[n + 1][i] / std::pow(10, n_pos);
        line += "$( " + round_double(val, significant_pos(err)) + " \\pm " +
                round_double(err, significant_pos(err)) + " ) ";
        if (n_pos == 1) {
          line += "10$\t&";
        } else {
          line += "10^{" + std::to_string(n_pos) + "}$\t&";
        }
      }
      if (f_scal) {
        line += round_double(measures[n + 2][i], 2) + "\t&";
      }
    }
    line.erase(line.end() - 1, line.end());
    line += "\\\\ \n";
    file_tab += line;
  }

  return file_tab;
}

void save_string(std::string const str, std::string const filename) {
  std::ofstream os(filename);
  if (!os) {
    std::string message = "Error writing to file " + filename + '\n';
    throw std::runtime_error(message);
  } else {
    os << str;
  }
}

int main() {
  std::string file = "./dati_csv/IV200.csv";
  std::vector<std::vector<double>> tab = {
      read_csv<double>(file, "V"),      read_csv<double>(file, "V_err"),
      read_csv<double>(file, "FscalV"), read_csv<double>(file, "I"),
      read_csv<double>(file, "I_err"),  read_csv<double>(file, "FscalI")};
  std::string const &str = file_tab(tab, true);
  // std::cout << str;
  save_string(str, "./IV200_Latex.txt");
}