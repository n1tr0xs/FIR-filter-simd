#include <random>
#include <iostream>
#include <vector>

// Неоптимизированная реализация FIR фильтра
void fir_scalar(const std::vector<float>& input, const std::vector<float>& coeffs, std::vector<float>& output) {
	size_t filter_len = coeffs.size();       // Длина импульсной характеристики
	size_t output_len = output.size();       // Сколько точек нужно вычислить

	// Проверка: input должен быть достаточно длинным для фильтрации
	if (input.size() < output_len + filter_len - 1) {
		throw std::runtime_error("fir_scalar: input.size() < output.size() + coeffs.size() - 1");
	}

	// Проходим по каждому выходному элементу
	for (size_t i = 0; i < output_len; ++i) {
		float acc = 0.0f;

		// Фильтрация: сумма произведений входных данных и коэффициентов
		for (size_t j = 0; j < filter_len; ++j) {
			acc += input[i + j] * coeffs[j];
		}

		output[i] = acc;
	}
}

// SIMD-оптимизированная реализация FIR фильтра (SSE)
void fir_simd(const std::vector<float>& input,
	const std::vector<float>& coeffs,
	std::vector<float>& output) {
	size_t filter_len = coeffs.size();   // Длина фильтра (импульсная характеристика)
	size_t output_len = output.size();   // Сколько значений нужно записать

	// Проверка: input должен быть не короче, чем output + filter_len - 1
	if (input.size() < output_len + filter_len - 1) {
		throw std::runtime_error("fir_simd: input.size() < output.size() + coeffs.size() - 1");
	}

	size_t j_limit = filter_len & ~3U;  // filter_len округляется вниз до ближайшего кратного 4

	for (size_t i = 0; i < output_len; ++i) {
		__m128 acc = _mm_setzero_ps();
		size_t j = 0;

		// Основная часть: по 4 элемента с использованием SSE
		for (; j < j_limit; j += 4) {
			__m128 in_vec = _mm_loadu_ps(&input[i + j]);   // 4 входных значения
			__m128 coef_vec = _mm_loadu_ps(&coeffs[j]);    // 4 коэффициента фильтра
			acc = _mm_add_ps(acc, _mm_mul_ps(in_vec, coef_vec)); // acc += input * coeffs
		}

		// Суммируем значения в регистре acc
		float sum[4];
		_mm_storeu_ps(sum, acc);
		float total = sum[0] + sum[1] + sum[2] + sum[3];

		// Остаток (если filter_len не кратен 4)
		for (; j < filter_len; ++j) {
			total += input[i + j] * coeffs[j];
		}

		output[i] = total;
	}
}

void generate_data(std::vector<float>& data) {
	std::mt19937 rng(42);
	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
	for (auto& x : data) {
		x = dist(rng);
	}
}

// Измерение времени выполнения функции
template <typename Func>
double benchmark(Func f, const std::vector<float>& input, const std::vector<float>& coeffs, std::vector<float>& output) {
	auto start = std::chrono::high_resolution_clock::now();
	f(input, coeffs, output);
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> dur = end - start;
	return dur.count();
}

int main() {

	std::vector<int> filter_lengths = { 4, 8, 16, 32, 64, 128, 256, 512, 1024 };
	int input_size = 1 << 20;  // 1M samples
	int output_size = input_size;

	for (int filter_length : filter_lengths) {
		std::cout << "Filter length: " << filter_length << "\n";
		
		if (filter_length >= input_size) {
			std::cerr << "Skipping: filter length too long for input\n";
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
