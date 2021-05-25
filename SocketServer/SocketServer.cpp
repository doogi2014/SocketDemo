
#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <synchapi.h>

#include "zh_socket.h"

int client_connected_callback()
{
    printf("do something when a new client connected.\r\n" );
    return 0;
}

int main()
{
    printf("[ESC] to exit\n");
    zh_socket server_socket;
    server_socket.start("0.0.0.0", 9999, client_connected_callback);

    while (_getch() != 27)
    {
        char buf[] = { 0x38,0x38,0x38,0x38 };
        server_socket.send_data(buf, 4);
        Sleep(1000);
    }
        
    server_socket.stop();
    printf("main stop.\n");
}

