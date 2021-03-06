#pragma once

#include "request.h"

/**
 * @brief print sucessful and failed bookings in format
 * 
 * @param   success list of successful requests
 * @param   fail list of failed requests
 */
void print_booking(request *success[], request *fail[], char *algo);


/**
 * @brief print the performance analysis
 * 
 * @param   success list of successful requests
 * @param   fail list of failed requests
 */
void print_perform(request *success[], request *fail[], char *algo);
