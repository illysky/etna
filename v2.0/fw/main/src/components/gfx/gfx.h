/*****************************************************************************************************************************************************************************************
 * @file						
 * @brief: 	
 * @version:			
 * @author: 		 Created by Dominic Moffat on 11/12/2017. Copyright � 2017 Illysky. All rights reserved.										
*****************************************************************************************************************************************************************************************/
#ifndef __GFX_H
#define __GFX_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define GFX_COLOUR_BLACK 0
#define GFX_COLOUR_WHITE   1 
#define GFX_COLOUR_INVERSE 2

typedef void (*gfx_p_write_disp_t)(uint8_t* buf, uint32_t size);

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define swap(a, b) { int16_t t = a; a = b; b = t; }

typedef struct {
	uint8_t width; 
	uint8_t height; 
	uint16_t offset;
} font_meta_t; 

typedef struct {
	const uint8_t height; 
	const uint8_t start; 
	const uint8_t end;
	const font_meta_t *meta; 
	const uint8_t *bitmap;
} font_t;


typedef enum {
	FONT_H_ALIGNMENT_RIGHT = 0, 
	FONT_H_ALIGNMENT_CENTER = 1, 
	FONT_H_ALIGNMENT_LEFT = 2, 
} font_h_alignment_t; 

typedef enum {
	FONT_V_ALIGNMENT_TOP = 0, 
	FONT_V_ALIGNMENT_MIDDLE = 1, 
	FONT_V_ALIGNMENT_BOTTOM = 2, 
} font_v_alignment_t; 

typedef struct {
	int16_t 	width; 
	int16_t	 	height; 
	uint32_t 	rotation; 
	int16_t 	cursor_x; 
	int16_t 	cursor_y;
	uint32_t 	fontSize; 
	uint16_t 	fontColor; 
	uint16_t 	fontBgColor; 
	uint8_t 	fontKern;
	bool 		wrap;
	gfx_p_write_disp_t wd_func; 
} gfx_config_t; 

extern gfx_config_t gfx_conf; 

void gfx_init (int16_t w, int16_t h, gfx_p_write_disp_t write_disp_func);
void gfx_clear (void); 
void gfx_update (void); 
void gfx_draw_bitmap (int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h,uint16_t colour);
void gfx_write_string (char *s, const font_t* font, uint16_t kerning, int16_t x, int16_t y, font_h_alignment_t align_h, font_v_alignment_t align_v); 
void gfx_draw_char_bitmap (const uint8_t* bitmap, uint16_t offset, uint16_t width, uint16_t height, int16_t x, int16_t y, uint16_t colour, uint16_t bg);
void gfx_draw_pixel (int16_t x, int16_t y, uint16_t colour); 


// Font data for Microsoft Sans Serif 8pt
extern const uint8_t microsoftSansSerif_8ptBitmaps[];
extern const font_t microsoftSansSerif_8ptFontInfo;
extern const font_meta_t microsoftSansSerif_8ptDescriptors[];


// Font data for Microsoft Tai Le 24pt
extern const uint8_t microsoftTaiLe_24ptBitmaps[];
extern const font_t microsoftTaiLe_24ptFontInfo;
extern const font_meta_t microsoftTaiLe_24ptDescriptors[];

// Font data for Open Sans 10pt
extern const uint8_t openSans_10ptBitmaps[];
extern const font_t openSans_10ptFontInfo;
extern const font_meta_t openSans_10ptDescriptors[];

// Font data for Segoe UI 10pt
extern const uint8_t segoeUI_10ptBitmaps[];
extern const font_t segoeUI_10ptFontInfo;
extern const font_meta_t segoeUI_10ptDescriptors[];

#ifdef __cplusplus
}
#endif
#endif




