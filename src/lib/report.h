#pragma once

// NOTE: all functions are already moved to main.c
// this file along with .c file are kept for future debugging needs
#ifdef _REPORT_DEBUG_ALONE
#include "request.h"

/**
 * @brief print sucessful and failed bookings in format
 * 
 * @param   success list of successful requests
 * @param   fail list of failed requests
 */
void print_booking(request success[], request fail[]);


/**
 * @brief print the performance analysis
 * 
 * @param   success list of successful requests
 * @param   fail list of failed requests
 */
void print_perform(request success[], request fail[]);
#endif