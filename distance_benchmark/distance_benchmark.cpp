// distance_benchmark.cpp : Defines the entry point for the application.
//

#include <immintrin.h>
#include <xmmintrin.h>

#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"

#include <algorithm>
#include <chrono>
#include <random>
#include <vector>
#include <intrin.h>

struct point {
	float x;
	float y;
};

// c++ lib way
float distance(point a, point b)
{
	return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) * 1.0);
}

// faster than c++ lib way?
float fast_distance(point a, point b) {
	double c = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
	return sqrt(c);
}

// faster than c++ lib way?
float fast_distance3(point a, point b) {
  float c = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
  return std::sqrtf(c);
}

float fast_distance2(point a, point b) {
	//double c = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
	__m128 c = _mm_set1_ps((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
	c = _mm_rsqrt_ps(c);
	return _mm_extract_ps(c, 0);
}

float fast_distance_sse(point a, point b) {
  __m128 l = _mm_load_ss(&a.x);
  __m128 r = _mm_load_ss(&b.x);
  __m128 s = _mm_sub_ps(l, r);
  __m128 m = _mm_mul_ps(s, s);
  // need to add here, awkward to do in sse
  m.m128_f32[0] += m.m128_f32[1];
  __m128 ret = _mm_sqrt_ps(m);
  // just need to return the sqrt
  return ret.m128_f32[0];
}

// approximation
float doom_distance(point a, point b) {
	float dx = abs(b.x - a.x);
	float dy = abs(b.y - a.y);
	if (dx < dy)
		return dx + dy - (dx / 2);
	return dx + dy - (dy / 2);
}

float doom_distance2(point a, point b) {
	float dx = abs(b.x - a.x);
	float dy = abs(b.y - a.y);
	if (dx < dy)
		return dx + dy - (*reinterpret_cast<uint32_t*>(&dx) >> 1);
	return dx + dy - (*reinterpret_cast<uint32_t*>(&dy) >> 1);
}

typedef struct {
	uint64_t state;
	uint64_t inc;
} pcg32_random_t;

uint32_t pcg32_random_r(pcg32_random_t* rng) {
	uint64_t oldstate = rng->state;
	// Advance internal state
	rng->state = oldstate * 6364136223846793005ULL + (rng->inc | 1);
	// Calculate output function (XSH RR), uses old state for max ILP
	uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
	uint32_t rot = oldstate >> 59u;
	return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

int main()
{
	std::chrono::high_resolution_clock clk;

	pcg32_random_t rng;
	rng.inc = std::chrono::duration_cast<std::chrono::nanoseconds>(
		clk.now().time_since_epoch())
		.count();

	ankerl::nanobench::Bench benchmark;
	// benchmark.epochs(1024);
	// benchmark.minEpochIterations(128);
	// benchmark.minEpochTime(std::chrono::nanoseconds{4000});
	benchmark.warmup(4);
	// for metrics
	// benchmark.unit("sorts");
	benchmark.performanceCounters(true);
	benchmark.relative(true);

	size_t bench_size = 2048;
	size_t target_size = bench_size*256;

	std::vector<point> data;
	std::vector<float> save_data;

	data.reserve(target_size);
	save_data.reserve(target_size);

	rng.state = ~(std::chrono::duration_cast<std::chrono::nanoseconds>(
		clk.now().time_since_epoch())
		.count());

	std::string title = std::string{ "________________________________" };

	while (bench_size < target_size) {
		benchmark.batch(bench_size);
		while (data.size() < bench_size) {
			data.emplace_back();
		}
		while (save_data.size() < bench_size) {
			save_data.emplace_back();
		}
		for (size_t i = 0; i < data.size(); i++) {
			data[i].x = (float)pcg32_random_r(&rng);
			data[i].y = (float)pcg32_random_r(&rng);
		}

		point target;
		target.x = (float)pcg32_random_r(&rng);
		target.y = (float)pcg32_random_r(&rng);

		std::string bench_num = std::to_string(bench_size);

		title = std::string{ "distance (" } + bench_num + ")";
		benchmark.run(title, [&]() {
			for (size_t i = 0; i < data.size(); i++) {
				save_data[i] = distance(data[i], target);
			}
		});

		title = std::string{ "fast distance (" } + bench_num + ")";
		benchmark.run(title, [&]() {
			for (size_t i = 0; i < data.size(); i++) {
				save_data[i] = fast_distance(data[i], target);
			}
			});

		title = std::string{"fast distance cast ("} + bench_num + ")";
        benchmark.run(title, [&]() {
                for (size_t i = 0; i < data.size(); i++) {
                save_data[i] = fast_distance3(data[i], target);
                }
            });

		title = std::string{ "fast distance sse (" } + bench_num + ")";
		benchmark.run(title, [&]() {
			for (size_t i = 0; i < data.size(); i++) {
				save_data[i] = fast_distance2(data[i], target);
			}
			});

		title = std::string{"fast distance sse 2("} + bench_num + ")";
        benchmark.run(title, [&]() {
            for (size_t i = 0; i < data.size(); i++) {
            save_data[i] = fast_distance_sse(data[i], target);
            }
        });

		title = std::string{ "doom distance (" } + bench_num + ")";
		benchmark.run(title, [&]() {
			for (size_t i = 0; i < data.size(); i++) {
				save_data[i] = doom_distance(data[i], target);
			}
			});

		title = std::string{ "doom distance 2 (" } + bench_num + ")";
		benchmark.run(title, [&]() {
			for (size_t i = 0; i < data.size(); i++) {
				save_data[i] = doom_distance2(data[i], target);
			}
			});

		bench_size *= 2;
	}

	return 0;
}
