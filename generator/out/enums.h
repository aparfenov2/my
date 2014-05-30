
#ifndef _ENUMS_H
#define _ENUMS_H
// Генерируем все параметры, определяем их тип
	
	
	
	
namespace dme_sfx_t {
	typedef enum {
			X = 0,
			Y = 1
	} dme_sfx_t;
}
		
		
	
namespace measure_ctl_t {
	typedef enum {
			START = 0,
			STOP = 1
	} measure_ctl_t;
}
	
namespace output_mode_t {
	typedef enum {
			ANT = 0,
			ATTEN = 1,
			CALIB = 2
	} output_mode_t;
}
	
#endif