#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "pgm.h"
#include "filter.h"

#define DEFAULT_X 256
#define DEFAULT_Y 256
#define DEFAULT_WINDOW_X 5
#define DEFAULT_WINDOW_Y 1

#define EXAMPLE_COMMAND "example"
#define FILE_COMMAND "file"
#define EXAMPLE_NOISY_PATH "pgm_noisy.pgm"
#define EXAMPLE_FILTERED_PATH "pgm_filtered.pgm"

typedef enum
{
    UNKNOWN,
    TEST,
    IMAGE
} program_modes_e;

int main(int argc, char *argv[])
{
    int result = PGM_SUCCESS;
    pgm_size size_x;
    pgm_size size_y;
    char *input_path = NULL;
    char *output_path = EXAMPLE_FILTERED_PATH;
    pgm_t* first_pgm = NULL;
    pgm_t* second_pgm = NULL;
    program_modes_e mode = UNKNOWN;
    srand(time(NULL));
    if (argc == 1)
    {
        printf("Example mode\n"
               "Using default example settings\n");
        size_x = DEFAULT_X;
        size_y = DEFAULT_Y;
        mode = TEST;
    }
    else if (argc == 4)
    {
        if (strncmp(argv[1], FILE_COMMAND, sizeof(FILE_COMMAND)) != 0)
        {
            printf("Example mode\n");
            size_x = strtoul(argv[1], NULL, 10);
            size_y = strtoul(argv[2], NULL, 10);
            if (size_x == 0 || size_y == 0)
            {
                printf("Wrong image size\n");
                goto _exit;
            }
            mode = TEST;
        }
        else if (strncmp(argv[1], EXAMPLE_COMMAND, sizeof(EXAMPLE_COMMAND)) != 0)
        {
            printf("Custom image mode\n");
            input_path = argv[1];
            output_path = argv[2];
            mode = IMAGE;
        }
    }
    else
    {
        printf("Usage:\n"
               "./median_filter - run example with default settings\n"
               "./median_filter example <size_x> <size_y> - generates a random <size_x> by <size_y> gray image (saved"
               " as "EXAMPLE_NOISY_PATH") and filters it (saved as "EXAMPLE_FILTERED_PATH")\n"
               "./median_filter file <input_path> <output_path> - filters a pgm image and saves it in a different"
               "file\n");
        goto _exit;
    }
    if (mode == TEST)
    {
        printf("Generating a noisy %ux%u gray image.\n", size_x, size_y);
        result = pgm_generate_noisy_image(size_x, size_y, &first_pgm);
        if (result != PGM_SUCCESS)
        {
            printf("Unable to generate an image, exiting\n");
            goto _exit;
        }
        result = pgm_write("pgm_noisy.pgm", first_pgm);
        if (result != PGM_SUCCESS)
        {
            printf("Unable to save the generated image, exiting\n");
            goto _exit;
        }
    }
    else if (mode == IMAGE)
    {
        printf("Reading an image from %s\n", input_path);
        result = pgm_read(input_path, &first_pgm);
        if (result != PGM_SUCCESS)
        {
            printf("Unable to read pgm image, exiting\n");
            goto _exit;
        }
    }
    else
    {
        printf("This shouldn't have happened, exiting\n");
        goto _exit;
    }
    printf("Filtering the image\n");
    pgm_median_filter(DEFAULT_WINDOW_X, DEFAULT_WINDOW_Y, first_pgm, &second_pgm);
    result = pgm_write(output_path, second_pgm);
    if (result != PGM_SUCCESS)
    {
        printf("Unable to save the filtered image, exiting\n");
        goto _exit;
    }
    printf("Done\n");
_exit:
    pgm_destroy(&first_pgm);
    pgm_destroy(&second_pgm);
    return result;
}
