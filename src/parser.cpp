#include "parser.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

static void stripCr(std::string& s) {
  if (!s.empty() && s.back() == '\r') s.pop_back();
}

static bool parseUint(const std::string& s, int& out) {
  if (s.empty()) return false;
  long long val = 0;
  for (char c : s) {
    if (c < '0' || c > '9') return false;
    val = (val * 10) + (c - '0');
    if (val > 1000000) return false;
  }
  out = static_cast<int>(val);
  return true;
}

static std::vector<std::string> tokenize(const std::string& s) {
  std::vector<std::string> out;
  std::istringstream iss(s);
  std::string tok;
  while (iss >> tok) out.push_back(tok);
  return out;
}

static bool parseIntList(const std::string& s, std::vector<int>& out,
                         int maxVal) {
  if (s.empty()) return false;
  int cur = 0;
  bool hasDigit = false;
  for (char c : s) {
    if (c >= '0' && c <= '9') {
      cur = (cur * 10) + (c - '0');
      if (cur > maxVal) return false;
      hasDigit = true;
    } else if (c == ',') {
      if (!hasDigit) return false;
      out.push_back(cur);
      cur = 0;
      hasDigit = false;
    } else {
      return false;
    }
  }
  if (!hasDigit) return false;
  out.push_back(cur);
  return true;
}

ParseResult parseInput(const std::string& filename, Dungeon& d) {
  std::ifstream in(filename);
  if (!in.is_open()) return {false, ""};

  std::string line;

  // N
  if (!std::getline(in, line)) return {false, ""};
  stripCr(line);
  int N = 0;
  if (!parseUint(line, N) || N < 1 || N > 255) return {false, line};
  d.N = N;
  d.rooms.assign(N + 1, Room{});
  std::vector<bool> defined(N + 1, false);
  std::vector<std::string> roomLines(N + 1);

  // N+1 room lines
  for (int i = 0; i <= N; i++) {
    if (!std::getline(in, line)) return {false, ""};
    stripCr(line);
    auto tokens = tokenize(line);
    if (tokens.size() != 2 && tokens.size() != 6) return {false, line};

    int id = 0;
    if (!parseUint(tokens[0], id) || id < 0 || id > N) return {false, line};
    if (defined[id]) return {false, line};

    Room r;
    if (!parseIntList(tokens[1], r.adjacent_rooms, N)) return {false, line};
    for (int a : r.adjacent_rooms) {
      if (a < 0 || a > N || a == id) return {false, line};
    }
    std::sort(r.adjacent_rooms.begin(), r.adjacent_rooms.end());
    r.adjacent_rooms.erase(
        std::unique(r.adjacent_rooms.begin(), r.adjacent_rooms.end()),
        r.adjacent_rooms.end());

    if (tokens.size() == 6) {
      for (int k = 0; k < 4; k++) {
        int rv = 0;
        if (!parseUint(tokens[2 + k], rv) || rv < 0 || rv > 255) {
          return {false, line};
        }
        r.resources[k] = rv;
      }
    } else {
      if (id != 0) return {false, line};
    }

    d.rooms[id] = r;
    defined[id] = true;
    roomLines[id] = line;
  }

  for (int i = 0; i <= N; i++) {
    if (!defined[i]) return {false, ""};
  }

  // Adjacency must be symmetric
  for (int i = 0; i <= N; i++) {
    for (int j : d.rooms[i].adjacent_rooms) {
      const auto& adj = d.rooms[j].adjacent_rooms;
      if (!std::binary_search(adj.begin(), adj.end(), i)) {
        return {false, roomLines[i]};
      }
    }
  }

  // food + target
  if (!std::getline(in, line)) return {false, ""};
  stripCr(line);
  auto tokens = tokenize(line);
  if (tokens.size() != 2) return {false, line};
  int M = 0;
  if (!parseUint(tokens[0], M) || M < 2 || M > 255) return {false, line};
  int t = -1;
  for (int k = 0; k < 4; k++) {
    if (tokens[1] == RES_NAMES[k]) {
      t = k;
      break;
    }
  }
  if (t < 0) return {false, line};
  d.food = M;
  d.target = t;

  return {true, ""};
}