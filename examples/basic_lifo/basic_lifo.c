/*******************************************************************************
 *  @file       basic_lifo.c
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
 *  @brief      Implementation of the 'basic_lifo' Example.
*******************************************************************************/

/* Self. */
#include "basic_lifo.h"
#include "basic_lifo_priv.h"

basic_lifo_error_e basic_lifo_init(
    basic_lifo_context_s* ptr_context,
    size_t num_elements,
    size_t element_size
) {
    basic_lifo_error_e error = BASIC_LIFO_E_OK;
    asl_lifo_error_e lifo_error = ASL_LIFO_E_OK;
    size_t buffer_size = 0;
    
    if ((NULL != ptr_context) && 
        (num_elements >= BASIC_LIFO_MIN_NUM_ELEMENTS) && 
        (num_elements <= BASIC_LIFO_MAX_NUM_ELEMENTS) &&
        (element_size >= BASIC_LIFO_MIN_ELEMENT_SIZE) && 
        (element_size <= BASIC_LIFO_MAX_ELEMENT_SIZE)) {
        
        /* Clear context. */
        memset(ptr_context, 0, sizeof(basic_lifo_context_s));
        
        /* Calculate buffer size. */
        buffer_size = num_elements * element_size;
        
        /* Allocate buffer memory. */
        ptr_context->buffer_memory = malloc(buffer_size);
        if (NULL != ptr_context->buffer_memory) {
            /* Setup LIFO structure. */
            ptr_context->lifo.elements.ptr = ptr_context->buffer_memory;
            ptr_context->lifo.elements.size = buffer_size;
            ptr_context->lifo.size_element = element_size;
            ptr_context->lifo.top = 0;
            ptr_context->element_size = element_size;
            ptr_context->max_elements = num_elements;
            
            /* Reset ASL LIFO. */
            lifo_error = asl_lifo_reset(&ptr_context->lifo);
            if (ASL_LIFO_E_OK == lifo_error) {
                ptr_context->test_passed = false;
                printf("LIFO initialized: %zu elements x %zu bytes each = %zu bytes total\n", 
                       num_elements, element_size, buffer_size);
            } else {
                free(ptr_context->buffer_memory);
                ptr_context->buffer_memory = NULL;
                error = BASIC_LIFO_E_MEMORY;
            }
        } else {
            error = BASIC_LIFO_E_MEMORY;
        }
    } else {
        error = BASIC_LIFO_E_PARAM;
    }
    
    return error;
}

basic_lifo_error_e basic_lifo_run_tests(basic_lifo_context_s* ptr_context) {
    basic_lifo_error_e error = BASIC_LIFO_E_OK;
    
    if (NULL != ptr_context) {
        printf("Starting comprehensive LIFO tests...\n");
        
        /* Test 1: Empty LIFO operations. */
        printf("Test 1: Empty LIFO operations\n");
        error = basic_lifo_test_empty(ptr_context);
        if (BASIC_LIFO_E_OK != error) {
            printf("Empty LIFO test FAILED\n");
        } else {
            printf("Empty LIFO test PASSED\n");
        }
        
        /* Test 2: Full LIFO operations. */
        if (BASIC_LIFO_E_OK == error) {
            printf("Test 2: Full LIFO operations\n");
            error = basic_lifo_test_full(ptr_context);
            if (BASIC_LIFO_E_OK != error) {
                printf("Full LIFO test FAILED\n");
            } else {
                printf("Full LIFO test PASSED\n");
            }
        }
        
        /* Set overall test result. */
        if (BASIC_LIFO_E_OK == error) {
            ptr_context->test_passed = true;
            printf("All LIFO tests PASSED!\n");
        } else {
            ptr_context->test_passed = false;
            printf("LIFO tests FAILED!\n");
        }
    } else {
        error = BASIC_LIFO_E_PARAM;
    }
    
    return error;
}

basic_lifo_error_e basic_lifo_cleanup(basic_lifo_context_s* ptr_context) {
    basic_lifo_error_e error = BASIC_LIFO_E_OK;
    
    if (NULL != ptr_context) {
        /* Free allocated memory. */
        if (ptr_context->buffer_memory) {
            free(ptr_context->buffer_memory);
            ptr_context->buffer_memory = NULL;
        }
        
        /* Clear context. */
        memset(ptr_context, 0, sizeof(basic_lifo_context_s));
    } else {
        error = BASIC_LIFO_E_PARAM;
    }
    
    return error;
}

void basic_lifo_print_usage(const char* program_name) {
    printf("Usage: %s [num_elements] [element_size]\n", program_name);
    printf("  num_elements: Number of elements LIFO should hold (default: %d)\n", 
           BASIC_LIFO_DEFAULT_NUM_ELEMENTS);
    printf("  element_size: Size of each element in bytes (default: %d)\n", 
           BASIC_LIFO_DEFAULT_ELEMENT_SIZE);
    printf("  Valid ranges:\n");
    printf("    num_elements: %d to %d\n", 
           BASIC_LIFO_MIN_NUM_ELEMENTS, BASIC_LIFO_MAX_NUM_ELEMENTS);
    printf("    element_size: %d to %d bytes\n", 
           BASIC_LIFO_MIN_ELEMENT_SIZE, BASIC_LIFO_MAX_ELEMENT_SIZE);
}

void basic_lifo_print_results(basic_lifo_context_s* ptr_context) {
    size_t capacity = 0;
    size_t used = 0;
    size_t free_count = 0;
    
    if (NULL == ptr_context) {
        return;
    }
    
    /* Get current LIFO status. */
    asl_lifo_get_count_capacity(&ptr_context->lifo, &capacity);
    asl_lifo_get_count_used(&ptr_context->lifo, &used);
    asl_lifo_get_count_free(&ptr_context->lifo, &free_count);
    
    printf("\nTest Results:\n");
    printf("LIFO Configuration:\n");
    printf("  Max Elements: %zu\n", ptr_context->max_elements);
    printf("  Element Size: %zu bytes\n", ptr_context->element_size);
    printf("  Total Buffer: %zu bytes\n", ptr_context->max_elements * ptr_context->element_size);
    printf("LIFO Status:\n");
    printf("  Capacity: %zu elements\n", capacity);
    printf("  Used: %zu elements\n", used);
    printf("  Free: %zu elements\n", free_count);
    printf("Overall Status: %s\n", ptr_context->test_passed ? "PASSED" : "FAILED");
}

// Private function implementations

static basic_lifo_error_e basic_lifo_test_empty(basic_lifo_context_s* ptr_context) {
    basic_lifo_error_e error = BASIC_LIFO_E_OK;
    asl_lifo_error_e lifo_error = ASL_LIFO_E_OK;
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
            
            /* Verify LIFO is empty. */
            lifo_error = asl_lifo_get_count_used(&ptr_context->lifo, &used);
            if ((ASL_LIFO_E_OK == lifo_error) && (0 == used)) {
                printf("  - LIFO is initially empty: PASSED\n");
                
                /* Test pop from empty LIFO (should fail). */
                lifo_error = asl_lifo_pop(&ptr_context->lifo, &element_buffer);
                if (ASL_LIFO_E_EMPTY == lifo_error) {
                    printf("  - Pop from empty LIFO correctly failed: PASSED\n");
                    
                    /* Test peek from empty LIFO (should fail). */
                    lifo_error = asl_lifo_peek(&ptr_context->lifo, &element_buffer);
                    if (ASL_LIFO_E_EMPTY == lifo_error) {
                        printf("  - Peek from empty LIFO correctly failed: PASSED\n");
                        
                        /* Verify free count equals capacity. */
                        size_t capacity = 0;
                        asl_lifo_error_e capacity_error = asl_lifo_get_count_capacity(&ptr_context->lifo, &capacity);
                        lifo_error = asl_lifo_get_count_free(&ptr_context->lifo, &free_count);
                        if ((ASL_LIFO_E_OK == capacity_error) && (ASL_LIFO_E_OK == lifo_error) && (free_count == capacity)) {
                            printf("  - Free count equals capacity: PASSED\n");
                        } else {
                            printf("  - Free count check FAILED\n");
                            error = BASIC_LIFO_E_TEST_FAIL;
                        }
                    } else {
                        printf("  - Peek from empty LIFO test FAILED\n");
                        error = BASIC_LIFO_E_TEST_FAIL;
                    }
                } else {
                    printf("  - Pop from empty LIFO test FAILED (got error 0x%X)\n", lifo_error);
                    error = BASIC_LIFO_E_TEST_FAIL;
                }
            } else {
                printf("  - LIFO empty check FAILED\n");
                error = BASIC_LIFO_E_TEST_FAIL;
            }
            
            free(test_element);
        } else {
            error = BASIC_LIFO_E_MEMORY;
        }
    } else {
        error = BASIC_LIFO_E_PARAM;
    }
    
    return error;
}

static basic_lifo_error_e basic_lifo_fill_to_capacity(
    basic_lifo_context_s* ptr_context,
    uint8_t* test_element,
    asl_buffer_s* element_buffer) {
    
    basic_lifo_error_e error = BASIC_LIFO_E_OK;
    asl_lifo_error_e lifo_error = ASL_LIFO_E_OK;
    size_t capacity = 0;
    
    if ((NULL != ptr_context) && (NULL != test_element) && (NULL != element_buffer)) {
        /* Get LIFO capacity. */
        lifo_error = asl_lifo_get_count_capacity(&ptr_context->lifo, &capacity);
        if (ASL_LIFO_E_OK == lifo_error) {
            printf("  - LIFO capacity: %zu elements\n", capacity);
            
            /* Fill LIFO to capacity. */
            element_buffer->ptr = test_element;
            element_buffer->size = ptr_context->element_size;
            
            for (size_t i = 0; (i < capacity) && (BASIC_LIFO_E_OK == error); i++) {
                /* Fill element with test pattern. */
                memset(test_element, (int)(i & 0xFF), ptr_context->element_size);
                
                /* Push element. */
                lifo_error = asl_lifo_push(&ptr_context->lifo, *element_buffer);
                if (ASL_LIFO_E_OK != lifo_error) {
                    printf("  - Push element %zu FAILED (error 0x%X)\n", i, lifo_error);
                    error = BASIC_LIFO_E_TEST_FAIL;
                }
            }
            
            if (BASIC_LIFO_E_OK == error) {
                printf("  - Pushed %zu elements: PASSED\n", capacity);
            }
        } else {
            printf("  - Get LIFO capacity FAILED\n");
            error = BASIC_LIFO_E_TEST_FAIL;
        }
    } else {
        error = BASIC_LIFO_E_PARAM;
    }
    
    return error;
}

static basic_lifo_error_e basic_lifo_verify_full_state(
    basic_lifo_context_s* ptr_context,
    uint8_t* test_element,
    asl_buffer_s* element_buffer) {
    
    basic_lifo_error_e error = BASIC_LIFO_E_OK;
    asl_lifo_error_e lifo_error = ASL_LIFO_E_OK;
    size_t capacity = 0;
    size_t used = 0;
    size_t free_count = 0;
    
    if ((NULL != ptr_context) && (NULL != test_element) && (NULL != element_buffer)) {
        /* Get capacity for verification. */
        lifo_error = asl_lifo_get_count_capacity(&ptr_context->lifo, &capacity);
        if (ASL_LIFO_E_OK == lifo_error) {
            /* Verify LIFO is full. */
            lifo_error = asl_lifo_get_count_used(&ptr_context->lifo, &used);
            if ((ASL_LIFO_E_OK == lifo_error) && (used == capacity)) {
                printf("  - LIFO is full: PASSED\n");
                
                /* Verify free count is 0. */
                lifo_error = asl_lifo_get_count_free(&ptr_context->lifo, &free_count);
                if ((ASL_LIFO_E_OK == lifo_error) && (0 == free_count)) {
                    printf("  - Free count is 0: PASSED\n");
                    
                    /* Test push to full LIFO (should fail). */
                    memset(test_element, 0xFF, ptr_context->element_size);
                    element_buffer->ptr = test_element;
                    lifo_error = asl_lifo_push(&ptr_context->lifo, *element_buffer);
                    if (ASL_LIFO_E_FULL == lifo_error) {
                        printf("  - Push to full LIFO correctly failed: PASSED\n");
                    } else {
                        printf("  - Push to full LIFO test FAILED (expected 0x%X, got 0x%X)\n", 
                               ASL_LIFO_E_FULL, lifo_error);
                        error = BASIC_LIFO_E_TEST_FAIL;
                    }
                } else {
                    printf("  - Free count check FAILED\n");
                    error = BASIC_LIFO_E_TEST_FAIL;
                }
            } else {
                printf("  - LIFO full check FAILED\n");
                error = BASIC_LIFO_E_TEST_FAIL;
            }
        } else {
            printf("  - Get LIFO capacity for verification FAILED\n");
            error = BASIC_LIFO_E_TEST_FAIL;
        }
    } else {
        error = BASIC_LIFO_E_PARAM;
    }
    
    return error;
}

static basic_lifo_error_e basic_lifo_test_peek_full(
    basic_lifo_context_s* ptr_context,
    uint8_t* peek_element,
    asl_buffer_s* element_buffer) {
    
    basic_lifo_error_e error = BASIC_LIFO_E_OK;
    asl_lifo_error_e lifo_error = ASL_LIFO_E_OK;
    size_t capacity = 0;
    bool data_valid = true;
    
    if ((NULL != ptr_context) && (NULL != peek_element) && (NULL != element_buffer)) {
        /* Get capacity for expected value calculation. */
        lifo_error = asl_lifo_get_count_capacity(&ptr_context->lifo, &capacity);
        if (ASL_LIFO_E_OK == lifo_error) {
            /* Test peek from full LIFO. */
            element_buffer->ptr = peek_element;
            element_buffer->size = ptr_context->element_size;
            lifo_error = asl_lifo_peek(&ptr_context->lifo, element_buffer);
            if (ASL_LIFO_E_OK == lifo_error) {
                /* Verify peeked data matches last pushed element (LIFO order). */
                uint8_t expected_value = (uint8_t)((capacity - 1) & 0xFF);
                data_valid = true;
                for (size_t i = 0; i < ptr_context->element_size; i++) {
                    if (peek_element[i] != expected_value) {
                        data_valid = false;
                        break;
                    }
                }
                
                if (data_valid) {
                    printf("  - Peek from full LIFO data valid: PASSED\n");
                } else {
                    printf("  - Peek data validation FAILED\n");
                    error = BASIC_LIFO_E_TEST_FAIL;
                }
            } else {
                printf("  - Peek from full LIFO FAILED (error 0x%X)\n", lifo_error);
                error = BASIC_LIFO_E_TEST_FAIL;
            }
        } else {
            printf("  - Get LIFO capacity for peek test FAILED\n");
            error = BASIC_LIFO_E_TEST_FAIL;
        }
    } else {
        error = BASIC_LIFO_E_PARAM;
    }
    
    return error;
}

static basic_lifo_error_e basic_lifo_pop_all_elements(
    basic_lifo_context_s* ptr_context,
    uint8_t* peek_element,
    asl_buffer_s* element_buffer) {
    
    basic_lifo_error_e error = BASIC_LIFO_E_OK;
    asl_lifo_error_e lifo_error = ASL_LIFO_E_OK;
    size_t capacity = 0;
    size_t used = 0;
    bool data_valid = true;
    
    if ((NULL != ptr_context) && (NULL != peek_element) && (NULL != element_buffer)) {
        /* Get capacity for loop and validation. */
        lifo_error = asl_lifo_get_count_capacity(&ptr_context->lifo, &capacity);
        if (ASL_LIFO_E_OK == lifo_error) {
            /* Pop all elements and verify data integrity in LIFO order. */
            for (size_t i = capacity; (i > 0) && (BASIC_LIFO_E_OK == error); i--) {
                memset(peek_element, 0, ptr_context->element_size);
                element_buffer->ptr = peek_element;
                element_buffer->size = ptr_context->element_size;
                
                lifo_error = asl_lifo_pop(&ptr_context->lifo, element_buffer);
                if (ASL_LIFO_E_OK == lifo_error) {
                    /* Verify data matches expected pattern (LIFO order). */
                    uint8_t expected_value = (uint8_t)((i - 1) & 0xFF);
                    data_valid = true;
                    for (size_t j = 0; j < ptr_context->element_size; j++) {
                        if (peek_element[j] != expected_value) {
                            data_valid = false;
                            break;
                        }
                    }
                    
                    if (!data_valid) {
                        printf("  - Pop element %zu data mismatch: FAILED\n", (capacity - i));
                        error = BASIC_LIFO_E_TEST_FAIL;
                    }
                } else {
                    printf("  - Pop element %zu FAILED (error 0x%X)\n", (capacity - i), lifo_error);
                    error = BASIC_LIFO_E_TEST_FAIL;
                }
            }
            
            if (BASIC_LIFO_E_OK == error) {
                printf("  - Popped all elements with valid data: PASSED\n");
                
                /* Verify LIFO is empty again. */
                lifo_error = asl_lifo_get_count_used(&ptr_context->lifo, &used);
                if ((ASL_LIFO_E_OK == lifo_error) && (0 == used)) {
                    printf("  - LIFO is empty after pop all: PASSED\n");
                } else {
                    printf("  - LIFO empty check after pop all FAILED\n");
                    error = BASIC_LIFO_E_TEST_FAIL;
                }
            }
        } else {
            printf("  - Get LIFO capacity for pop test FAILED\n");
            error = BASIC_LIFO_E_TEST_FAIL;
        }
    } else {
        error = BASIC_LIFO_E_PARAM;
    }
    
    return error;
}

static basic_lifo_error_e basic_lifo_test_full(basic_lifo_context_s* ptr_context) {
    basic_lifo_error_e error = BASIC_LIFO_E_OK;
    asl_buffer_s element_buffer = {0};
    uint8_t* test_element = NULL;
    uint8_t* peek_element = NULL;
    
    if (NULL != ptr_context) {
        /* Allocate memory for test elements. */
        test_element = malloc(ptr_context->element_size);
        peek_element = malloc(ptr_context->element_size);
        
        if ((NULL != test_element) && (NULL != peek_element)) {
            /* Fill LIFO to capacity. */
            if (BASIC_LIFO_E_OK == error) {
                error = basic_lifo_fill_to_capacity(ptr_context, test_element, &element_buffer);
            }
            
            /* Verify LIFO full state and test boundary conditions. */
            if (BASIC_LIFO_E_OK == error) {
                error = basic_lifo_verify_full_state(ptr_context, test_element, &element_buffer);
            }
            
            /* Test peek from full LIFO. */
            if (BASIC_LIFO_E_OK == error) {
                error = basic_lifo_test_peek_full(ptr_context, peek_element, &element_buffer);
            }
            
            /* Pop all elements and verify data integrity. */
            if (BASIC_LIFO_E_OK == error) {
                error = basic_lifo_pop_all_elements(ptr_context, peek_element, &element_buffer);
            }
            
            /* Clean up. */
            if (NULL != test_element) {
                free(test_element);
            }
            if (NULL != peek_element) {
                free(peek_element);
            }
        } else {
            error = BASIC_LIFO_E_MEMORY;
        }
    } else {
        error = BASIC_LIFO_E_PARAM;
    }
    
    return error;
}
