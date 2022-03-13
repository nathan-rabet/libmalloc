
#ifndef DEBUG_H
#define DEBUG_H

#include <stdbool.h>

#include "slab.h"

/**
 * @brief Check if a slab_data is valid (and doesn't SIGSEGV).
 *
 * @param slab_data The slab_data to check.
 * @param size The size of the RAW slab_data.
 * @param is_virgin If the slab_data is virgin (all 0).
 *
 * @return true if the slab_data is valid, false otherwise.
 */
bool debug_check_slab_data_access(struct slab_data *slab_data, size_t size,
                                  bool is_virgin);

/**
 * @brief Check if a calloc call is valid (and doesn't SIGSEGV).
 *
 * @param slab_data_old The slab_data before the calloc call.
 * @param old_size
 * @param slab_data_new
 * @param new_size
 * @return true
 * @return false
 */
bool debug_calloc_slab_data_check_validity(struct slab_data *slab_data_old,
                                           size_t old_size,
                                           struct slab_data *slab_data_new,
                                           size_t new_size);
#endif // DEBUG_H
