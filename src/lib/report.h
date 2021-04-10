#pragma once

#include "request.h"

/**
 * @brief print sucessful and failed bookings in format
 * 
 * @param   success list of successful requests
 * @param   fail list of failed requests
 */
void print_booking(request success[], request fail[]);

// print the performance analysis
// TODO
void print_perform(request success[], request fail[]);