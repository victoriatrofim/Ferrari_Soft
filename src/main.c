#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structs.h"
// initialization functions for tire and pmu sensor
tire_sensor *init_tire(void)
{
	tire_sensor *sensor_data = (tire_sensor *)malloc(sizeof(tire_sensor));
	sensor_data->pressure = 0;
	sensor_data->temperature = 0;
	sensor_data->wear_level = 0;
	sensor_data->performace_score = 0;
	return sensor_data;
}

power_management_unit *init_pmu(void)
{
	power_management_unit *pmu = (power_management_unit *)
									malloc(sizeof(power_management_unit));
	pmu->voltage = 0;
	pmu->current = 0;
	pmu->power_consumption = 0;
	pmu->energy_regen = 0;
	pmu->energy_storage = 0;
	return pmu;
}


int main(int argc, char const *argv[])
{
	// read values from binary file
	FILE *file = fopen(argv[1], "rb");
	if (file == NULL) {
		printf("Error at opening file!\n");
		return -1;
	}
	// read the number of sensors from binary file
	int nr_sens = 0, type = 0;

	fread(&nr_sens, sizeof(nr_sens), 1, file);
	// alloc space in memory for array of sensors
	sensor **arr_sensors = malloc(nr_sens * sizeof(sensor));
	for (int i = 0; i < nr_sens; i++) {
		arr_sensors[i] = malloc(sizeof(sensor));
		// read the type of sensors from binary file
		fread(&type, sizeof(type), 1, file);
		// pmu case
		if (type == 1) {
			int nr_op = 0;
			arr_sensors[i]->sensor_type = PMU;
			// init a pmu type of pointer
			power_management_unit *pmu = init_pmu();
			arr_sensors[i]->sensor_data = pmu;
			// read the data for pmu
			fread(pmu, sizeof(power_management_unit), 1, file);
			fread(&nr_op, sizeof(int), 1, file);
			arr_sensors[i]->nr_operations = nr_op;
			int *op = (int *)malloc(nr_op * sizeof(int));
			arr_sensors[i]->operations_idxs = op;
			fread(op, sizeof(int), nr_op, file);

		} else if (type == 0) { // tire case
			int nr_op = 0;
			arr_sensors[i]->sensor_type = TIRE;
			// init a tire pointer
			tire_sensor *tire = init_tire();
			arr_sensors[i]->sensor_data = tire;
			// read the data for tire
			fread(tire, sizeof(tire_sensor), 1, file);
			fread(&nr_op, sizeof(int), 1, file);
			arr_sensors[i]->nr_operations = nr_op;
			int *op = (int *)malloc(nr_op * sizeof(int));
			arr_sensors[i]->operations_idxs = op;
			fread(op, sizeof(int), nr_op, file);
		}
	}
	// sort the array by type: pmu( = 1)first
	for (int i = 0 ; i < nr_sens; i++) {
		for (int j = 0; j < nr_sens - i - 1; j++) {
			if (arr_sensors[j]->sensor_type < arr_sensors[j + 1]->sensor_type) {
				void *tmp = arr_sensors[j];
				arr_sensors[j] = arr_sensors[j + 1];
				arr_sensors[j + 1] = tmp;
			}
		}
	}
	char *command = malloc(20);
	int index = 0;
	// read from terminal the command until we find 'exit' one
	scanf("%s", command);
	while (strcmp(command, "exit") != 0) {
		// for print command
		if (strcmp(command, "print") == 0) {
			// read one more time for index
			scanf("%d", &index);
			if (index < 0 || index >= nr_sens) {
				printf("Index not in range!\n");
			} else {
				// print the tire sensor and it's data
				if (arr_sensors[index]->sensor_type == 0) {
					printf("Tire Sensor\n");
					printf("Pressure: %.2f\n",
						((tire_sensor *)
							(arr_sensors[index]->sensor_data))->pressure);
					printf("Temperature: %.2f\n",
						((tire_sensor *)
							(arr_sensors[index]->sensor_data))->temperature);
					printf("Wear Level: %d%%\n",
						((tire_sensor *)
							(arr_sensors[index]->sensor_data))->wear_level);
					if (((tire_sensor *)
				(arr_sensors[index]->sensor_data))->performace_score == 0) {
						printf("Performance Score: Not Calculated\n");
					} else
						printf("Performance Score: %d\n",
						((tire_sensor *)
						(arr_sensors[index]->sensor_data))->performace_score);
				} else {
				// print the pmu sensor and it's data
					printf("Power Management Unit\n");
					printf("Voltage: %.2f\n",
						((power_management_unit *)
							(arr_sensors[index]->sensor_data))->voltage);
					printf("Current: %.2f\n",
						((power_management_unit *)
							(arr_sensors[index]->sensor_data))->current);
					printf("Power Consumption: %.2f\n",
						((power_management_unit *)
						(arr_sensors[index]->sensor_data))->power_consumption);
					printf("Energy Regen: %d%%\n",
						((power_management_unit *)
							(arr_sensors[index]->sensor_data))->energy_regen);
					printf("Energy Storage: %d%%\n",
						((power_management_unit *)
						(arr_sensors[index]->sensor_data))->energy_storage);
				}
			}
		} else {
			// for analize command
			if (strcmp(command, "analyze") == 0) {
				scanf("%d", &index);
				if (index < 0 || index >= nr_sens) {
					printf("Index not in range!\n");
				} else {
					// alloc space in memory for operations array
					void **operations = malloc(8 * sizeof(void *));
					// init the array with elem from get_operaton function
					get_operations(operations);
					// void (*functions)(void *data);
					// execute all the operations given in order
				for (int i = 0; i < arr_sensors[index]->nr_operations; i++) {
						// cast to void* funct that receive a void* param
						((void (*) (void *))
							operations[arr_sensors[index]->operations_idxs[i]])
								(arr_sensors[index]->sensor_data);
					}
					free(operations);
				}
			} else {
				// for clear command
				for (int i = 0; i < nr_sens; i++) {
					int ok = 0;
					// check all the necesary conditions
					if (arr_sensors[i]->sensor_type == 0) {
						if (((tire_sensor *)
						(arr_sensors[i]->sensor_data))->pressure < 19 ||
						((tire_sensor *)
						(arr_sensors[i]->sensor_data))->pressure > 28 ||
						((tire_sensor *)
						(arr_sensors[i]->sensor_data))->temperature < 0 ||
						((tire_sensor *)
						(arr_sensors[i]->sensor_data))->temperature > 120 ||
						((tire_sensor *)
						(arr_sensors[i]->sensor_data))->wear_level < 0 ||
						((tire_sensor *)
						(arr_sensors[i]->sensor_data))->wear_level > 100) {
							ok = 1;
						}
					} else {
						if (((power_management_unit *)
						(arr_sensors[i]->sensor_data))->voltage < 10 ||
							((power_management_unit *)
							(arr_sensors[i]->sensor_data))->voltage > 20 ||
							((power_management_unit *)
							(arr_sensors[i]->sensor_data))->current < -100 ||
							((power_management_unit *)
							(arr_sensors[i]->sensor_data))->current > 100 ||
							((power_management_unit *)
					(arr_sensors[i]->sensor_data))->power_consumption < 0 ||
							((power_management_unit *)
					(arr_sensors[i]->sensor_data))->power_consumption > 1000 ||
							((power_management_unit *)
							(arr_sensors[i]->sensor_data))->energy_regen < 0 ||
							((power_management_unit *)
						(arr_sensors[i]->sensor_data))->energy_regen > 100 ||
							((power_management_unit *)
						(arr_sensors[i]->sensor_data))->energy_storage < 0 ||
							((power_management_unit *)
						(arr_sensors[i]->sensor_data))->energy_storage > 100) {
								ok = 1;
						}
					}
					// delete sensors that don't corespond
					if (ok == 1) {
						free(arr_sensors[i]->sensor_data);
						free(arr_sensors[i]->operations_idxs);
						free(arr_sensors[i]);

						for (int j = i; j < nr_sens - 1; j++) {
						arr_sensors[j] = arr_sensors[j + 1];
						}
						// realloc the array of sensors
						nr_sens--;
						arr_sensors =
							realloc(arr_sensors, nr_sens
								* sizeof(sensor) - sizeof(sensor));
						i--;
					}
				}
			}
		}
		// read the next command
		scanf("%s", command);
	}
	// free the alocated memory
	for (int i = 0; i < nr_sens; i++) {
		free(arr_sensors[i]->sensor_data);
		free(arr_sensors[i]->operations_idxs);
		free(arr_sensors[i]);
	}
	free(arr_sensors);
	free(command);
	fclose(file);
	return 0;
}
