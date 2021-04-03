# distance_benchmark
Quick benchmarking of some distance functions

Some quick results, it appears that with minimal effort the optimizer takes
```c
float fast_distance(point a, point b) {
  float c = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
  return std::sqrtf(c);
}
```
and converts it to a very high performance function with sse instructions, probably a decent choice all considered, and may behave even better in loops and other optimizable locations.

```
| relative |               ns/op |                op/s |    err% |     total | benchmark
|---------:|--------------------:|--------------------:|--------:|----------:|:----------
|   100.0% |               78.91 |       12,673,267.33 |    0.1% |      0.00 | `distance (2048)`
| 1,767.0% |                4.47 |      223,936,381.71 |    8.4% |      0.00 | :wavy_dash: `fast distance (2048)` (Unstable with ~11.6 iters. Increase `minEpochIterations` to e.g. 116)
| 2,978.1% |                2.65 |      377,419,980.60 |    0.0% |      0.00 | `fast distance cast (2048)`
| 2,775.6% |                2.84 |      351,755,725.19 |    0.1% |      0.00 | `fast distance sse (2048)`
| 2,980.1% |                2.65 |      377,669,740.15 |    0.0% |      0.00 | `fast distance sse 2(2048)`
| 1,215.0% |                6.49 |      153,984,962.41 |    1.0% |      0.00 | `doom distance (2048)`
| 1,070.2% |                7.37 |      135,629,139.07 |    1.6% |      0.00 | `doom distance 2 (2048)`
|    45.7% |               86.28 |       11,590,265.99 |    7.1% |      0.00 | :wavy_dash: `distance (4096)` (Unstable with ~1.0 iters. Increase `minEpochIterations` to e.g. 10)
|   958.6% |                4.12 |      242,983,050.85 |    0.1% |      0.00 | `fast distance (4096)`
| 1,490.8% |                2.65 |      377,859,778.60 |    0.0% |      0.00 | `fast distance cast (4096)`
| 1,387.8% |                2.84 |      351,755,725.19 |    0.0% |      0.00 | `fast distance sse (4096)`
| 1,490.8% |                2.65 |      377,859,778.60 |    0.1% |      0.00 | `fast distance sse 2(4096)`
|   515.7% |                7.65 |      130,723,404.26 |    0.0% |      0.00 | `doom distance (4096)`
|   444.8% |                8.87 |      112,733,944.95 |    0.6% |      0.00 | `doom distance 2 (4096)`
|    23.6% |               83.57 |       11,966,111.60 |    8.5% |      0.01 | :wavy_dash: `distance (8192)` (Unstable with ~1.0 iters. Increase `minEpochIterations` to e.g. 10)
|   479.5% |                4.11 |      243,086,053.41 |    0.0% |      0.00 | `fast distance (8192)`
|   745.4% |                2.65 |      377,859,778.60 |    0.0% |      0.00 | `fast distance cast (8192)`
|   694.2% |                2.84 |      351,890,034.36 |    0.1% |      0.00 | `fast distance sse (8192)`
|   616.8% |                3.20 |      312,671,755.73 |   21.0% |      0.00 | :wavy_dash: `fast distance sse 2(8192)` (Unstable with ~3.9 iters. Increase `minEpochIterations` to e.g. 39)
|   268.7% |                7.34 |      136,192,851.21 |    0.1% |      0.00 | `doom distance (8192)`
|   226.6% |                8.70 |      114,894,810.66 |    0.5% |      0.00 | `doom distance 2 (8192)`
|    11.9% |               82.70 |       12,091,512.92 |    5.6% |      0.02 | :wavy_dash: `distance (16384)` (Unstable with ~1.0 iters. Increase `minEpochIterations` to e.g. 10)
|   239.8% |                4.11 |      243,086,053.41 |    0.0% |      0.00 | `fast distance (16384)`
|   372.8% |                2.65 |      377,946,943.48 |    0.0% |      0.00 | `fast distance cast (16384)`
|   346.4% |                2.85 |      351,211,146.84 |    0.2% |      0.00 | `fast distance sse (16384)`
|   372.8% |                2.65 |      377,946,943.48 |    0.0% |      0.00 | `fast distance sse 2(16384)`
|   156.0% |                6.32 |      158,146,718.15 |    0.0% |      0.00 | `doom distance (16384)`
|   132.2% |                7.46 |      134,075,286.42 |    0.3% |      0.00 | `doom distance 2 (16384)`
|     5.9% |               83.90 |       11,918,670.21 |    1.5% |      0.03 | `distance (32768)`
|   119.9% |                4.11 |      243,086,053.41 |    0.0% |      0.00 | `fast distance (32768)`
|   186.2% |                2.65 |      377,511,520.74 |    0.1% |      0.00 | `fast distance cast (32768)`
|   193.5% |                2.55 |      392,431,137.72 |    0.1% |      0.00 | `fast distance sse (32768)`
|   186.4% |                2.65 |      377,946,943.48 |    0.0% |      0.00 | `fast distance sse 2(32768)`
|    63.9% |                7.71 |      129,620,253.16 |    1.0% |      0.00 | `doom distance (32768)`
|    60.3% |                8.18 |      122,314,296.38 |    0.2% |      0.00 | `doom distance 2 (32768)`
|     2.9% |               84.02 |       11,902,435.48 |    8.5% |      0.07 | :wavy_dash: `distance (65536)` (Unstable with ~1.0 iters. Increase `minEpochIterations` to e.g. 10)
|    59.9% |                4.11 |      243,086,053.41 |   11.7% |      0.00 | :wavy_dash: `fast distance (65536)` (Unstable with ~1.0 iters. Increase `minEpochIterations` to e.g. 10)
|    49.2% |                5.01 |      199,683,120.05 |   11.1% |      0.00 | :wavy_dash: `fast distance cast (65536)` (Unstable with ~1.0 iters. Increase `minEpochIterations` to e.g. 10)
|    60.5% |                4.07 |      245,453,183.52 |   34.4% |      0.00 | :wavy_dash: `fast distance sse (65536)` (Unstable with ~1.0 iters. Increase `minEpochIterations` to e.g. 10)
|    57.6% |                4.28 |      233,473,459.21 |   12.6% |      0.00 | :wavy_dash: `fast distance sse 2(65536)` (Unstable with ~1.0 iters. Increase `minEpochIterations` to e.g. 10)
|    34.3% |                7.20 |      138,965,224.77 |    0.8% |      0.01 | `doom distance (65536)`
|    28.2% |                8.74 |      114,393,436.90 |    2.1% |      0.01 | `doom distance 2 (65536)`
|     1.5% |               81.07 |       12,335,375.55 |    0.7% |      0.12 | `distance (131072)`
|    10.5% |               11.69 |       85,533,803.18 |    0.3% |      0.02 | `fast distance (131072)`
|    34.1% |                3.61 |      276,639,932.46 |    0.5% |      0.01 | `fast distance cast (131072)`
|    28.8% |                4.28 |      233,390,313.39 |   10.9% |      0.01 | :wavy_dash: `fast distance sse (131072)` (Unstable with ~1.0 iters. Increase `minEpochIterations` to e.g. 10)
|    34.1% |                3.62 |      276,581,557.29 |    0.6% |      0.01 | `fast distance sse 2(131072)`
|    12.6% |                9.78 |      102,264,180.39 |    1.1% |      0.01 | `doom distance (131072)`
|    16.2% |                7.60 |      131,638,043.59 |    0.1% |      0.01 | `doom distance 2 (131072)`
|     0.8% |               73.76 |       13,556,881.77 |    0.8% |      0.21 | `distance (262144)`
|    11.9% |                5.19 |      192,682,102.17 |    3.8% |      0.02 | `fast distance (262144)`
|    25.9% |                2.38 |      419,564,660.69 |    0.1% |      0.01 | `fast distance cast (262144)`
|    24.2% |                2.55 |      392,431,137.72 |    0.0% |      0.01 | `fast distance sse (262144)`
|    26.0% |                2.37 |      421,656,747.63 |    0.3% |      0.01 | `fast distance sse 2(262144)`
|     7.1% |                8.66 |      115,492,113.84 |   24.0% |      0.04 | :wavy_dash: `doom distance (262144)` (Unstable with ~1.0 iters. Increase `minEpochIterations` to e.g. 10)
|     6.1% |               10.09 |       99,120,505.16 |    2.1% |      0.03 | `doom distance 2 (262144)`
```
