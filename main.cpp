#include <random>
#include <iostream>
#include <vector>

void generate_data(std::vector<float>& data) {
	std::mt19937 rng(42);
	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
	for (auto& x : data) {
		x = dist(rng);
	}
}

int main() {

	std::vector<int> filter_lengths = { 4, 8, 16, 32, 64, 128, 256, 512, 1024 };
	int input_size = 1 << 20;  // 1M samples
	int output_size = input_size;

	for (int filter_length : filter_lengths) {
		if (filter_length >= input_size) {
			std::cerr << "Skipping filter length " << filter_length << " (too long for input)\n";
			continue;
		}
		
		std::vector<float> input(input_size + filter_length);
		std::vector<float> coeffs(filter_length);
		std::vector<float> out_scalar(output_size);
		std::vector<float> out_simd(output_size);

		generate_data(input);
		std::cout << "Input data generated.\n";
		generate_data(coeffs);
		std::cout << "Coeffs data generated.\n";
	}

	return 0;
}
