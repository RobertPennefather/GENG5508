//Gurashish Singh Bhatia, 21324773
//Junho Jung, 21302695

#include <math.h>
#include "eyebot.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "im.h"
#include "fann.h"

#define SCALE 1

#define IMAGE_WIDTH 160
#define IMAGE_HEIGHT 120

int main(){
	
	//Place robot facing object
	//Initialise camera
	//CAMInit(QQVGA);
	
	//-------------------------------------------
	
	//Initialise LCD Screen
	LCDClear();
	LCDMenu("One", "Two", "Three", "Four");
	printf("\nPress Key 1 to draw nodes.\n");
	KEYWait(KEY1);
	
	//-------------------------------------------
	
	//
	const unsigned int num_input = 144;
    const unsigned int num_output = 3;
    const unsigned int num_layers = 3;
    const unsigned int num_neurons_hidden = 30;
    const float desired_error = (const float) 0.001;
    const unsigned int max_epochs = 500000;
    const unsigned int epochs_between_reports = 1000;

    struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);

    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

    fann_train_on_file(ann, "data.txt", max_epochs, epochs_between_reports, desired_error);

    fann_save(ann, "train_data.net");

    fann_destroy(ann);

    return 0;
	
	//-------------------------------------------
	
	//printf("\nPress any key to exit.");
	
	LCDMenu("Exit","Exit","Exit","Exit");
	KEYWait(ANYKEY);
}
