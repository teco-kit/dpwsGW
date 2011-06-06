/*
 * discovery_helper.h
 *
 *  Created on: 13.08.2008
 *      Author: elmex
 */

#ifndef DISCOVERY_HELPER_H_
#define DISCOVERY_HELPER_H_

int discovery_helper_start (struct dpws_s *dpws);
int discovery_helper_stop ();
void discovery_helper_kill ();

typedef int (*discovery_helper_addDevice) (struct ws4d_epr * device);
typedef int (*discovery_helper_invalidateDevice) (struct ws4d_epr * device);
int discovery_helper_addDeviceCB (discovery_helper_addDevice cb);
int discovery_helper_invalidateDeviceCB (discovery_helper_invalidateDevice
                                         cb);

#endif /* DISCOVERY_HELPER_H_ */
