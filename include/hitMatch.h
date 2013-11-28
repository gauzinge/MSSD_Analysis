#ifndef _HITMATCH_H_
#define _HITMATCH_H_

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

class Event {
private:
  std::vector<int> HitTreeRaw_ ;
public:
  Event() {}
  void addHit(const int& hitLine);
  const std::vector<int>& GetHitLines();
  std::string Serialize() const;
  unsigned int GetNHits() const;
};

class EventCollection {
private:
  std::map<int, Event> myCollection_;
public:
  void addHit(const int& hitLineNumber, const int& eventNumber);
  void Serialize(std::ostream&);
  void DeSerialize(std::ifstream& read);
  const std::vector<int>& GetHitLines(int eventNumber);
};

#endif
