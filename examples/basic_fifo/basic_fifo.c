/*******************************************************************************
 *  @file       basic_fifo.c
 *
 *  @copyright  Accolade Electronics Pvt Ltd, 2025-26
 *              All Rights Reserved
 *              UNPUBLISHED, LICENSED SOFTWARE.
 *              Accolade Electronics, Pune
 *              CONFIDENTIAL AND PROPRIETARY INFORMATION
 *              WHICH IS THE PROPERTY OF M/s Accolade Electronics.
 *
 *  @version    0.0.2
 *
 *  @date       22 August 2025
 *
 *  @brief      Implementation of the 'basic_fifo' Example.
*******************************************************************************/

/* Self. */
#include "basic_fifo.h"
#include "basic_fifo_priv.h"

basic_fifo_error_e basic_fifo_init(
    basic_fifo_context_s* ptr_context,
    size_t num_elements,
    size_t element_size
) {
    basic_fifo_error_e error = BASIC_FIFO_E_OK;
    asl_fifo_error_e fifo_error = ASL_FIFO_E_OK;
    size_t buffer_size = 0;
    
    if ((NULL != ptr_context) && 
        (num_elements >= BASIC_FIFO_MIN_NUM_ELEMENTS) && 
        (num_elements <= BASIC_FIFO_MAX_NUM_ELEMENTS) &&
        (element_size >= BASIC_FIFO_MIN_ELEMENT_SIZE) && 
        (element_size <= BASIC_FIFO_MAX_ELEMENT_SIZE)) {
        
        /* Clear context. */
        memset(ptr_context, 0, sizeof(basic_fifo_context_s));
        
        /* Calculate buffer size. */
        buffer_size = num_elements * element_size;
        
        /* Allocate buffer memory. */
        ptr_context->buffer_memory = malloc(buffer_size);
        if (NULL != ptr_context->buffer_memory) {
            /* Setup FIFO structure. */
            ptr_context->fifo.elements.ptr = ptr_context->buffer_memory;
            ptr_context->fifo.elements.size = buffer_size;
            ptr_context->fifo.size_element = element_size;
            ptr_context->fifo.index_write = 0;
            ptr_context->fifo.index_read = 0;
            ptr_context->element_size = element_size;
            ptr_context->max_elements = num_elements;
            
            /* Reset ASL FIFO. */
            fifo_error = asl_fifo_reset(&ptr_context->fifo);
            if (ASL_FIFO_E_OK == fifo_error) {
                ptr_context->test_passed = false;
                printf("FIFO initialized: %zu elements x %zu bytes each = %zu bytes total\n", 
                       num_elements, element_size, buffer_size);
            } else {
                free(ptr_context->buffer_memory);
                ptr_context->buffer_memory = NULL;
                error = BASIC_FIFO_E_MEMORY;
            }
        } else {
            error = BASIC_FIFO_E_MEMORY;
        }
    } else {
        error = BASIC_FIFO_E_PARAM;
    }
    
    return error;
}

basic_fifo_error_e basic_fifo_run_tests(basic_fifo_context_s* ptr_context) {
    basic_fifo_error_e error = BASIC_FIFO_E_OK;
    
    if (NULL != ptr_context) {
        printf("Starting comprehensive FIFO tests...\n");
        
        /* Test 1: Empty FIFO operations. */
        printf("Test 1: Empty FIFO operations\n");
        error = basic_fifo_test_empty(ptr_context);
        if (BASIC_FIFO_E_OK != error) {
            printf("Empty FIFO test FAILED\n");
        } else {
            printf("Empty FIFO test PASSED\n");
        }
        
        /* Test 2: Full FIFO operations. */
        if (BASIC_FIFO_E_OK == error) {
            printf("Test 2: Full FIFO operations\n");
            error = basic_fifo_test_full(ptr_context);
            if (BASIC_FIFO_E_OK != error) {
                printf("Full FIFO test FAILED\n");
            } else {
                printf("Full FIFO test PASSED\n");
            }
        }
        
        /* Set overall test result. */
        if (BASIC_FIFO_E_OK == error) {
            ptr_context->test_passed = true;
            printf("All FIFO tests PASSED!\n");
        } else {
            ptr_context->test_passed = false;
            printf("FIFO tests FAILED!\n");
        }
    } else {
        error = BASIC_FIFO_E_PARAM;
    }
    
    return error;
}

basic_fifo_error_e basic_fifo_cleanup(basic_fifo_context_s* ptr_context) {
    basic_fifo_error_e error = BASIC_FIFO_E_OK;
    
    if (NULL != ptr_context) {
        /* Free allocated memory. */
        if (ptr_context->buffer_memory) {
            free(ptr_context->buffer_memory);
            ptr_context->buffer_memory = NULL;
        }
        
        /* Clear context. */
        memset(ptr_context, 0, sizeof(basic_fifo_context_s));
    } else {
        error = BASIC_FIFO_E_PARAM;
    }
    
    return error;
}

void basic_fifo_print_usage(const char* program_name) {
    printf("Usage: %s [num_elements] [element_size]\n", program_name);
    printf("  num_elements: Number of elements FIFO should hold (default: %d)\n", 
           BASIC_FIFO_DEFAULT_NUM_ELEMENTS);
    printf("  element_size: Size of each element in bytes (default: %d)\n", 
           BASIC_FIFO_DEFAULT_ELEMENT_SIZE);
    printf("  Valid ranges:\n");
    printf("    num_elements: %d to %d\n", 
           BASIC_FIFO_MIN_NUM_ELEMENTS, BASIC_FIFO_MAX_NUM_ELEMENTS);
    printf("    element_size: %d to %d bytes\n", 
           BASIC_FIFO_MIN_ELEMENT_SIZE, BASIC_FIFO_MAX_ELEMENT_SIZE);
}

void basic_fifo_print_results(basic_fifo_context_s* ptr_context) {
    size_t capacity = 0;
    size_t used = 0;
    size_t free_count = 0;
    
    if (NULL == ptr_context) {
        return;
    }
    
    /* Get current FIFO status. */
    asl_fifo_get_count_capacity(&ptr_context->fifo, &capacity);
    asl_fifo_get_count_used(&ptr_context->fifo, &used);
    asl_fifo_get_count_free(&ptr_context->fifo, &free_count);
    
    printf("\nTest Results:\n");
    printf("FIFO Configuration:\n");
    printf("  Max Elements: %zu\n", ptr_context->max_elements);
    printf("  Element Size: %zu bytes\n", ptr_context->element_size);
    printf("  Total Buffer: %zu bytes\n", ptr_context->max_elements * ptr_context->element_size);
    printf("FIFO Status:\n");
    printf("  Capacity: %zu elements\n", capacity);
    printf("  Used: %zu elements\n", used);
    printf("  Free: %zu elements\n", free_count);
    printf("Overall Status: %s\n", ptr_context->test_passed ? "PASSED" : "FAILED");
}

// Private function implementations

static basic_fifo_error_e basic_fifo_test_empty(basic_fifo_context_s* ptr_context) {
    basic_fifo_error_e error = BASIC_FIFO_E_OK;
    asl_fifo_error_e fifo_error = ASL_FIFO_E_OK;
    asl_buffer_s element_buffer = {0};
    uint8_t* test_element = NULL;
    size_t used = 0;
    size_t free_count = 0;
    
    if (NULL != ptr_context) {
        /* Allocate memory for test element. */
        test_element = malloc(ptr_context->element_size);
        if (NULL != test_element) {
            element_buffer.ptr = test_element;
            element_buffer.size = ptr_context->element_size;
            
            /* Verify FIFO is empty. */
            fifo_error = asl_fifo_get_count_used(&ptr_context->fifo, &used);
            if ((ASL_FIFO_E_OK == fifo_error) && (0 == used)) {
                printf("  - FIFO is initially empty: PASSED\n");
                
                /* Test dequeue from empty FIFO (should fail). */
                fifo_error = asl_fifo_dequeue(&ptr_context->fifo, &element_buffer);
                if (ASL_FIFO_E_EMPTY == fifo_error) {
                    printf("  - Dequeue from empty FIFO correctly failed: PASSED\n");
                    
                    /* Test preview from empty FIFO (should fail). */
                    fifo_error = asl_fifo_preview(&ptr_context->fifo, &element_buffer);
                    if (ASL_FIFO_E_EMPTY == fifo_error) {
                        printf("  - Preview from empty FIFO correctly failed: PASSED\n");
                        
                        /* Verify free count equals capacity. */
                        size_t capacity = 0;
                        asl_fifo_error_e capacity_error = asl_fifo_get_count_capacity(&ptr_context->fifo, &capacity);
                        fifo_error = asl_fifo_get_count_free(&ptr_context->fifo, &free_count);
                        if ((ASL_FIFO_E_OK == capacity_error) && (ASL_FIFO_E_OK == fifo_error) && (free_count == capacity)) {
                            printf("  - Free count equals capacity: PASSED\n");
                        } else {
                            printf("  - Free count check FAILED\n");
                            error = BASIC_FIFO_E_TEST_FAIL;
                        }
                    } else {
                        printf("  - Preview from empty FIFO test FAILED\n");
                        error = BASIC_FIFO_E_TEST_FAIL;
                    }
                } else {
                    printf("  - Dequeue from empty FIFO test FAILED (got error 0x%X)\n", fifo_error);
                    error = BASIC_FIFO_E_TEST_FAIL;
                }
            } else {
                printf("  - FIFO empty check FAILED\n");
                error = BASIC_FIFO_E_TEST_FAIL;
            }
            
            free(test_element);
        } else {
            error = BASIC_FIFO_E_MEMORY;
        }
    } else {
        error = BASIC_FIFO_E_PARAM;
    }
    
    return error;
}

static basic_fifo_error_e basic_fifo_fill_to_capacity(
    basic_fifo_context_s* ptr_context, 
    size_t capacity, 
    uint8_t* test_element
) {
    basic_fifo_error_e error = BASIC_FIFO_E_OK;
    asl_fifo_error_e fifo_error = ASL_FIFO_E_OK;
    asl_buffer_s element_buffer = {0};
    
    if ((NULL != ptr_context) && (NULL != test_element)) {
        /* Fill FIFO to capacity. */
        printf("  - Filling FIFO to capacity (%zu elements)...\n", capacity);
        for (size_t i = 0; i < capacity; i++) {
            /* Create test pattern. */
            for (size_t j = 0; j < ptr_context->element_size; j++) {
                test_element[j] = (uint8_t)((i + j) & 0xFF);
            }
            
            element_buffer.ptr = test_element;
            element_buffer.size = ptr_context->element_size;
            
            fifo_error = asl_fifo_enqueue(&ptr_context->fifo, element_buffer);
            if (ASL_FIFO_E_OK != fifo_error) {
                printf("  - Enqueue element %zu FAILED\n", i);
                error = BASIC_FIFO_E_TEST_FAIL;
                break;
            }
        }
    } else {
        error = BASIC_FIFO_E_PARAM;
    }
    
    return error;
}

static basic_fifo_error_e basic_fifo_test_enqueue_full(
    basic_fifo_context_s* ptr_context, 
    asl_buffer_s element_buffer
) {
    basic_fifo_error_e error = BASIC_FIFO_E_OK;
    asl_fifo_error_e fifo_error = ASL_FIFO_E_OK;
    size_t free_count = 0;
    
    if (NULL != ptr_context) {
        /* Verify FIFO is full. */
        fifo_error = asl_fifo_get_count_free(&ptr_context->fifo, &free_count);
        if ((ASL_FIFO_E_OK == fifo_error) && (0 == free_count)) {
            printf("  - FIFO is full: PASSED\n");
            
            /* Test enqueue to full FIFO (should fail). */
            fifo_error = asl_fifo_enqueue(&ptr_context->fifo, element_buffer);
            if (ASL_FIFO_E_FULL == fifo_error) {
                printf("  - Enqueue to full FIFO correctly failed: PASSED\n");
            } else {
                printf("  - Enqueue to full FIFO test FAILED\n");
                error = BASIC_FIFO_E_TEST_FAIL;
            }
        } else {
            printf("  - FIFO full check FAILED\n");
            error = BASIC_FIFO_E_TEST_FAIL;
        }
    } else {
        error = BASIC_FIFO_E_PARAM;
    }
    
    return error;
}

static basic_fifo_error_e basic_fifo_dequeue_and_verify(
    basic_fifo_context_s* ptr_context, 
    size_t capacity, 
    uint8_t* received_element
) {
    basic_fifo_error_e error = BASIC_FIFO_E_OK;
    asl_fifo_error_e fifo_error = ASL_FIFO_E_OK;
    asl_buffer_s element_buffer = {0};
    size_t used = 0;
    bool data_valid = true;
    
    if ((NULL != ptr_context) && (NULL != received_element)) {
        /* Dequeue all elements and verify data. */
        printf("  - Dequeuing and verifying all elements...\n");
        for (size_t i = 0; i < capacity; i++) {
            element_buffer.ptr = received_element;
            element_buffer.size = ptr_context->element_size;
            
            fifo_error = asl_fifo_dequeue(&ptr_context->fifo, &element_buffer);
            if (ASL_FIFO_E_OK == fifo_error) {
                /* Verify data pattern. */
                for (size_t j = 0; j < ptr_context->element_size; j++) {
                    uint8_t expected = (uint8_t)((i + j) & 0xFF);
                    if (received_element[j] != expected) {
                        printf("  - Data mismatch in element %zu, byte %zu: expected 0x%02X, got 0x%02X\n",
                               i, j, expected, received_element[j]);
                        data_valid = false;
                        break;
                    }
                }
                if (!data_valid) {
                    break;
                }
            } else {
                printf("  - Dequeue element %zu FAILED\n", i);
                error = BASIC_FIFO_E_TEST_FAIL;
                break;
            }
        }
        
        if (BASIC_FIFO_E_OK == error) {
            if (data_valid) {
                printf("  - All data validation PASSED\n");
                
                /* Verify FIFO is empty again. */
                fifo_error = asl_fifo_get_count_used(&ptr_context->fifo, &used);
                if ((ASL_FIFO_E_OK == fifo_error) && (0 == used)) {
                    printf("  - FIFO is empty after dequeue: PASSED\n");
                } else {
                    printf("  - FIFO empty check after dequeue FAILED\n");
                    error = BASIC_FIFO_E_TEST_FAIL;
                }
            } else {
                printf("  - Data validation FAILED\n");
                error = BASIC_FIFO_E_TEST_FAIL;
            }
        }
    } else {
        error = BASIC_FIFO_E_PARAM;
    }
    
    return error;
}

static basic_fifo_error_e basic_fifo_test_full(basic_fifo_context_s* ptr_context) {
    basic_fifo_error_e error = BASIC_FIFO_E_OK;
    asl_fifo_error_e fifo_error = ASL_FIFO_E_OK;
    asl_buffer_s element_buffer = {0};
    uint8_t* test_element = NULL;
    uint8_t* received_element = NULL;
    size_t capacity = 0;
    
    if (NULL != ptr_context) {
        /* Get actual FIFO capacity. */
        fifo_error = asl_fifo_get_count_capacity(&ptr_context->fifo, &capacity);
        if (ASL_FIFO_E_OK != fifo_error) {
            error = BASIC_FIFO_E_TEST_FAIL;
        } else {
            /* Allocate memory for test elements. */
            test_element = malloc(ptr_context->element_size);
            received_element = malloc(ptr_context->element_size);
            if ((NULL != test_element) && (NULL != received_element)) {
                
                /* Fill FIFO to capacity. */
                error = basic_fifo_fill_to_capacity(ptr_context, capacity, test_element);
                
                if (BASIC_FIFO_E_OK == error) {
                    /* Test enqueue to full FIFO. */
                    element_buffer.ptr = test_element;
                    element_buffer.size = ptr_context->element_size;
                    error = basic_fifo_test_enqueue_full(ptr_context, element_buffer);
                    
                    if (BASIC_FIFO_E_OK == error) {
                        /* Dequeue all elements and verify data. */
                        error = basic_fifo_dequeue_and_verify(ptr_context, capacity, received_element);
                    }
                }
                
                /* Free allocated memory. */
                if (test_element) {
                    free(test_element);
                }
                if (received_element) {
                    free(received_element);
                }
            } else {
                error = BASIC_FIFO_E_MEMORY;
            }
        }
    } else {
        error = BASIC_FIFO_E_PARAM;
    }
    
    return error;
}
