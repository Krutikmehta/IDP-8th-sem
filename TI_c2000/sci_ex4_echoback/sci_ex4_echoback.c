//#############################################################################
//
// FILE:   sci_ex4_echoback.c
//
// TITLE:  SCI echoback example.
//
//! \addtogroup driver_example_list
//! <h1>SCI Echoback</h1>
//!
//!  This test receives and echo-backs data through the SCI-A port.
//!
//!  A terminal such as 'putty' can be used to view the data from
//!  the SCI and to send information to the SCI. Characters received
//!  by the SCI port are sent back to the host.
//!
//!  \b Running \b the \b Application
//!  Open a COM port with the following settings using a terminal:
//!  -  Find correct COM port
//!  -  Bits per second = 9600
//!  -  Data Bits = 8
//!  -  Parity = None
//!  -  Stop Bits = 1
//!  -  Hardware Control = None
//!
//!  The program will print out a greeting and then ask you to
//!  enter a character which it will echo back to the terminal.
//!
//!  \b Watch \b Variables \n
//!  - loopCounter - the number of characters sent
//!
//! \b External \b Connections \n
//!  Connect the SCI-A port to a PC via a transceiver and cable.
//!  - GPIO28 is SCI_A-RXD (Connect to Pin3, PC-TX, of serial DB9 cable)
//!  - GPIO29 is SCI_A-TXD (Connect to Pin2, PC-RX, of serial DB9 cable)
//
//#############################################################################
//
//
// $Copyright:
// Copyright (C) 2021 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include "stdio.h"
#include "string.h"

//
// Defines
//
// Define AUTOBAUD to use the autobaud lock feature
//#define AUTOBAUD

//
// Globals
//
uint16_t loopCounter = 0;

uint16_t SCI_readIntBlockingFIFO(){
    int i = 0;
    unsigned char bytes[2];
    uint16_t rxStatus = 0U;
    for(; i<2; i++){
        bytes[i] = SCI_readCharBlockingFIFO(SCIA_BASE);
        rxStatus = SCI_getRxStatus(SCIA_BASE);
        if((rxStatus & SCI_RXSTATUS_ERROR) != 0){
            ESTOP0;
        }
        SCI_readCharBlockingFIFO(SCIA_BASE);
    }
    uint16_t myInt1 = bytes[0] + (bytes[1] << 8);
    unsigned char msg[2];
//    itoa(myInt1, msg, 10);
    SCI_writeCharArray(SCIA_BASE, (uint16_t*)msg, 2);

    return myInt1;

}

uint16_t myAtoi(unsigned char* str)
{
    // Initialize result
    uint16_t res = 0;
    int i=0;
    // Iterate through all characters
    // of input string and update result
    // take ASCII character of corresponding digit and
    // subtract the code from '0' to get numerical
    // value and multiply res by 10 to shuffle
    // digits left to update running total

    for(; str[i] != '\0'; ++i){
        res = res * 10 + str[i] - '0';
    }

    // return result.
    return res;
}

char* appendCharToCharArray(char* array, char a)
{
    size_t len = strlen(array);
    char* ret = malloc((len+2)*sizeof(*ret));
    strcpy(ret, array);
    ret[len] = a;
    ret[len+1] = '\0';

    return ret;
}
//
// Main
//
void main(void)
{
//    printf("hello here/n");
//    uint16_t receivedChar;
    unsigned char *msg;
    uint16_t rxStatus = 0U;

    //
    // Configure PLL, disable WD, enable peripheral clocks.
    //
    Device_init();

    //
    // Disable pin locks and enable internal pullups.
    //
    Device_initGPIO();

    //
    // GPIO28 is the SCI Rx pin.
    //
    GPIO_setMasterCore(DEVICE_GPIO_PIN_SCIRXDA, GPIO_CORE_CPU1);
    GPIO_setPinConfig(DEVICE_GPIO_CFG_SCIRXDA);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_SCIRXDA, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_SCIRXDA, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_SCIRXDA, GPIO_QUAL_ASYNC);

    //
    // GPIO29 is the SCI Tx pin.
    //
    GPIO_setMasterCore(DEVICE_GPIO_PIN_SCITXDA, GPIO_CORE_CPU1);
    GPIO_setPinConfig(DEVICE_GPIO_CFG_SCITXDA);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_SCITXDA, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_SCITXDA, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_SCITXDA, GPIO_QUAL_ASYNC);

    //
    // Initialize interrupt controller and vector table.
    //
    Interrupt_initModule();
    Interrupt_initVectorTable();

    //
    // Initialize SCIA and its FIFO.
    //
    SCI_performSoftwareReset(SCIA_BASE);

    //
    // Configure SCIA for echoback.
    //
    SCI_setConfig(SCIA_BASE, DEVICE_LSPCLK_FREQ, 115200, (SCI_CONFIG_WLEN_8 |
                                                        SCI_CONFIG_STOP_ONE |
                                                        SCI_CONFIG_PAR_NONE));
    SCI_resetChannels(SCIA_BASE);
    SCI_resetRxFIFO(SCIA_BASE);
    SCI_resetTxFIFO(SCIA_BASE);
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF | SCI_INT_RXFF);
    SCI_enableFIFO(SCIA_BASE);
    SCI_enableModule(SCIA_BASE);
    SCI_performSoftwareReset(SCIA_BASE);

#ifdef AUTOBAUD
    //
    // Perform an autobaud lock.
    // SCI expects an 'a' or 'A' to lock the baud rate.
    //
    SCI_lockAutobaud(SCIA_BASE);
#endif

    //
    // Send starting message.
    //
//    msg = "\r\n\n\nHello World!\0";
//    SCI_writeCharArray(SCIA_BASE, (uint16_t*)msg, 17);
//    msg = "\r\nYou will enter a character, and the DSP will echo it back!\n\0";
//    SCI_writeCharArray(SCIA_BASE, (uint16_t*)msg, 62);

    for(;;)
    {
//        msg = "\r\nEnter a character: \0";
//        SCI_writeCharArray(SCIA_BASE, (uint16_t*)msg, 22);

        //
        // Read a character from the FIFO.
        //
//        receivedChar = SCI_readCharBlockingFIFO(SCIA_BASE);

//        rxStatus = SCI_getRxStatus(SCIA_BASE);
////        if((rxStatus & SCI_RXSTATUS_ERROR) != 0)
////        {
////            //
////            //If Execution stops here there is some error
////            //Analyze SCI_getRxStatus() API return value
////            //
////            ESTOP0;
////        }
//
//        //
//        // Echo back the character.
//        //
        msg = "  You sent: \0";
//        SCI_writeCharArray(SCIA_BASE, (uint16_t*)msg, 13);
//        receivedChar = SCI_readCharBlockingFIFO(SCIA_BASE);

//        return((uint16_t)(HWREGH(base + SCI_O_RXBUF) & SCI_RXBUF_SAR_M));
//        SCI_writeCharBlockingFIFO(SCIA_BASE, receivedChar);
//        SCI_readIntBlockingFIFO();
//        unsigned char *rxd_int;
//        unsigned char bytes[2];
        uint16_t n = 100;

//        bytes[0] = (n >> 8) & 0xFF;
//        bytes[1] = (n >> 0) & 0xFF;
//        bytes[0] = SCI_readCharBlockingFIFO(SCIA_BASE);
//        bytes[1] = SCI_readCharBlockingFIFO(SCIA_BASE);

//        uint16_t val = bytes[1] + (bytes[0] << 8);
//        SCI_writeCharArray(SCIA_BASE,(uint16_t*)bytes,2);
//        SCI_writeCharArray(SCIA_BASE,(uint16_t*)val,2);


           unsigned char *rxd_msg;
           SCI_readCharArray(SCIA_BASE,(uint16_t*)rxd_msg,n);
           SCI_writeCharArray(SCIA_BASE,(uint16_t*)rxd_msg,n);

//           unsigned char *tx_msg;
//           SCI_writeCharArray(SCIA_BASE,(uint16_t*)tx_msg,40);


//        unsigned char* ret = "\0";
//        char ch = SCI_readCharNonBlocking(SCIA_BASE);
//        int i=0;
//        while(ch!='\0'){
//            i++;
//            ret = appendCharToCharArray(ret,ch);
//            ch = SCI_readCharNonBlocking(SCIA_BASE);
//            SCI_writeCharNonBlocking(SCIA_BASE,ch);
//        }
//        SCI_writeCharArray(SCIA_BASE,(uint16_t*)ret,i);

        //
        // Increment the loop count variable.
        //
        loopCounter++;
    }
}

//
// End of File
//

