#ifndef VISUAL_H
#define VISUAL_H

#include <string>
#include <unordered_map>

#include "country.h"

struct edge_pair {
    std::string country1;
    std::string country2;

    edge_pair(const std::string& c1, const std::string& c2);
    bool operator<(const edge_pair& other) const;
};

void export_graph(std::unordered_map<std::string, country>& countries,
                  const std::string& filename);

#endif  // VISUAL_H
