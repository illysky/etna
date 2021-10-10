/*****************************************************************************************************************************************************************************************
 * @file						
 * @brief: 	
 * @version:			
 * @author: 		 Created by Dominic Moffat on 11/12/2017. Copyright � 2017 Illysky. All rights reserved.										
*****************************************************************************************************************************************************************************************/
#include "gfx.h"
#include "ssd1309.h"

gfx_config_t gfx_instance; 
uint8_t frame_buffer [ (128 * 64) / 8]; 

 /************************************************************************************************************************************//**
 * @function
 * @brief
 ***************************************************************************************************************************************/
void gfx_init (int16_t w, int16_t h, gfx_p_write_disp_t write_disp_func)
{
 	gfx_instance.cursor_x = 0;
	gfx_instance.cursor_y = 0; 
	gfx_instance.wrap = 1; 
	gfx_instance.width = w; 
	gfx_instance.height = h; 
	gfx_instance.rotation = 0; 
	gfx_instance.fontBgColor = GFX_COLOUR_BLACK; 
	gfx_instance.fontColor = GFX_COLOUR_WHITE; 
	gfx_instance.fontSize = 0; 
	gfx_instance.wd_func = write_disp_func; 
 }

  /************************************************************************************************************************************//**
 * @function
 * @brief
 ***************************************************************************************************************************************/
void gfx_update (void) 
{
	gfx_instance.wd_func(frame_buffer,sizeof(frame_buffer)); 
}

/************************************************************************************************************************************//**
 * @function
 * @brief
 ***************************************************************************************************************************************/
void gfx_clear (void) 
{
		memset(frame_buffer, 0, sizeof(frame_buffer)); 
}


 /************************************************************************************************************************************//**
 * @function
 * @brief
 ***************************************************************************************************************************************/ 
void gfx_draw_pixel (int16_t x, int16_t y, uint16_t colour)
{
	
	int16_t tmp; 
	// Return if out of bounds
	if ((x < 0) || (x >= SSD1309_WIDTH) || (y < 0) || (y >= SSD1309_HEIGHT))
    return;

  // check rotation, move pixel around if necessary
  switch (gfx_instance.rotation) {
		case 1:
			tmp = x; 
			x = y; 
			y = tmp; 
			x = SSD1309_WIDTH  - x - 1;
			break;
		case 2:
			x = SSD1309_WIDTH  - x - 1;
			y = SSD1309_HEIGHT - y - 1;
			break;
		case 3:
			tmp = x; 
			x = y; 
			y = tmp; 
			y = SSD1309_HEIGHT - y - 1;
			break;
  }  

  // x is which column
	switch (colour) 
	{
		case GFX_COLOUR_WHITE:   frame_buffer[x+ (y/8)*SSD1309_WIDTH ] |=  (1 << (y&7)); break;
		case GFX_COLOUR_BLACK:   frame_buffer[x+ (y/8)*SSD1309_WIDTH ] &= ~(1 << (y&7)); break; 
		case GFX_COLOUR_INVERSE: frame_buffer[x+ (y/8)*SSD1309_WIDTH ] ^=  (1 << (y&7)); break; 
	}
} 

 /************************************************************************************************************************************//**
 * @function
 * @brief
 ***************************************************************************************************************************************/
void gfx_write_string (char *s, const font_t* font, uint16_t kerning, int16_t x, int16_t y, font_h_alignment_t align_h, font_v_alignment_t align_v)
{
	
	char* s_ptr = s; 
	uint32_t string_width = 0; 
	uint32_t string_height = 0; 
	uint32_t lines = 1; 

	// 1. Calculate max height and width of string
	s_ptr = s; 
	while (*s_ptr) {
		
		// Check the character exists in bitmap
		if (((*s_ptr) >= font->start) && ((*s_ptr) <= font->end)) 
		{
			uint16_t index = ((*s_ptr) - font->start); 																											// Get the char index
			string_width += font->meta[index].width + 1; 																										// Accumulated Width
			if ((font->meta[index].height) > string_height) string_height = font->meta[index].height; 			// Largest Height
		}
		
		// Count lines
		if ((*s_ptr) == '\n') {
			lines++;  
		}
		s_ptr++; 
	}
	
	if (string_width) string_width--; 
	string_height *= lines; 
	
	
	// Offset for horizontal alignment
	if (align_h == FONT_H_ALIGNMENT_CENTER) 			x = x - (string_width/2.0); 
	else if (align_h == FONT_H_ALIGNMENT_RIGHT) 	x = x - (string_width); 
	
	// OFfset for vertical alignment
	if (align_v == FONT_V_ALIGNMENT_MIDDLE) 			y = y - (string_height/2.0); 
	if (align_v == FONT_V_ALIGNMENT_BOTTOM) 			y = y - (string_height); 
	
	
	int16_t cursor_x = x; 
	int16_t cursor_y = y; 
	
	
	// 2. Print string in display buffer
	s_ptr = s; 
	const uint8_t* bitmap = font->bitmap; 
	while (*s_ptr)
	{
		
		// *** Character ***
		if (((*s_ptr) >= font->start) && ((*s_ptr) <= font->end)) 
		{
			uint32_t index = ((*s_ptr) - font->start); 			// Get the char index
			uint32_t width = font->meta[index].width; 			// Get the char width
			uint32_t height = font->meta[index].height;			// Get the char height
			uint32_t offset = font->meta[index].offset;			// Get the char height
			
			// Draw Char
			gfx_draw_char_bitmap (bitmap, offset, width, height, cursor_x, cursor_y, GFX_COLOUR_WHITE, GFX_COLOUR_BLACK); 
			cursor_x += width+1; 
			
			if (kerning) {
				
				//gfx_fill_rect (GFXConfig.cursor_x + w*GFXConfig.fontSize, GFXConfig.cursor_y, GFXConfig.fontKern*GFXConfig.fontSize, h*GFXConfig.fontSize, GFXConfig.fontBgColor);
    	
			}
		}
		
		// *** Line Break ***
		if ((*s_ptr) == '\n') 
		{
			cursor_y += font->meta[0].height;  
			cursor_x  = x;
		} 
		// *** Carriage Return ***
		if ((*s) == '\r') 
		{
			// Ignore
		} 
		// *** Printable ***
		s_ptr++; 
	}

}


 /************************************************************************************************************************************//**
 * @function
 * @brief
 ***************************************************************************************************************************************/
void gfx_draw_char_bitmap (const uint8_t* bitmap, uint16_t offset, uint16_t width, uint16_t height, int16_t x, int16_t y, uint16_t colour, uint16_t bg)
{

	// Clip characters out of screen
	//if(	( x >= gfx_instance.width)	||( y >= gfx_instance.height)||((x + width) < 0)||((y + height) < 0)) return;
	
	
	uint8_t bitCount=0;
	for (int8_t i=0; i<height; i++ ) 
	{				
			uint8_t line;
			for (int8_t j = 0; j<width; j++) 
			{					
					if (bitCount++%8 == 0) 
					{
							line = (*(const unsigned char *)(bitmap+offset++)); 
					}
								
					if (line & 0x80) 
					{
							gfx_draw_pixel(x+j, y+i, colour);
					}
					else if (bg != colour)
					{
							gfx_draw_pixel(x+j, y+i, bg);
					}
					line <<= 1;
			}
			bitCount = 0;
	}
}

