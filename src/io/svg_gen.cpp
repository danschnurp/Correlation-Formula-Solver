#include <iostream>
#include <fstream>
#include "svg_gen.h"

void make_svg(std::shared_ptr<Equation> &best_one,
              std::pair<std::shared_ptr<RecordACC>, std::shared_ptr<RecordHR>> &data) {
  // Create an output file stream
  std::ofstream svgFile("plot.svg");

  // Check if the file opened successfully
  if (!svgFile.is_open()) {
    std::cerr << "Error: Unable to open the SVG file." << std::endl;
    return;
  }

  // Define the width and height of the SVG canvas
    int width = 450;
    int height = 300;

    // Generate the SVG code
    svgFile << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>" << std::endl;
    svgFile << "<svg width=\"" << width << "\" height=\"" << height << "\" xmlns=\"http://www.w3.org/2000/svg\">" << std::endl;

    // Create a white background rectangle
    svgFile << "  <rect x=\"0\" y=\"0\" width=\"" << width << "\" height=\"" << height << "\" fill=\"white\" />" << std::endl;

    // Create X-axis
    svgFile << "  <line x1=\"50\" y1=\"250\" x2=\"350\" y2=\"250\" stroke=\"black\" stroke-width=\"2\" />" << std::endl;

    // Create Y-axis
    svgFile << "  <line x1=\"50\" y1=\"250\" x2=\"50\" y2=\"50\" stroke=\"black\" stroke-width=\"2\" />" << std::endl;

    // X-axis label
    svgFile << "  <text x=\"375\" y=\"275\" font-size=\"16\" fill=\"black\">X-axis</text>" << std::endl;

    // Y-axis label
    svgFile << "  <text x=\"0\" y=\"15\" font-size=\"16\" fill=\"black\" transform=\"rotate(270, 25, 25)\">Y-axis</text>" << std::endl;

    // Define example data points
    int dataPoints[] = {10, 10, 10, 180, 160, 140, 120};

    // Create the polyline
    svgFile << "  <polyline points=\"";
    for (int i = 0; i < 7; i++) {
        svgFile << 50 + dataPoints[i] << "," << 250 - dataPoints[i] << " ";
    }
    svgFile << "\" fill=\"none\" stroke=\"black\" stroke-width=\"2\" />" << std::endl;

    // Title
    svgFile << "  <text x=\"200\" y=\"30\" font-size=\"20\" fill=\"black\" text-anchor=\"middle\">SVG Polyline Plot</text>" << std::endl;

    // X-axis captions
    for (int i = 0; i < 7; i++) {
        svgFile << "  <text x=\"" << 50 + i * 50 << "\" y=\"265\" font-size=\"12\" fill=\"black\">" << i << "</text>" << std::endl;
    }

    // Y-axis values and captions
    for (int i = 0; i < 7; i++) {
        svgFile << "  <text x=\"25\" y=\"" << 50 + i * 30 << "\" font-size=\"12\" fill=\"black\">" << 30 * i << "</text>" << std::endl;
    }

    // Close the SVG file
    svgFile << "</svg>" << std::endl;

    // Close the file stream
    svgFile.close();

    std::cout << "SVG plot with white background and black lines created successfully in 'plot.svg'." << std::endl;
}
