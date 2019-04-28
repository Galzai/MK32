#include "keycode_conv.h"
#include "testFunc.h"
#include "plugin_manager.h"
//Test function that prints to serial monitor whatever is typed after pressing enter
void testFunc(void *pvParameters){
	while(1){
		printf(input_string());
		printf("\n");
	}
}
