#include "mbed.h"
#include "rtos.h" //for some reason, this cannot be included in imu.h.  I don't know why, but i put it here it works so ok.
#include "mbed_rpc.h"
#include "comms.h"

char rpc_input_buf[256];
char rpc_output_buf[1024];
//The input and output buffers.


//SerialRPCInterface SerialRPC(USBTX, USBRX, 115200);
MODSERIAL pc(USBTX, USBRX); // tx, rx
//Serial pc(USBTX,USBRX);
Thread* commsThread; //a thread that handles incoming traffice at no more than 10hz, but realistically, less because of the implimention of gets_cr
//for some reason, it's a thread* instead of a thread.  * shrug  *

//copy_paste from 192.  gets when using carriange returns
int gets_cr(MODSERIAL &src, char *s, int max) {
    int counter = 0;
    char c = 0;
    while(c != '\r')//src.readable() && c != '\r') 
    {
        c = src.getc();
        //pc.printf("%c \n\r",c);
        *(s++) = c;
        counter++;
        if (counter == max-1) break;
    }
    //*(s+1) = ' ';
    *(s++) = '\0';

    return counter;
}

void handle_incoming_traffic(void const *n)
{
    while(1) //so this thread doesn't run out of code!
    {
        //pc.gets(rpc_input_buf, 256);
        gets_cr(pc,rpc_input_buf,256); //works around the ctrl-enter thing. nneed to append things with a space...
        //pc.printf("input_buf is %sEND \n\r",rpc_input_buf);

        /*

        char* loc = &rpc_input_buf[0];
        pc.printf("Mem addr is %i \n\r",loc);
        for(int i = 0; i<20; i++)
        {
            pc.printf("%i.",*(loc+i));

        }


        */

        //pc.printf("\n\r");
        
        //for(int i = 0; i<20; i++)
        //{
        //    pc.printf("%c.",*(loc+i));
        //}

        pc.printf("\n\r");
        
        //pc.printf("%i..\n\r",rpc_input_buf);
        
        //pc.printf(rpc_input_buf);
        RPC::call(rpc_input_buf, rpc_output_buf);

        pc.printf("%s \n\r>>>", rpc_output_buf);
        Thread::wait(100);
    }
}

void init_comms()
{
    pc.baud(115200);
    wait_ms(20); //pause for just a bit.
    pc.printf("Hello world! \n\r");

    commsThread = new Thread(handle_incoming_traffic);
}