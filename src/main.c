#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "uart.h"

SemaphoreHandle_t xUARTMutex;
SemaphoreHandle_t xEmergencySemaphore;

volatile unsigned int *const SCB_DEMCR = (unsigned int *)0xE000EDFC;
volatile unsigned int *const DWT_CONTROL = (unsigned int *)0xE0001000;
volatile unsigned int *const DWT_CYCCNT = (unsigned int *)0xE0001004;
volatile unsigned int isr_start_cycle = 0;

volatile int current_speed = 100;
volatile int braking_mode = 0;
volatile int pedal_active = 0;

void vCruisingTask(void *pvParameters);
void vEmergencyBrakeTask(void *pvParameters);
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);

int main(void) {
  *SCB_DEMCR |= 0x01000000;
  *DWT_CYCCNT = 0;
  *DWT_CONTROL |= 1;

  xUARTMutex = xSemaphoreCreateMutex();
  xEmergencySemaphore = xSemaphoreCreateBinary();

  if (xUARTMutex != NULL && xEmergencySemaphore != NULL) {
    xTaskCreate(vEmergencyBrakeTask, "Brake", 256, NULL, 3, NULL);
    xTaskCreate(vCruisingTask, "Cruise", 256, NULL, 1, NULL);

    UART_EnableRXInterrupt();
    vTaskStartScheduler();
  }
  while (1)
    ;
  return 0;
}

void vCruisingTask(void *pvParameters) {
  for (;;) {

    if (pedal_active == 0) {
      if (xSemaphoreTake(xUARTMutex, portMAX_DELAY) == pdTRUE) {
        if (current_speed == 0) {
          UART_PrintString("[SYSTEM] Vehicle Stopped. Press 'C' to accelerate "
                           "back to 100 km/h.\n");
        } else {
          UART_PrintString("[SYSTEM] Cruising at ");
          UART_PrintInt(current_speed);
          UART_PrintString(" km/h... (Hold 'B' for Manual, 'H' for ABS)\n");
        }
        xSemaphoreGive(xUARTMutex);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void vEmergencyBrakeTask(void *pvParameters) {
  int wheel_speed = 100;
  int slip_ratio = 0;

  for (;;) {
    xSemaphoreTake(xEmergencySemaphore, portMAX_DELAY);

    wheel_speed = current_speed;

    while (pedal_active > 0 && current_speed > 0) {

      pedal_active--;

      if (xSemaphoreTake(xUARTMutex, portMAX_DELAY) == pdTRUE) {
        UART_PrintString("Veh: ");
        UART_PrintInt(current_speed);
        UART_PrintString(" | Whl: ");
        UART_PrintInt(wheel_speed);
        UART_PrintString(" -> ");

        if (braking_mode == 2) {
          UART_PrintString("[ABS PANIC] ");
          current_speed -= 4;

          slip_ratio = ((current_speed - wheel_speed) * 100) / current_speed;

          if (slip_ratio > 20) {
            UART_PrintString("Valve: RELEASE\n");
            wheel_speed += 8;
          } else if (slip_ratio < 10) {
            UART_PrintString("Valve: APPLY\n");
            wheel_speed -= 12;
          } else {
            UART_PrintString("Valve: HOLD\n");
            wheel_speed -= 6;
          }
        } else if (braking_mode == 1) {
          UART_PrintString("[MANUAL BRAKE] Smooth Deceleration\n");
          current_speed -= 1;
          wheel_speed = current_speed;
        }

        if (wheel_speed > current_speed)
          wheel_speed = current_speed;
        if (wheel_speed < 0)
          wheel_speed = 0;
        if (current_speed < 0)
          current_speed = 0;

        xSemaphoreGive(xUARTMutex);
      }
      vTaskDelay(pdMS_TO_TICKS(100));
    }

    if (xSemaphoreTake(xUARTMutex, portMAX_DELAY) == pdTRUE) {
      if (current_speed <= 0) {
        UART_PrintString(
            "\n[SYSTEM] Vehicle completely stopped. Idling at 0 km/h.\n\n");
      } else {
        UART_PrintString("\n[SYSTEM] Brake Released. Holding new speed: ");
        UART_PrintInt(current_speed);
        UART_PrintString(" km/h\n\n");
      }
      xSemaphoreGive(xUARTMutex);
    }

    while (xSemaphoreTake(xEmergencySemaphore, 0) == pdTRUE)
      ;
  }
}

void UART0_Handler(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  char received_key = (char)(*UART0_DR);
  *UART0_ICR |= (1 << 4);

  if (received_key == 'B' || received_key == 'b') {
    braking_mode = 1;
    pedal_active = 5;
    xSemaphoreGiveFromISR(xEmergencySemaphore, &xHigherPriorityTaskWoken);
  } else if (received_key == 'H' || received_key == 'h') {
    braking_mode = 2;
    pedal_active = 5;
    xSemaphoreGiveFromISR(xEmergencySemaphore, &xHigherPriorityTaskWoken);
  } else if (received_key == 'C' || received_key == 'c') {

    current_speed = 100;
    pedal_active = 0;
  }

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
  UART_PrintString("\n[FATAL EXCEPTION] MEMORY STACK OVERFLOW!\n");
  while (1)
    ;
}