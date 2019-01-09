#include <cstdio>
#include <cstdlib>
#include <cassert>

#include "bilateral_grid.h"
#ifndef NO_AUTO_SCHEDULE
#include "bilateral_grid_classic_auto_schedule.h"
#include "bilateral_grid_auto_schedule.h"
#include "bilateral_grid_simple_auto_schedule.h"
#endif

#include "benchmark_util.h"
#include "HalideBuffer.h"
#include "halide_image_io.h"

using namespace Halide::Tools;
using namespace Halide::Runtime;

int main(int argc, char **argv) {
    if (argc < 5) {
        printf("Usage: ./filter input.png output.png range_sigma timing_iterations\n"
               "e.g. ./filter input.png output.png 0.1 10\n");
        return 0;
    }

    float r_sigma = (float) atof(argv[3]);
    const int samples = atoi(argv[4]);
    const int iterations = 10;

    Buffer<float> input = load_and_convert_image(argv[1]);
    Buffer<float> output(input.width(), input.height());

    multi_way_bench({
        {"Manual", [&]() { bilateral_grid(input, r_sigma, output); output.device_sync(); }},
    #ifndef NO_AUTO_SCHEDULE
        {"Classic auto-scheduled", [&]() { bilateral_grid_classic_auto_schedule(input, r_sigma, output); output.device_sync(); }},
        {"Auto-scheduled", [&]() { bilateral_grid_auto_schedule(input, r_sigma, output); output.device_sync(); }},
        {"Simple auto-scheduled", [&]() { bilateral_grid_simple_auto_schedule(input, r_sigma, output); output.device_sync(); }}
    #endif
        },
        samples,
        iterations
    );

    convert_and_save_image(output, argv[2]);

    return 0;
}
