#ifndef GLOBAL_H
#define GLOBAL_H

#define USE_CAN 1
#define plan_A 1
#define plan_B 0
#define plan_C 0

#define Move_On()                                  \
    {                                              \
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5, SET); \
    }

#define Move_Off()                                   \
    {                                                \
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5, RESET); \
    }

#define Power_On()                                                     \
    {                                                                  \
        HAL_GPIO_WritePin(POWER1_GPIO_Port, POWER1_Pin, GPIO_PIN_SET); \
        HAL_GPIO_WritePin(POWER2_GPIO_Port, POWER2_Pin, GPIO_PIN_SET); \
        HAL_GPIO_WritePin(POWER3_GPIO_Port, POWER3_Pin, GPIO_PIN_SET); \
        HAL_GPIO_WritePin(POWER4_GPIO_Port, POWER4_Pin, GPIO_PIN_SET); \
    }

#define Power_Off()                                                      \
    {                                                                    \
        HAL_GPIO_WritePin(POWER1_GPIO_Port, POWER1_Pin, GPIO_PIN_RESET); \
        HAL_GPIO_WritePin(POWER2_GPIO_Port, POWER2_Pin, GPIO_PIN_RESET); \
        HAL_GPIO_WritePin(POWER3_GPIO_Port, POWER3_Pin, GPIO_PIN_RESET); \
        HAL_GPIO_WritePin(POWER4_GPIO_Port, POWER4_Pin, GPIO_PIN_RESET); \
    }

#endif
