# 🗺️ RegionGraphBuilder

[![C++](https://img.shields.io/badge/C%2B%2B-90.1%25-00599C?logo=c%2B%2B&logoColor=white)](https://github.com/xirzo/RegionGraphBuilder)
[![CMake](https://img.shields.io/badge/CMake-9.9%25-064F8C?logo=cmake&logoColor=white)](https://github.com/xirzo/RegionGraphBuilder)

## 📝 Description

RegionGraphBuilder is a C++ application that creates geographical region graphs. It generates visual representations of regions (like Europe) where countries are vertices and shared borders are edges.

## 🚀 Key Features

- 🌍 Builds geographical region graphs (e.g., Europe, Asia)
- 🔄 Automatic fetching and caching of geographical data
- 📊 Comprehensive graph metrics analysis including:
  - Radius and diameter calculations
  - Center node identification
  - Degree analysis (max/min)
  - Component analysis
  - Chromatic number calculation
  - Clique detection
  - Distance between capitals
- 🎨 SVG graph visualization using OGDF library
- 💾 JSON-based data caching for efficient subsequent runs

## ⚙️ Prerequisites

- Modern C++ compiler with C++23 support
- CMake 3.30.5 or higher
- Dependencies (automatically handled by CMake):
  - [OGDF](https://github.com/ogdf/ogdf) (Open Graph Drawing Framework)
  - [nlohmann/json](https://github.com/nlohmann/json)
  - [libcpr](https://github.com/libcpr/cpr)

## 🛠️ Installation

```bash
# Clone the repository
git clone https://github.com/xirzo/RegionGraphBuilder.git

# Navigate to the project directory
cd RegionGraphBuilder

# Create a build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .
```

## 💡 Usage

```bash
# Set your API key for the geodata source
export geo_data_api_key="your_api_key_here"

# Run with default region (europe)
./region_graph_builder

# Run with specific region
./region_graph_builder asia
```

The program will:

1. Fetch data
3. Build a graph representation
4. Generate an SVG visualization
5. Calculate and display graph metrics

## 📊 Output

- SVG visualization of the region graph
- Detailed metrics including:
  - Graph connectivity measures
  - Node degree statistics
  - Component analysis
  - Various graph properties (Eulerian/Hamiltonian characteristics)

## ⚠️ Important Notes

- Requires a valid API key for the geodata source
- API key must be set in the environment variable `geo_data_source_api_key`, obtain it from [here](https://www.geodatasource.com/license)
