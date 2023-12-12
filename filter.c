#include <stdlib.h>
#include "filter.h"

#define TEST_MAX_VALUE 256
#define TEST_NOISE_THRESHOLD 26
#define EXIT_IF_TRUE(condition, return_result) if (condition) {result = return_result; goto _exit;}

static void fill_window(pgm_size x, pgm_size y,
                        pgm_size window_size_x, pgm_size window_size_y,
                        pgm_t *input, pgm_value *window);
static pgm_value find_median(pgm_size window_size_x, pgm_size window_size_y, pgm_value *window);
static int sort_compare(const void *a, const void *b);

pgm_err_e pgm_generate_noisy_image(pgm_size size_x, pgm_size size_y, pgm_t **output)
{
    pgm_err_e result = PGM_SUCCESS;

    /* creating a blank grayscale image */
    result = pgm_init(size_x, size_y, TEST_MAX_VALUE, output);
    EXIT_IF_TRUE(result != PGM_SUCCESS, result)

    for (pgm_size i = 0; i < size_x; i++)
    {
        for (pgm_size j = 0; j < size_y; j++)
        {
            pgm_value value = TEST_MAX_VALUE / 2;
            /* creating a gray image with 10% probability of pixel being noisy */
            if (rand() % 101 <= 10)
            {
                value -= rand() % TEST_NOISE_THRESHOLD;
            }
            pgm_set_value(i, j, value, *output);
        }
    }
_exit:
    return result;
}

pgm_err_e pgm_median_filter(pgm_size window_size_x, pgm_size window_size_y, pgm_t *input, pgm_t **output)
{
    pgm_err_e result = PGM_SUCCESS;
    pgm_value *window = NULL;

    /* verifying pointers */
    EXIT_IF_TRUE(input == NULL || input->values == NULL, PGM_NULL_PTR_ERROR)

    /* verifying length size */
    EXIT_IF_TRUE(input->size_x < window_size_x || input->size_y < window_size_y, PGM_OUT_OF_BOUNDS_ERROR)

    /* creating a blank grayscale image */
    result = pgm_init(input->size_x, input->size_y, input->max_value, output);
    EXIT_IF_TRUE(result != PGM_SUCCESS, result)

    /* allocating window */
    window = calloc(sizeof(pgm_value), window_size_x * window_size_y);
    EXIT_IF_TRUE(window == NULL, PGM_MALLOC_ERROR)

    /* filtering */
    for (pgm_size i = 0; i < input->size_x; i++)
    {
        for (pgm_size j = 0; j < input->size_y; j++)
        {
            if ((i < window_size_x || i >= input->size_x - window_size_x)
                ||
                (j < window_size_y || j >= input->size_x - window_size_y))
            {
                /* skipping edges */
                pgm_value value;
                pgm_get_value(i, j, &value, input);
                pgm_set_value(i, j, value, *output);
                continue;
            }
            fill_window(i, j, window_size_x, window_size_y, input, window);
            pgm_set_value(i, j, find_median(window_size_x, window_size_y, window), *output);
        }
    }
_exit:
    if (window != NULL)
    {
        free(window);
    }
    return result;
}

static void fill_window(pgm_size x, pgm_size y,
                        pgm_size window_size_x, pgm_size window_size_y,
                        pgm_t *input, pgm_value *window)
{
    for (pgm_size i = 0; i < window_size_x; i++)
    {
        for (pgm_size j = 0; j < window_size_y; j++)
        {
            pgm_value value;
            pgm_get_value(x + i, y + j, &value, input);
            window[j * window_size_x + i] = value;
        }
    }
}

static pgm_value find_median(pgm_size window_size_x, pgm_size window_size_y, pgm_value *window)
{
    pgm_size length = window_size_x * window_size_y;
    qsort(window, length, sizeof(pgm_value), sort_compare);
    return window[length / 2];
}

static int sort_compare(const void *a, const void *b)
{
    int result = 0;
    if (*(pgm_value *) a < *(pgm_value *) b)
    {
        result = -1;
    }
    else if (*(pgm_value *) a > *(pgm_value *) b)
    {
        result = 1;
    }
    return result;
}