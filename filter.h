#pragma once

#include "pgm.h"

pgm_err_e pgm_generate_noisy_image(pgm_size size_x, pgm_size size_y, pgm_t **output);

pgm_err_e pgm_median_filter(pgm_size window_size_x, pgm_size window_size_y, pgm_t *input, pgm_t **output);
