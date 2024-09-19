#ifndef _INITSTORYSPLASH_HPP_
#define _INITSTORYSPLASH_HPP_

#include <kos.h>



extern pvr_ptr_t font_tex;
extern pvr_ptr_t back_tex;
extern pvr_ptr_t bsg_back_tex;


void back_init(void);
void bsg_back_init(void);
void initStorySplash();
void initSplash();

#endif