
#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#define init_board(g)          				wiieva_lcd_init_board()
#define post_init_board(g)     				wiieva_lcd_post_init_board()
#define setpin_reset(g, state) 				wiieva_lcd_setpin_reset(state)
#define acquire_bus(g)         				wiieva_lcd_aquirebus()
#define release_bus(g)         				wiieva_lcd_releasebus()
#define write_cmd(g, cmd)      				wiieva_lcd_write_cmd(cmd)
#define write_data(g, data)    				wiieva_lcd_write_data(data)
#define write_data_byte(g, data)    		wiieva_lcd_write_data_byte (data)
#define write_data_repeat(g, data, count) 	wiieva_lcd_write_data_repeat(data, count)
#define set_backlight(g, percent)			wiieva_lcd_set_backlight (percent)

#define setreadmode(g)
#define setwritemode(g)
#define read_data(g) 0

#ifdef __cplusplus
extern "C" {
#endif

void wiieva_lcd_init_board(void);
void wiieva_lcd_post_init_board(void);
void wiieva_lcd_setpin_reset(int state);
void wiieva_lcd_aquirebus(void);
void wiieva_lcd_releasebus(void);
void wiieva_lcd_write_cmd(uint8_t cmd);
void wiieva_lcd_write_data(uint16_t data);
void wiieva_lcd_write_data_byte (uint8_t data);
void wiieva_lcd_write_data_repeat (uint16_t data,int cnt);
void wiieva_lcd_write_data_buf (uint16_t *data,int cnt);
void wiieva_lcd_set_backlight (uint16_t percent);

#ifdef __cplusplus
}
#endif



#endif /* _GDISP_LLD_BOARD_H */
