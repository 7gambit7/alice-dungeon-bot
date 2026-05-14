// Build: g++ -std=c++17 -O2 src/*.cpp -o task
// Run: ./task in.txt (writes result.txt)

#include <fstream>

#include "alice_bot.h"
#include "parser.h"

int main(int argc, char** argv) {
  std::ofstream out("result.txt");
  if (argc < 2) return 0;

  Dungeon dungeon;
  ParseResult pr = parseInput(argv[1], dungeon);
  if (!pr.ok) {
    out << pr.badLine << "\n";
    return 0;
  }

  AliceBot bot;
  bot.run(dungeon, out);
  return 0;
}