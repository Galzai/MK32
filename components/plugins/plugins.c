#include "plugins.h"

//We create tasks for the plugins here
void plugin_launcher(uint16_t keycode){

    switch(keycode){
        case PN_CLOSE:
            close_plugin_task();
            break;

        case PN_LAYOUT:
            start_plugin_task(http_server, 1, 0);
            break;

        case PN_TEST:
            start_plugin_task(testFunc, 0, 0);
            break;

        default:
            break;
    }



}
