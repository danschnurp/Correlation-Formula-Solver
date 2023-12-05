#include <iostream>
#include <fstream>
#include "svg_gen.h"

void make_svg(std::shared_ptr<Equation> &best_one,
              std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data, std::string name) {
  // Create an output file stream
  std::ofstream svgFile("correlation_plot_" + name + ".svg");

  // Check if the file opened successfully
  if (!svgFile.is_open()) {
    std::cerr << "Error: Unable to open the SVG file." << std::endl;
    return;
  }

  // Define the width and height of the SVG canvas
  int width = 800;
  int height = 300;

  // Generate the SVG code
  svgFile << R"(<?xml version="1.0" encoding="UTF-8" standalone="no"?>)" << std::endl;
  svgFile << "<svg width=\"" << width << "\" height=\"" << height << R"(" xmlns="http://www.w3.org/2000/svg">)"
          << std::endl;

  // Create a white background rectangle
  svgFile << R"(  <rect x="0" y="0" width=")" << width << "\" height=\"" << height << R"(" fill="white" />)"
          << std::endl;

  // Create X-axis
  svgFile << R"(  <line x1="50" y1="250" x2="750" y2="250" stroke="black" stroke-width="2" />)" << std::endl;

  // Create Y-axis
  svgFile << R"(  <line x1="50" y1="250" x2="50" y2="50" stroke="black" stroke-width="2" />)" << std::endl;

  // X-axis label
  svgFile << R"(  <text x="575" y="280" font-size="12" fill="black">Time (seconds from start)</text>)" << std::endl;

  // Y-axis label
  svgFile << R"lit(  <text x="0" y="15" font-size="12" fill="black" transform="rotate(270, 25, 25)">Values</text>)lit"
          << std::endl;

  std::vector<float> norm;
  for (int i = 0; i < 700; i++) {
    norm.emplace_back(best_one->evaluate(data.first->x[i], data.first->y[i], data.first->z[i]));
  }
  normalize(norm);

  // Create the scaled polyline for ACC
  svgFile << "  <polyline points=\"";
  for (int i = 0; i < 700; i++) {
    svgFile << 50 + i << "," << 250 - norm[i] * 150 << " ";
  }
  svgFile << R"(" fill="none" stroke="green" stroke-width="2" />)" << std::endl;

  // Create the scaled polyline for HR
  svgFile << "  <polyline points=\"";
  for (int i = 0; i < 700; i++) {
    svgFile << 50 + i << "," << 250 - data.second->x[i] * 250 << " ";
  }
  svgFile << R"(" fill="none" stroke="blue" stroke-width="2" />)" << std::endl;
  // Title
  svgFile << R"(  <text x="450" y="30" font-size="19" class="speed-description" fill="black" text-anchor="middle">Correlation Between Heart rate (blue) and Accelerometer (green)
                         )" << "</text>" << std::endl;
  svgFile << R"(  <text x="450" y="50" class="speed-description" font-size="11" fill="gray" text-anchor="middle">)"
          << *best_one << "</text>" << std::endl;

  // X-axis captions
  for (int i = 0; i < 14; i++) {
    svgFile << "  <text x=\"" << 50 + i * 50 << R"(" y="265" font-size="12" fill="black">)" << i * 50 << "</text>"
            << std::endl;
  }

  // Y-axis values and captions
  for (int i = 0; i < 7; i++) {
    svgFile << R"(  <text x="15" y=")" << 50 + i * 30 << R"(" font-size="12" fill="black">)" << 1 - i * 0.14
            << "</text>" << std::endl;
  }

  // Close the SVG file
  svgFile << "</svg>" << std::endl;

    // Close the file stream
    svgFile.close();

  std::cout << "SVG plot created successfully in correlation_plot_" + name + ".svg" << std::endl;
}
