/***********************************************************************
    Mark Butcher    Bsc (Hons) MPhil MIET
    
    M.J.Butcher Consulting
    Birchstrasse 20f,    CH-5406, Rütihof
    Switzerland
    
    www.uTasker.com    Skype: M_J_Butcher
    
    ---------------------------------------------------------------------
    File:      FreeRTOSapplication.c
    Project:   uTasker project
    ---------------------------------------------------------------------
    Copyright (C) M.J.Butcher Consulting 2004..2018
    *********************************************************************
*/

#define _FREE_RTOS_APPLICATION
#include "config.h"
#if defined RUN_IN_FREE_RTOS
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

extern void fn_uTasker_main(void *);
#if defined FREE_RTOS_USB
    extern unsigned char fnGetUSB_Handle(void);
    static void usb_task(void *pvParameters);
#endif
#if defined FREE_RTOS_UART
    extern unsigned char fnGetUART_Handle(void);
    static void uart_task(void *pvParameters);
#endif
#if defined FREE_RTOS_BLINKY
    extern void fnInitialiseRedLED(void);
    extern void fnToggleRedLED(void);
    static void blinky(void *par);
#endif
static void prvSetupHardware(void);


extern void fnFreeRTOS_main(void)
{
    prvSetupHardware();                                                  // perform any hardware setup necessary (that the uTasker initialisation hasn't done)

    // Add the uTasker OS and project as a single task under FreeRTOS
    //
    if (xTaskCreate(                                                     // start uTasker as a task in the FreeRTOS task environment
        fn_uTasker_main,                                                 // pointer to the task
        "uTasker",                                                       // task name for kernel awareness debugging
        (UTASKER_STACK_SIZE / sizeof(portSTACK_TYPE)),                   // task stack size
        (void *)0,                                                       // optional task startup argument
        (UTASKER_TASK_PRIORITY),                                         // initial priority
        0                                                                // optional task handle to create
    ) != pdPASS) {
        _EXCEPTION("FreeRTOS failed to initialise task");
        return;                                                          // this only happens when there was a failure to initialise the task (usually not enough heap)
    }

    // Add further user tasks here
    //
    #if defined FREE_RTOS_BLINKY
    if (xTaskCreate(                                                     // FreeRTOS blinky
        blinky,                                                          // pointer to the task
        "Blinky",                                                        // task name for kernel awareness debugging
        configMINIMAL_STACK_SIZE,                                        // task stack size
        (void*)NULL,                                                     // optional task startup argument
        tskIDLE_PRIORITY,                                                // initial priority
        NULL
        ) != pdPASS) {
        _EXCEPTION("FreeRTOS failed to initialise blinky task");
        return;                                                          // this only happens when there was a failure to initialise the task (usually not enough heap)
    }
    #endif
    #if defined FREE_RTOS_USB
    if (xTaskCreate(                                                     // FreeRTOS USB task
        usb_task,                                                        // pointer to the task
        "usb_task",                                                      // task name for kernel awareness debugging
        configMINIMAL_STACK_SIZE,                                        // task stack size
        (void*)NULL,                                                     // optional task startup argument
        (configMAX_PRIORITIES - 1),                                      // initial priority
        NULL
    ) != pdPASS) {
        _EXCEPTION("FreeRTOS failed to initialise usb task");
        return;                                                          // this only happens when there was a failure to initialise the task (usually not enough heap)
    }
    #endif
    #if defined FREE_RTOS_UART
    if (xTaskCreate(                                                     // FreeRTOS UART task
        uart_task,                                                       // pointer to the task
        "uart_task",                                                     // task name for kernel awareness debugging
        configMINIMAL_STACK_SIZE,                                        // task stack size
        (void*)NULL,                                                     // optional task startup argument
        (configMAX_PRIORITIES - 1),                                      // initial priority
        NULL
        ) != pdPASS) {
        _EXCEPTION("FreeRTOS failed to initialise uart task");
        return;                                                          // this only happens when there was a failure to initialise the task (usually not enough heap)
    }
    #endif

    vTaskStartScheduler();                                               // start the created tasks for scheduling (this never returns as long as there was no error)
    
    // This never returns under normal circumstances
    //
    _EXCEPTION("FreeRTOS failed to start");
    return;                                                              // this only happens when there was a failure to initialise and start FreeRTOS (usually not enough heap)
}

static void prvSetupHardware(void)
{
    // Add FreeRTOS context hardware initialisation if required
    //
#if defined FREE_RTOS_BLINKY
    fnInitialiseRedLED();
#endif
}

#if defined FREE_RTOS_BLINKY
// Blinky task
//
static void blinky(void *par)
{
    FOREVER_LOOP() {
        fnToggleRedLED();
        vTaskDelay(750/portTICK_RATE_MS);                                // wait for 750ms in order to flash the LED at 1Hz
    }
}
#endif

#if defined FREE_RTOS_USB
extern unsigned char fnGetUSB_Handle(void);
static void usb_task(void *pvParameters)
{
    QUEUE_TRANSFER length = 0;
    QUEUE_HANDLE usb_handle;
    unsigned char ucRxData[128];
    while ((usb_handle = fnGetUSB_Handle()) == NO_ID_ALLOCATED) {        // get the USB handle
        vTaskDelay(500/portTICK_RATE_MS);                                // wait for 500ms and try again
    }
    FOREVER_LOOP() {
        length = fnRead(usb_handle, ucRxData, sizeof(ucRxData));         // read any usb data received
        if (length != 0) {                                               // if something is available
            fnWrite(usb_handle, (unsigned char *)"FreeRTOS Echo:", 14);  // echo it back
            fnWrite(usb_handle, ucRxData, length);                       // send the data back
            fnWrite(usb_handle, (unsigned char *)"\r\n", 2);             // with termination
        }
        else {                                                           // nothing in the input buffer
            vTaskDelay(1);                                               // wait a single tick to allow other tasks to execute
        }
    }
}
#endif

#if defined FREE_RTOS_UART

#if defined UART_SUPPORT_IDLE_LINE_INTERRUPT && defined FREE_RUNNING_RX_DMA_RECEPTION
    #include "semphr.h"
    SemaphoreHandle_t xSemaphore = NULL;
#endif

static void uart_task(void *pvParameters)
{
    QUEUE_TRANSFER length = 0;
    QUEUE_HANDLE uart_handle;
#if defined UART_SUPPORT_IDLE_LINE_INTERRUPT && defined FREE_RUNNING_RX_DMA_RECEPTION
    unsigned char dataBuffer[128];
    xSemaphore = xSemaphoreCreateBinary();                               // create a binary semaphore
#else
    unsigned char dataBuffer[1];
#endif
    while ((uart_handle = fnGetUART_Handle()) == NO_ID_ALLOCATED) {      // get the UART handle
        vTaskDelay(500/portTICK_RATE_MS);                                // wait for 500ms in order to allow uTasker to configure UART interfaces
    }
    fnDebugMsg("FreeRTOS Output\r\n");                                   // test a UART transmission when the task starts and the UART is ready
    FOREVER_LOOP() {
#if defined UART_SUPPORT_IDLE_LINE_INTERRUPT && defined FREE_RUNNING_RX_DMA_RECEPTION
        xSemaphoreTake(xSemaphore, portMAX_DELAY);                       // take semaphore/wait for semaphore to become free (idle line detection)
#endif
        length = fnRead(uart_handle, dataBuffer, sizeof(dataBuffer));    // read waiting data from the DMA input buffer (returns immediately)
        if (length != 0) {                                               // if something is available
            fnDebugMsg("FreeRTOS Echo:");                                // echo it back
            fnWrite(uart_handle, dataBuffer, length);                    // send the reception back
            fnDebugMsg("\r\n");                                          // with termination
        }
#if defined UART_SUPPORT_IDLE_LINE_INTERRUPT || defined FREE_RUNNING_RX_DMA_RECEPTION
        else {                                                           // nothing in the input buffer
            vTaskDelay(1);                                               // wait a single tick to allow other tasks to execute
        }
#endif
    }
}


#if defined UART_SUPPORT_IDLE_LINE_INTERRUPT && defined FREE_RUNNING_RX_DMA_RECEPTION
// Called from UART idle line interrupt
//
extern int fnSciRxIdle(QUEUE_HANDLE channel)
{
    xSemaphoreGive(xSemaphore);                                          // allow the UART task to continue
    return 0;
}
#endif
#endif
#endif