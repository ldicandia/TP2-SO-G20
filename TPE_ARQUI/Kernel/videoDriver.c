#include <videoDriver.h>

struct vbe_mode_info_structure {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;
 
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
 
	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed));

//donde arranca la pantalla
uint16_t cursorX = 0;
uint16_t cursorY = 0;
uint8_t charSize = 1;

typedef struct vbe_mode_info_structure * VBEInfoPtr;

VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00;

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y) {
    uint8_t * framebuffer = (uint8_t *) VBE_mode_info->framebuffer;
    uint64_t offset = (x * ((VBE_mode_info->bpp)/8)) + (y * VBE_mode_info->pitch);
    framebuffer[offset]     =  (hexColor) & 0xFF;
    framebuffer[offset+1]   =  (hexColor >> 8) & 0xFF; 
    framebuffer[offset+2]   =  (hexColor >> 16) & 0xFF;
}


void driver_printChar(char c, Color color){
	switch (c) {
        case '\n':
            driver_newLine();
        break;
        case '\b':
            driver_backspace();
        break;
        case '\0':
            /* nada, no imprime nada */
        break;
        default:
            drawChar(c, color);
        break;
    }
}

void driver_backspace(){
    Color BLACK = {0,0,0};
    if(cursorX > 0){
        cursorX -= 8*charSize;
    }
    else{
        //paso al final de la linea anterior
        //la idea es ver cuantas letras entraron con este tamanio e ir a la ultima de ellas
        cursorX = (VBE_mode_info->width%(8*charSize))*8;
        cursorY -= 16*charSize;
    }

    for(int x= 0; x< 8*charSize; x++){
        for(int y= 0; y<16*charSize; y++){
            putPixel(0x000000,cursorX + x, cursorY + y);
        }
    }
}



void drawChar(char letter, Color color){
	if(cursorX >= VBE_mode_info->width ){
		//pasa a la siguiente linea desde la primera pos a la izquierda
		cursorX = 0;
	    cursorY += 16*charSize;
	}
	if(cursorY > VBE_mode_info->height){
		cursorY -= 16*charSize;
        scroll();
	}
	uint8_t *pos = getFontChar(letter);
    for(int x = 0; x < 8; x++){
        for(int y = 0; y < 16; y++){
            if (pos[y] & (1 << (7 - x))) {
                for(int sx = 0; sx < charSize; sx++) {
                    for(int sy = 0; sy < charSize; sy++) {
                        putPixel(rgbToHex(color.r, color.g, color.b), cursorX + x * charSize + sx, cursorY + y * charSize + sy);
                    }
                }
            }
        }
    }
    cursorX += 8 * charSize;
}

void driver_newLine(){
    Color WHITE = {255,255,255};
	cursorX = 0;
	cursorY += 16*charSize;
    drawChar('>', WHITE);
}

uint32_t rgbToHex(uint8_t r, uint8_t g, uint8_t b) {
    return (r << 16) | (g << 8) | b;
}


void driver_printStr(char * str, Color color){
	for(int i=0; str[i]; i++){
		driver_printChar(str[i], color);
	}
}

void scroll(){
	int nextX, nextY;
    for(int x = 0; x < VBE_mode_info->width; x++){
        for(int y = 0; y < VBE_mode_info->height - 16 * charSize; y++){
            //Obtiene el color del píxel de la siguiente línea.
            uint8_t *framebuffer = (uint8_t *) VBE_mode_info->framebuffer;
            uint64_t nextOffset = x * (VBE_mode_info->bpp/8) + (y + 16 * charSize) * VBE_mode_info->pitch;
            uint32_t nextPixelColor = framebuffer[nextOffset] |
                                      (framebuffer[nextOffset + 1] << 8) |
                                      (framebuffer[nextOffset + 2] << 16);

            // Pone el color en la posición actual.
            putPixel(nextPixelColor, x, y);
        }
    }

    // Borra la última línea (deja en negro)
    for(int x = 0; x < VBE_mode_info->width; x++){
        for(int y = VBE_mode_info->height - 16 * charSize; y < VBE_mode_info->height; y++){
            putPixel(0x000000, x, y); // Borra con color negro (RGB: 0,0,0).
        }
    }
}
