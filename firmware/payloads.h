#ifndef PAYLOADS_H_
#define PAYLOADS_H_

#include <stdint.h>
#include "script.h"

/* Return the sequence for a prank + OS combination, or NULL if the
 * combination is not supported. */
const script_evt_t *payload_get(uint32_t prank, uint32_t os);

/* Run every selected prank for every selected target, in a sensible order
 * (shutdown always last). */
void payload_run_all(uint32_t targets, uint32_t pranks);

#endif /* PAYLOADS_H_ */
