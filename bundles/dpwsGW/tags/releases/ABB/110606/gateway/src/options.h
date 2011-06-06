/*
 * options.h
 *
 *  Created on: 30.05.2011
 *      Author: wildschut
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

/**
 * Process the general section of the ini file
 */
int processGeneralSection(char * path);

/**
 * Process the platform-specific section of the ini file
 */
int processPlatformSection(char * path);

#endif /* OPTIONS_H_ */
