#include "stm32f4xx.h"
#include "./DAC/DAC.h"
#include "./USART/USART.h"
#include "string.h"

void delayMs(int n);
int main(void)
{
	DAC_config();
	DAC_SetChannel1Data(DAC_Align_12b_R,0xF17);  /* 0xE64 will give an output current of 0.5A, which is the maximum for current implementation */
	USART_Config();

	float current;
	int DAC_input;
	printf("This is a program to interface charging current through USART.\n\n");
	printf("In default, the current is set to be the maximum, 0.5A.\n\n");


	while(1){
		/* testing program of the final interface */
		int mode_sel;
		printf("Please type in the desired mode number, followed by a space/enter and then press send: \n");
		printf("Mode 1: stable current\n");
		printf("Mode 2: charing schedule\n\n");
		scanf("%d",&mode_sel);
		
		
		while(mode_sel == 1){
			printf("------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
			printf("Mode 1\n\n");
			printf("Please type in the desired current(0-0.5A), followed by a space/enter and then press send: \n\n");
			scanf("%f", &current);
			printf("Charing current now: %.3f A\n\n", current);
			DAC_input = 0xF17 * current / 0.5f;
			DAC_SetChannel1Data(DAC_Align_12b_R, DAC_input);
			while(1){
				printf("Please type in the next desired current(0-0.5A), followed by a space/enter and then press send: \n\n");
				scanf("%f", &current);
				printf("Charing current now: %.3f A\n\n", current);
				DAC_input = 0xF17 * current / 0.5f;
				DAC_SetChannel1Data(DAC_Align_12b_R, DAC_input);
			}
		}
		
		
		while(mode_sel == 2){
			int total_stages;
			printf("------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
			printf("Mode 2\n\n");
			printf("Please specify the total stages of the charging schedule, followed by a space/enter and then press send: \n\n");
			scanf("%d", &total_stages);
			printf("The charging schedule will have %d stages.\n\n", total_stages);
			printf("------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
			
			float current_array[total_stages];
			int time_array[total_stages];
			for(int i = 0; i < total_stages; i++){
				printf("Please type in the desired current(0-0.5A) in stage %d, followed by a space/enter and then press send:\n", i + 1);
				scanf("%f",&current_array[i]);
				printf("Please type in the time(in seconds) in stage %d, followed by a space/enter and then press send: \n", i + 1);
				scanf("%d",&time_array[i]);
			}

			
			ask_for_comfirmation:
			printf("\nThe charging schedule is being recorded as follows: \n");
			for(int i = 0; i < total_stages; i++){
				printf("     Stage %d: %.3f A for %d s\n", i + 1, current_array[i], time_array[i]);
			}
			char comfirmation_flag[2];
			printf("\nConfirm and proceed? Y/N followed by a space/enter and then press send: \n");
			scanf("%s", comfirmation_flag);
			printf("You entered: %s\n", comfirmation_flag);

			int comparison = 0;
			while(comparison == 0){
				/* check if flag is N or n */
				/* if it's N or n, then comparion == 0 */
				comparison = strcmp(comfirmation_flag, "N") && strcmp(comfirmation_flag, "n");
				
				/* user entered N, modify data */
				if(comparison == 0){
					int to_be_modified_stage;
					float to_be_modified_current;
					int to_be_modified_time;
					printf("Please type in the number of the stage to be modified, followed by a space/enter and then press send: \n");
					scanf("%d", &to_be_modified_stage);
					printf("Please type in the desired current(0-0.5A) in stage %d, followed by a space/enter and then press send: \n", to_be_modified_stage);
					scanf("%f", &to_be_modified_current);
					printf("Please type in the new time(in seconds) in stage %d, followed by a space/enter and then press send:\n", to_be_modified_stage);
					scanf("%d", &to_be_modified_time);
					current_array[to_be_modified_stage - 1] = to_be_modified_current;
					time_array[to_be_modified_stage - 1] = to_be_modified_time;
					
					/* print the modified schedule again for reference */
					printf("\nThe charging schedule is being recorded as follows: \n");
					for(int i = 0; i < total_stages; i++){
						printf("     Stage %d: %.3f A for %d s\n", i + 1, current_array[i], time_array[i]);
					}
					
					/* check again to see if there are other modifications needed */
					printf("\nConfirm and proceed? Y/N followed by a space/enter and then press send: \n");
					scanf("%s", comfirmation_flag);
					printf("You entered: %s\n", comfirmation_flag);
					
					/* if it's N or n, then comparion == 0 */
					comparison = strcmp(comfirmation_flag, "N") && strcmp(comfirmation_flag, "n");
				}		
			}
			
			/* check if flag is Y or y */
			/* if it's Y or y, then comparion == 0 */
			comparison = strcmp(comfirmation_flag, "Y") && strcmp(comfirmation_flag, "y");
			while(comparison == 0){
				int delay_in_ms;
				for(int i = 0; i < total_stages; i++){
					DAC_input = 0xF17 * current_array[i] / 0.5f;
					DAC_SetChannel1Data(DAC_Align_12b_R, DAC_input);
					delay_in_ms = time_array[i] * 1000;
					delayMs(delay_in_ms);
				}
			}
			printf("\nInvalid input!\n");
			goto ask_for_comfirmation;
		}
	}
}

/* 180 MHz SysTick*/
void delayMs(int n) {
    int i;

    /* Configure SysTick */
    SysTick->LOAD = 180000;  /* reload with number of clocks per millisecond */
    SysTick->VAL = 0;       /* clear current value register */
    SysTick->CTRL = 0x5;    /* Enable the timer */

    for(i = 0; i < n; i++) {
        while((SysTick->CTRL & 0x10000) == 0) /* wait until the COUNTFLAG is set */
            { }
    }
    SysTick->CTRL = 0;      /* Stop the timer (Enable = 0) */
}
