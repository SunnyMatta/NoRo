#ifndef GLOBAL_H
#define GLOBAL_H

#include <GL/gl.h>

/*
 * For loading DDS format 
 * 
 * That's it... i mean it also support DXT1/3/5
 */
GLuint loaddds(const char* file);

/*
 * Converting file information into string
 *
 * Mainly, it being used for reading shader files
 */

char* filetostring(const char* filepath);

#endif