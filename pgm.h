#pragma once
typedef unsigned int pgm_size;
typedef unsigned short pgm_value;

typedef enum
{
	PGM_SUCCESS,
	PGM_FILE_ACCESS_ERROR,
    PGM_FILE_TYPE_ERROR,
    PGM_FILE_WRITE_ERROR,
	PGM_MALLOC_ERROR,
    PGM_NULL_PTR_ERROR,
	PGM_OUT_OF_BOUNDS_ERROR
} pgm_err_e;

typedef struct
{
	pgm_size size_x;
	pgm_size size_y;
	pgm_value max_value;
	pgm_value *values;
} pgm_t;

pgm_err_e pgm_init(pgm_size size_x, pgm_size size_y, pgm_value max_value, pgm_t **pgm);

pgm_err_e pgm_read(char *file_path, pgm_t **pgm);

pgm_err_e pgm_write(char *file_path, pgm_t *pgm);

pgm_err_e pgm_get_value(pgm_size x, pgm_size y, pgm_value *value, pgm_t *pgm);

pgm_err_e pgm_set_value(pgm_size x, pgm_size y, pgm_value value, pgm_t *pgm);

pgm_err_e pgm_destroy(pgm_t **pgm);
