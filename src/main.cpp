//
// Created by Dan Schnurpfeil on 21.09.2023.
//
#include "preprocessing/utils.h"
#include <thread>
#include <iostream>
#include "genetic_alg/Population.h"
#include <memory>

void print_time(auto start_time) {
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = static_cast<time_t>(std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() / 1000.0);
    auto duration_t = localtime(&duration);
    std::cout << "Time taken: " <<  duration_t->tm_min << " min " << duration_t->tm_sec << " sec" << std::endl  << std::endl;
}



void create_one_generation(std::unique_ptr<Population> &population, auto &data, int wave) {

    std::cout << "starting correlation compute... " << std::endl;

    std::vector<Equation> new_equations;
    std::vector<float> population_results;
    std::vector<float> equation_results;
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < population->populationSize; ++i) {
        try {
            equation_results = population->evaluate(data.first->x, data.first->y, data.first->z, i);
//          equation_results = population->evaluateCPU(data.first->x,data.first->y,data.first->z,i);

            new_equations.push_back(population->equations[i]);
            float corr = population->countFitFunction(equation_results);
            population_results.emplace_back(corr);
        }
        catch (std::invalid_argument &ex) {
            std::cerr << std::endl << ex.what() << std::endl;
            equation_results.clear();
            continue;
        }
        catch (std::exception & err)
        {
            std::cerr << std::endl << err.what() << std::endl;
            return;
        }
    }
    print_time(start_time);

    std::cout << "average fitness: " << mean(population_results) << std::endl;
    float mean_result = mean(population_results);

    for (int j = 0; j < new_equations.size(); ++j) {
        if (population_results[j] - population->fitness[j] <= (mean_result + mean_result / 45 * (1 + wave))) {
            new_equations[j].root = std::numeric_limits<float>::max();
            population_results[j] = std::numeric_limits<float>::max();
        }
    }
    std::erase_if(new_equations,[] (Equation& value) {return value.root == std::numeric_limits<float>::max();});
    std::erase_if(population_results,[] (float& value) {return value == std::numeric_limits<float>::max();});

    auto index_max = std::max_element(population_results.begin(), population_results.end());
    int index_m = std::distance(population_results.begin(), index_max);
    std::cout << "max fitness: " << population_results[index_m] << std::endl;
    std::cout << "best local equation: " << new_equations[index_m] << std::endl;

    std::cout << "parents population size: " << new_equations.size() << std::endl;
    std::cout << "average population fitness after selection: " << mean(population_results) << std::endl;
    start_time = std::chrono::high_resolution_clock::now();
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(new_equations.begin(), new_equations.end(), g);
    population->equations = new_equations;
    std::cout << "started crossbreeding... " << std::endl;

    std::vector<Equation> children = population->crossbreed();
    std::cout << "children: " << children.size() << std::endl;

    if (population->populationSize - population->equations.size() < children.size()) {
        for (int j = 0; j < population->populationSize - population->equations.size(); ++j) {
            population->equations.push_back(children[j]);
        }
    } else {
        for (auto &item: children) population->equations.push_back(item);
        for (int j = 0; j < population->populationSize - population->equations.size(); ++j) {
            population->equations.emplace_back();
        }
    }
    print_time(start_time);


}

int main(int argc, char **argv) {


//todo
// input data path
//  cleared acc or no
// GPU device to run
// GPU workgroup size
// precompile spirv  check
// refactor gpu computation unit
// equation xyz interval
// evaluate on cpu or on gpu



  const auto width = 90;
  const auto height = 60;
  const auto a = 2.0f; // saxpy scaling factor

  auto y = std::vector<float>(width * height, 0.71f);
  auto x = std::vector<float>(width * height, 0.65f);

  std::cout << "VULKAN correctness demo: result = 2 * 0.65 + 0.71 ...should be 2.01" << std::endl;

  ComputationUnit f("../saxpy.spv", true);
  auto d_y = vuh::Array<float>::fromHost(y, f.device, f.physDevice);
    auto d_x = vuh::Array<float>::fromHost(x, f.device, f.physDevice);

    f.compute(d_y, d_x, {width, height, a});

    auto out_tst = std::vector<float>{};
    d_y.to_host(out_tst); // and now out_tst should contain the result of saxpy (y = y + ax)
    std::cout << "result " << out_tst[0] << std::endl;

    try {
        std::cout << std::endl << "starting... " << std::endl;
        bool load_sequential = false;
        auto start_time = std::chrono::high_resolution_clock::now();
        std::string hr_filename = "../data/HR_007.csv";
        std::string acc_filename = "../data/ACC_007_cleared.csv";

        auto data = load_data(hr_filename, acc_filename, load_sequential);
        if (data.first->x.empty()) {
            return 1;
        }
        if (data.second->x.empty()) {
            return 1;
        }
        std::cout << "loaded hr " << data.second->x.size() << std::endl;
        std::cout << "loaded acc " << data.first->x.size() << std::endl;
        print_time(start_time);

        start_time = std::chrono::high_resolution_clock::now();
        //  performing some preprocessing steps on the data before it is used further
        preprocess(data, acc_filename);

        std::cout << "clean acc " << data.first->x.size() << std::endl;
        print_time(start_time);
        auto population = std::make_unique<Population>();

        population->prepareForFitFunction(data.second->x);

        for (int epoch = 0; epoch < 1000; ++epoch) {
            create_one_generation(population, data, epoch % 20);
            std::cout << "epoch " << epoch << " done... " << std::endl << "##########################" << std::endl;
        }
    }
        catch (std::exception & err)
        {
            std::cerr << std::endl << err.what() << std::endl;
            return 1;
        }
    std::cout << "done... " << std::endl;
    return 0;
}
