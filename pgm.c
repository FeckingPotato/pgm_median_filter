#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pgm.h"

#define EXIT_IF_TRUE(condition, return_result) if (condition) {result = return_result; goto _exit;}

#define BUFFER_SIZE 8
#define BUFFER_READ_FORMAT "%7s"
#define ASCII_FILE_HEADER "P2"
#define BINARY_FILE_HEADER "P5"
#define FILE_HEADER_SIZE 2
#define STRTOUL_BASE 10

typedef enum {
    UNKNOWN,
    ASCII,
    BINARY
} file_type_e;

static pgm_err_e fwrite_header(FILE *file);

static pgm_err_e fwrite_number(unsigned long number, int newline, FILE *file);

pgm_err_e pgm_init(pgm_size size_x, pgm_size size_y, pgm_value max_value, pgm_t **pgm)
{
    pgm_err_e result = PGM_SUCCESS;

    /* allocating structure memory */
    *pgm = (pgm_t *) calloc(sizeof(pgm_t), 1);
    if (*pgm == NULL)
    {
        result = PGM_MALLOC_ERROR;
        goto _exit;
    }

    /* setting params */
    (*pgm)->size_x = size_x;
    (*pgm)->size_y = size_y;
    (*pgm)->max_value = max_value;

    /* allocating array memory */
    (*pgm)->values = calloc(sizeof(pgm_size), (*pgm)->size_x * (*pgm)->size_y);
    EXIT_IF_TRUE(*pgm == NULL, PGM_MALLOC_ERROR)

_exit:
    if (*pgm != NULL && result != PGM_SUCCESS)
    {
        if ((*pgm)->values != NULL)
        {
            free((*pgm)->values);
        }
        free(*pgm);
        *pgm = NULL;
    }
    return result;
}

pgm_err_e pgm_read(char *file_path, pgm_t **pgm)
{
    pgm_err_e result = PGM_SUCCESS;
    file_type_e file_type = UNKNOWN;
    char *endptr;
    char buffer[BUFFER_SIZE];
    FILE *file = NULL;

    /* opening file */
    file = fopen(file_path, "r");
    EXIT_IF_TRUE(file == NULL, PGM_FILE_ACCESS_ERROR)

    /* checking header */
    fscanf(file, BUFFER_READ_FORMAT, buffer);
    if (strncmp(ASCII_FILE_HEADER, buffer, FILE_HEADER_SIZE + 1) == 0)
    {
        file_type = ASCII;
    }
    else if (strncmp(BINARY_FILE_HEADER, buffer, FILE_HEADER_SIZE + 1) == 0)
    {
        file_type = BINARY;
    }
    else
    {
        EXIT_IF_TRUE(1, PGM_FILE_TYPE_ERROR)
    }

    /* allocating structure memory */
    *pgm = (pgm_t *) calloc(sizeof(pgm_t), 1);
    EXIT_IF_TRUE(*pgm == NULL, PGM_MALLOC_ERROR)

    /* reading size_x */
    fscanf(file, BUFFER_READ_FORMAT, buffer);
    (*pgm)->size_x = strtoul(buffer, &endptr, STRTOUL_BASE);
    EXIT_IF_TRUE(endptr[0] != '\0', PGM_FILE_TYPE_ERROR)

    /* reading size_y */
    fscanf(file, BUFFER_READ_FORMAT, buffer);
    (*pgm)->size_y = strtoul(buffer, &endptr, STRTOUL_BASE);
    EXIT_IF_TRUE(endptr[0] != '\0', PGM_FILE_TYPE_ERROR)

    /* reading max_value */
    fscanf(file, BUFFER_READ_FORMAT, buffer);
    (*pgm)->max_value = strtoul(buffer, &endptr, STRTOUL_BASE);
    EXIT_IF_TRUE(endptr[0] != '\0', PGM_FILE_TYPE_ERROR)

    /* allocating array memory */
    (*pgm)->values = calloc(sizeof(pgm_size), (*pgm)->size_x * (*pgm)->size_y);
    EXIT_IF_TRUE((*pgm)->values == NULL, PGM_MALLOC_ERROR)

    /* reading values */
	if (file_type == ASCII)
	{
		for (pgm_size i = 0; i < ((*pgm)->size_x * (*pgm)->size_y); i++)
		{
			fscanf(file, BUFFER_READ_FORMAT, buffer);
			(*pgm)->values[i] = strtoul(buffer, &endptr, STRTOUL_BASE);
			EXIT_IF_TRUE(endptr[0] != '\0', PGM_FILE_TYPE_ERROR)
		}
	}
	else
	{
		for (pgm_size i = 0; i < ((*pgm)->size_x * (*pgm)->size_y); i++)
		{
            int size;
            if ((*pgm)->max_value < 256)
            {
                size = sizeof(char);
            }
            else
            {
                size = sizeof(pgm_value);
            }
			fread(&((*pgm)->values[i]), size, 1, file);
		}
	}

_exit:
    if (file != NULL)
    {
        fclose(file);
    }
    if (*pgm != NULL && result != PGM_SUCCESS)
    {
        if ((*pgm)->values != NULL)
        {
            free((*pgm)->values);
        }
        free(*pgm);
        *pgm = NULL;
    }
	return result;
}

pgm_err_e pgm_write(char *file_path, pgm_t *pgm)
{
    pgm_err_e result = PGM_SUCCESS;
    FILE *file = NULL;

    /* verifying pointers */
    EXIT_IF_TRUE((pgm == NULL || pgm->values == NULL), PGM_NULL_PTR_ERROR)

    /* opening file */
    file = fopen(file_path, "w");
    EXIT_IF_TRUE(file == NULL, PGM_FILE_ACCESS_ERROR)

    /* writing params */
    result = fwrite_header(file);
    EXIT_IF_TRUE(result != PGM_SUCCESS, result)
    result = fwrite_number(pgm->size_x, 0, file);
    EXIT_IF_TRUE(result != PGM_SUCCESS, result)
    result = fwrite_number(pgm->size_y, 0, file);
    EXIT_IF_TRUE(result != PGM_SUCCESS, result)
    result = fwrite_number(pgm->max_value, 1, file);
    EXIT_IF_TRUE(result != PGM_SUCCESS, result)

    /* writing values */
    for (pgm_size i = 0; i < pgm->size_y; i++)
    {
        for (pgm_size j = 0; j < pgm->size_x - 1; j++)
        {
            result = fwrite_number(pgm->values[i * pgm->size_x + j], 0, file);
            EXIT_IF_TRUE(result != PGM_SUCCESS, result)
        }
        result = fwrite_number(pgm->values[i * pgm->size_x + pgm->size_x - 1], 1, file);
        EXIT_IF_TRUE(result != PGM_SUCCESS, result)
    }
_exit:
    if (file != NULL)
    {
        fclose(file);
    }
    return result;
}

pgm_err_e pgm_get_value(pgm_size x, pgm_size y, pgm_value *value, pgm_t *pgm)
{
    pgm_err_e result = PGM_SUCCESS;

    /* verifying pointers */
    EXIT_IF_TRUE((pgm == NULL || pgm->values == NULL || value == NULL), PGM_NULL_PTR_ERROR)

    /* verifying coordinates */
    EXIT_IF_TRUE((x > pgm->size_x || y > pgm->size_y), PGM_OUT_OF_BOUNDS_ERROR)

    /* getting value */
    *value = pgm->values[y * pgm->size_x + x];

_exit:
    return result;
}

pgm_err_e pgm_set_value(pgm_size x, pgm_size y, pgm_value value, pgm_t *pgm)
{
    pgm_err_e result = PGM_SUCCESS;

    /* verifying pointer */
    EXIT_IF_TRUE((pgm == NULL || pgm->values == NULL), PGM_NULL_PTR_ERROR)

    /* verifying coordinates */
    EXIT_IF_TRUE((x > pgm->size_x || y > pgm->size_y), PGM_OUT_OF_BOUNDS_ERROR)

    /* setting value */
    pgm->values[y * pgm->size_x + x] = value;

_exit:
    return result;
}

pgm_err_e pgm_destroy(pgm_t **pgm)
{
    pgm_err_e result = PGM_SUCCESS;

    /* verifying pointers */
    EXIT_IF_TRUE((*pgm == NULL || (*pgm)->values == NULL), PGM_NULL_PTR_ERROR)

    /* freeing memory */
    free((*pgm)->values);
    free(*pgm);
    *pgm = NULL;
_exit:
    return result;
}

static pgm_err_e fwrite_header(FILE *file)
{
    pgm_err_e result = PGM_SUCCESS;
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "%s ", ASCII_FILE_HEADER);
    if (fwrite(buffer, FILE_HEADER_SIZE + 1, 1, file) != 1)
    {
        result = PGM_FILE_WRITE_ERROR;
    }
    return result;
}

static pgm_err_e fwrite_number(unsigned long number, int newline, FILE *file)
{
    pgm_err_e result = PGM_SUCCESS;
    char buffer[BUFFER_SIZE];
    if (newline)
    {
        sprintf(buffer, "%lu\n", number);
    }
    else
    {
        sprintf(buffer, "%lu ", number);
    }
    if (fwrite(buffer, strlen(buffer), 1, file) != 1)
    {
        result = PGM_FILE_WRITE_ERROR;
    }
    return result;
}
