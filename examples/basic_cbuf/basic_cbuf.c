/*******************************************************************************
 *  @file       basic_cbuf.c
 *
 *  @copyright  Accolade Electronics Pvt Ltd, 2025-26
 *              All Rights Reserved
 *              UNPUBLISHED, LICENSED SOFTWARE.
 *              Accolade Electronics, Pune
 *              CONFIDENTIAL AND PROPRIETARY INFORMATION
 *              WHICH IS THE PROPERTY OF M/s Accolade Electronics.
 *
 *  @version    0.0.1
 *
 *  @date       27 October 2025
 *
 *  @brief      Implementation of the 'basic_cbuf' Example.
*******************************************************************************/

/* Self. */
#include "basic_cbuf.h"
#include "basic_cbuf_priv.h"

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

basic_cbuf_error_e basic_cbuf_init(basic_cbuf_context_s* ptr_context, size_t cbuf_size) {
    basic_cbuf_error_e error = BASIC_CBUF_E_OK;
    asl_cbuf_error_e cbuf_error = ASL_CBUF_E_OK;
    size_t available_write = 0;
    
    if ((NULL != ptr_context) && (0 != cbuf_size)) {
        /* Clear context. */
        memset(ptr_context, 0, sizeof(basic_cbuf_context_s));
        
        /* Allocate buffer memory. */
        ptr_context->buffer_memory = malloc(cbuf_size);
        if (NULL != ptr_context->buffer_memory) {
            /* Setup CBUF structure. */
            ptr_context->cbuf.mem = ptr_context->buffer_memory;
            ptr_context->cbuf.size_mem = cbuf_size;
            ptr_context->cbuf.front = 0;
            ptr_context->cbuf.rear = 0;
            ptr_context->buffer_size = cbuf_size;
            
            /* Initialize ASL CBUF. */
            cbuf_error = asl_cbuf__init(&ptr_context->cbuf);
            if (ASL_CBUF_E_OK == cbuf_error) {
                /* Determine maximum usable capacity. */
                cbuf_error = asl_cbuf__available_write(&ptr_context->cbuf, &available_write);
                if (ASL_CBUF_E_OK == cbuf_error) {
                    ptr_context->max_capacity = available_write;
                    ptr_context->test_complete = false;
                    ptr_context->test_passed = false;
                    ptr_context->current_test_size = 0;
                    ptr_context->successful_tests = 0;
                    ptr_context->failed_tests = 0;
                    printf("CBUF initialized: buffer size %zu bytes, max capacity %zu bytes\n", 
                           cbuf_size, ptr_context->max_capacity);
                } else {
                    free(ptr_context->buffer_memory);
                    ptr_context->buffer_memory = NULL;
                    error = BASIC_CBUF_E_MEMORY;
                }
            } else {
                free(ptr_context->buffer_memory);
                ptr_context->buffer_memory = NULL;
                error = BASIC_CBUF_E_MEMORY;
            }
        } else {
            error = BASIC_CBUF_E_MEMORY;
        }
    } else {
        error = BASIC_CBUF_E_PARAM;
    }
    
    return error;
}

basic_cbuf_error_e basic_cbuf_run_test(basic_cbuf_context_s* ptr_context) {
    basic_cbuf_error_e error = BASIC_CBUF_E_OK;
    int pthread_error = 0;
    bool producer_created = false;
    bool consumer_created = false;
    
    if (NULL != ptr_context) {
        printf("Starting comprehensive test: sizes 1 to %zu bytes...\n", ptr_context->max_capacity);
        
        /* Test all sizes from 1 to maximum capacity. */
        for (size_t test_size = 1; test_size <= ptr_context->max_capacity; test_size++) {
            /* Reset CBUF for each test. */
            asl_cbuf_error_e cbuf_error = asl_cbuf__init(&ptr_context->cbuf);
            if (ASL_CBUF_E_OK != cbuf_error) {
                printf("ERROR: Failed to reinitialize CBUF for test size %zu\n", test_size);
                error = BASIC_CBUF_E_TEST_FAIL;
                break;
            }
            
            ptr_context->current_test_size = test_size;
            ptr_context->test_complete = false;
            producer_created = false;
            consumer_created = false;
            
            /* Create producer thread. */
            pthread_error = pthread_create(&ptr_context->producer_thread, NULL, 
                                           basic_cbuf_producer_thread, ptr_context);
            if (0 == pthread_error) {
                producer_created = true;
                
                /* Create consumer thread. */
                pthread_error = pthread_create(&ptr_context->consumer_thread, NULL, 
                                               basic_cbuf_consumer_thread, ptr_context);
                if (0 == pthread_error) {
                    consumer_created = true;
                    
                    /* Wait for threads to complete. */
                    pthread_join(ptr_context->producer_thread, NULL);
                    pthread_join(ptr_context->consumer_thread, NULL);
                    
                    /* Check test result. */
                    if (ptr_context->test_passed) {
                        ptr_context->successful_tests++;
                        if ((test_size % 100) == 0 || test_size <= 10 || test_size > (ptr_context->max_capacity - 10)) {
                            printf("Test size %zu: PASSED\n", test_size);
                        }
                    } else {
                        ptr_context->failed_tests++;
                        printf("Test size %zu: FAILED\n", test_size);
                        error = BASIC_CBUF_E_TEST_FAIL;
                        break;
                    }
                } else {
                    printf("ERROR: Failed to create consumer thread for test size %zu\n", test_size);
                    error = BASIC_CBUF_E_TEST_FAIL;
                    break;
                }
                
                /* Cleanup producer thread if consumer failed. */
                if (!consumer_created && producer_created) {
                    ptr_context->test_complete = true;
                    pthread_join(ptr_context->producer_thread, NULL);
                }
            } else {
                printf("ERROR: Failed to create producer thread for test size %zu\n", test_size);
                error = BASIC_CBUF_E_TEST_FAIL;
                break;
            }
        }
        
        /* Set overall test result. */
        if (BASIC_CBUF_E_OK == error && 0 == ptr_context->failed_tests) {
            ptr_context->test_passed = true;
            printf("All tests PASSED!\n");
        } else {
            ptr_context->test_passed = false;
            printf("Tests FAILED!\n");
        }
    } else {
        error = BASIC_CBUF_E_PARAM;
    }
    
    return error;
}

basic_cbuf_error_e basic_cbuf_cleanup(basic_cbuf_context_s* ptr_context) {
    basic_cbuf_error_e error = BASIC_CBUF_E_OK;
    
    if (NULL != ptr_context) {
        /* Free allocated memory. */
        if (ptr_context->buffer_memory) {
            free(ptr_context->buffer_memory);
            ptr_context->buffer_memory = NULL;
        }
        
        /* Clear context. */
        memset(ptr_context, 0, sizeof(basic_cbuf_context_s));
    } else {
        error = BASIC_CBUF_E_PARAM;
    }
    
    return error;
}

void basic_cbuf_print_usage(const char* program_name) {
    printf("Usage: %s [buffer_size]\n", program_name);
    printf("  buffer_size: Size of circular buffer (default: %d bytes)\n", BASIC_CBUF_DEFAULT_SIZE);
}

void basic_cbuf_print_results(basic_cbuf_context_s* ptr_context) {
    if (NULL == ptr_context) {
        return;
    }
    
    printf("\nTest Results:\n");
    printf("Buffer Size: %zu bytes\n", ptr_context->buffer_size);
    printf("Max Capacity: %zu bytes\n", ptr_context->max_capacity);
    printf("Tests Executed: %zu\n", ptr_context->successful_tests + ptr_context->failed_tests);
    printf("Successful Tests: %zu\n", ptr_context->successful_tests);
    printf("Failed Tests: %zu\n", ptr_context->failed_tests);
    printf("Overall Status: %s\n", ptr_context->test_passed ? "PASSED" : "FAILED");
}

/* Private function implementations */

static void* basic_cbuf_producer_thread(void* arg) {
    basic_cbuf_context_s* context = (basic_cbuf_context_s*)arg;
    asl_buffer_s data_buffer = {0};
    uint8_t* test_data = NULL;
    asl_cbuf_error_e cbuf_error = ASL_CBUF_E_OK;
    size_t test_size = context->current_test_size;
    
    /* Allocate memory for test data. */
    test_data = malloc(test_size);
    if (NULL == test_data) {
        printf("Producer ERROR: Failed to allocate memory for test size %zu\n", test_size);
        return NULL;
    }
    
    /* Initialize test data pattern. */
    for ( size_t i = 0 ; i < test_size ; i++ ) {
        test_data[i] = (uint8_t)(i & 0xFF);
    }
    
    data_buffer.ptr = test_data;
    data_buffer.size = test_size;
    
    /* Enqueue test data. */
    cbuf_error = asl_cbuf__enqueue(&context->cbuf, data_buffer, test_size);
    
    if (ASL_CBUF_E_OK == cbuf_error) {
        if (test_size <= 10 || (test_size % 100) == 0 || test_size > (context->max_capacity - 10)) {
            printf("Producer: Enqueued %zu bytes successfully\n", test_size);
        }
    } else {
        printf("Producer ERROR: Enqueue failed for size %zu with error 0x%08X\n", test_size, cbuf_error);
    }
    
    /* Free allocated memory. */
    free(test_data);
    return NULL;
}

static void* basic_cbuf_consumer_thread(void* arg) {
    basic_cbuf_context_s* context = (basic_cbuf_context_s*)arg;
    asl_buffer_s data_buffer = {0};
    uint8_t* received_data = NULL;
    bool data_valid = true;
    asl_cbuf_error_e cbuf_error = ASL_CBUF_E_OK;
    size_t test_size = context->current_test_size;
    
    /* Wait a moment for producer to add data. */
    usleep(BASIC_CBUF_SLEEP_US);
    
    /* Allocate memory for received data. */
    received_data = malloc(test_size);
    if (NULL == received_data) {
        printf("Consumer ERROR: Failed to allocate memory for test size %zu\n", test_size);
        context->test_passed = false;
        context->test_complete = true;
        return NULL;
    }
    
    data_buffer.ptr = received_data;
    data_buffer.size = test_size;
    
    /* Dequeue test data. */
    cbuf_error = asl_cbuf__dequeue(&context->cbuf, data_buffer, test_size);
    
    if (ASL_CBUF_E_OK == cbuf_error) {
        if (test_size <= 10 || (test_size % 100) == 0 || test_size > (context->max_capacity - 10)) {
            printf("Consumer: Dequeued %zu bytes successfully\n", test_size);
        }
        
        /* Validate received data. */
        for ( size_t i = 0 ; i < test_size ; i++ ) {
            if (received_data[i] != (uint8_t)(i & 0xFF)) {
                printf("Consumer ERROR: Data validation failed at index %zu for size %zu (expected 0x%02X, got 0x%02X)\n", 
                       i, test_size, (uint8_t)(i & 0xFF), received_data[i]);
                data_valid = false;
                break;
            }
        }
        
        if (data_valid) {
            if (test_size <= 10 || (test_size % 100) == 0 || test_size > (context->max_capacity - 10)) {
                printf("Consumer: Data validation PASSED for size %zu\n", test_size);
            }
            context->test_passed = true;
        } else {
            printf("Consumer ERROR: Data validation FAILED for size %zu\n", test_size);
            context->test_passed = false;
        }
    } else {
        printf("Consumer ERROR: Dequeue failed for size %zu with error 0x%08X\n", test_size, cbuf_error);
        context->test_passed = false;
    }
    
    context->test_complete = true;
    
    /* Free allocated memory. */
    free(received_data);
    return NULL;
}
