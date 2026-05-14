#include "alice_bot.h"

#include <array>
#include <climits>
#include <iostream>
#include <queue>
#include <vector>

void AliceBot::run(Dungeon dungeon, std::ostream& out) {
  const int target = dungeon.target;
  const int M = dungeon.food;
  const int total = dungeon.N + 1;
  auto& rooms = dungeon.rooms;

  std::vector<bool> visited(total, false);
  std::vector<bool> firstCollectDone(total, false);
  std::vector<std::array<bool, 4>> collectedFlag(total);
  for (auto& cf : collectedFlag) cf = {false, false, false, false};
  int gathered[4] = {0, 0, 0, 0};

  int current = 0;
  int spent = 0;
  visited[0] = true;

  auto printState = [&](int roomId) {
    out << "state " << roomId;
    for (int k = 0; k < 4; k++) {
      out << " ";
      if (collectedFlag[roomId][k])
        out << "_";
      else
        out << rooms[roomId].resources[k];
    }
    out << "\n";
  };

  auto pickHighestPresent = [&](int roomId) -> int {
    int bestRes = -1, bestVal = -1;
    for (int k = 0; k < 4; k++) {
      if (rooms[roomId].resources[k] > 0) {
        int v = resourceValue(k, target);
        if (v > bestVal) {
          bestVal = v;
          bestRes = k;
        }
      }
    }
    return bestRes;
  };

  auto doCollect = [&](int roomId, int resIdx) {
    int cost = firstCollectDone[roomId] ? 1 : 0;
    spent += cost;
    firstCollectDone[roomId] = true;
    collectedFlag[roomId][resIdx] = true;
    gathered[resIdx] += rooms[roomId].resources[resIdx];
    rooms[roomId].resources[resIdx] = 0;
    out << "collect " << RES_NAMES[resIdx] << "\n";
    printState(roomId);
  };

  // Exploration phase
  const int budget = M / 2;

  while (true) {
    // Collect once on first arrival to a room (free).
    if (!firstCollectDone[current]) {
      int bestRes = pickHighestPresent(current);
      if (bestRes >= 0) {
        doCollect(current, bestRes);
      }
    }

    if (spent >= budget) break;

    int next = chooseNextExplore(current, visited, rooms);
    if (next < 0) break;

    spent++;
    current = next;
    visited[current] = true;
    out << "go " << current << "\n";
    printState(current);
  }

  // Return phase
  while (current != 0) {
    // BFS through visited rooms to get distance-to-start for every visited
    // room.
    std::vector<int> dist(total, -1);
    dist[0] = 0;
    std::queue<int> q;
    q.push(0);
    while (!q.empty()) {
      int u = q.front();
      q.pop();
      for (int v : rooms[u].adjacent_rooms) {
        if (!visited[v] || dist[v] != -1) continue;
        dist[v] = dist[u] + 1;
        q.push(v);
      }
    }

    int foodNeeded = dist[current];
    int foodLeft = M - spent;
    int excess = foodLeft - foodNeeded;

    // Сollecting remaining resources by descending value.
    while (excess > 0) {
      int bestRes = pickHighestPresent(current);
      if (bestRes < 0) break;
      int cost = firstCollectDone[current] ? 1 : 0;
      if (cost > excess) break;
      doCollect(current, bestRes);
      foodLeft = M - spent;
      excess = foodLeft - foodNeeded;
    }

    // Steping toward 0 along a shortest path. At a fork, picking
    // smallest-numbered neighbor.
    int nextStep = -1;
    for (int adj : rooms[current].adjacent_rooms) {
      if (!visited[adj]) continue;
      if (dist[adj] == dist[current] - 1) {
        nextStep = adj;
        break;
      }
    }
    if (nextStep < 0) break;

    spent++;
    current = nextStep;
    out << "go " << current << "\n";
    if (current != 0) printState(current);
  }

  long long totalValue = 0;
  for (int k = 0; k < 4; k++) {
    totalValue += (long long)gathered[k] * resourceValue(k, target);
  }
  out << "result";
  for (int k = 0; k < 4; k++) out << " " << gathered[k];
  out << " " << totalValue << "\n";
}

int AliceBot::chooseNextExplore(int current, const std::vector<bool>& visited,
                                const std::vector<Room>& rooms) {
  for (int adj : rooms[current].adjacent_rooms) {
    if (!visited[adj]) return adj;
  }

  const int N = (int)rooms.size();

  // BFS from current through visited rooms (recording distance to every
  // reachable room) P.S. visited rooms transit, unvisited rooms are reached but
  // not expanded
  std::vector<int> dist(N, -1);
  dist[current] = 0;
  std::queue<int> q;
  q.push(current);
  while (!q.empty()) {
    int u = q.front();
    q.pop();
    for (int v : rooms[u].adjacent_rooms) {
      if (dist[v] != -1) continue;
      dist[v] = dist[u] + 1;
      if (visited[v]) q.push(v);
    }
  }

  int targetRoom = -1, targetDist = INT_MAX;
  for (int v = 0; v < N; v++) {
    if (visited[v] || dist[v] == -1) continue;
    if (dist[v] < targetDist ||
        (dist[v] == targetDist && (targetRoom == -1 || v < targetRoom))) {
      targetRoom = v;
      targetDist = dist[v];
    }
  }
  if (targetRoom < 0) return -1;

  // Distances from the target through visited rooms
  // they are needed to identify which adjacent visited room is on a shortest
  // path
  std::vector<int> distFromTarget(N, -1);
  distFromTarget[targetRoom] = 0;
  std::queue<int> q2;
  q2.push(targetRoom);
  while (!q2.empty()) {
    int u = q2.front();
    q2.pop();
    for (int v : rooms[u].adjacent_rooms) {
      if (distFromTarget[v] != -1) continue;
      if (v != targetRoom && !visited[v]) continue;
      distFromTarget[v] = distFromTarget[u] + 1;
      q2.push(v);
    }
  }

  for (int adj : rooms[current].adjacent_rooms) {
    if (visited[adj] && distFromTarget[adj] == targetDist - 1) {
      return adj;
    }
  }
  return -1;
}