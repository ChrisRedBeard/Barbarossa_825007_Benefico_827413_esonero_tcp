/*
 * protocol.h
 *
 * Server header file
 * Definitions, constants and function prototypes for the server
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

// Shared application parameters
#define SERVER_PORT 27015  // Server port (change if needed)
#define BUFFER_SIZE 512    // Buffer size for messages
#define QUEUE_SIZE 5       // Size of pending connections queue


typedef struct {
    unsigned int status;  // Response status code
    char type;            // Echo of request type
    float value;          // Weather data value
} weather_response_t;

// Function prototypes
// Add here the signatures of the functions implemented by students
float get_temperature(void);    // Range: -10.0 to 40.0 °C
float get_humidity(void);       // Range: 20.0 to 100.0 %
float get_wind(void);           // Range: 0.0 to 100.0 km/h
float get_pressure(void);       // Range: 950.0 to 1050.0 hPa


#endif /* PROTOCOL_H_ */
