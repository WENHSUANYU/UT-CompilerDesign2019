#include <iostream>
#include <cassert>
#include <fstream>
#include <string>

using std::cout;
using std::endl;
using std::string;
using std::ifstream;

string ReadFile(const string& filename) {
  ifstream fin(filename);
  string content;
  string line;
  while (std::getline(fin, line)) {
    content += line + '\n';
  }
  fin.close();
  return content;
}

void TestOutput(const string& filename, const string& result) {
  // Run the scanner executable
  system(("./scanner " + filename + " 2>&1 >/dev/null").c_str());
  
  // Result is written in output.txt, compare it with the `result` argument.
  assert(ReadFile("output.txt") == result);
}


int main() {
  // Test scan_sc()
  cout << "Testing scan_sc()" << endl;
  string sc_result = "1\tSC\t// A single line comment\n2\tSC\t//a Single line comment\n";
  TestOutput("test/sc.c", sc_result);

  // Test scan_mc()
  cout << "Testing scan_mc()" << endl;
  string mc_result = "1-3\tMC\n4\tMC\n5-6\tMC\tERROR: missing */\n";
  TestOutput("test/mc.c", mc_result);

  cout << "All test has passed!" << endl;
}
