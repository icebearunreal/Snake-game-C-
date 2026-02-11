#include "array_util.h"

void swap_elements(char *arr, int indexA, int indexB, int size) {
    if (indexA >= 0 && indexA < size && indexB >= 0 && indexB < size) {
        char temp = arr[indexA];
        arr[indexA] = arr[indexB];
        arr[indexB] = temp;
    }
}