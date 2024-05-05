#include <stdio.h>
#include <string.h>

char* gapAction(float gap) {
    if (gap < 3.5) return "Gogogo";
    else if (gap >= 3.5 && gap <= 10) return "Push";
    else return "Stay out of trouble";
}

char* fuelAction(float fuel) {
    if (fuel > 80) return "Push Push Push";
    else if (fuel >= 50 && fuel <= 80) return "You can go";
    else return "Conserve Fuel";
}

char* tireAction(int tireWear) {
    if (tireWear > 80) return "Go Push Go Push";
    else if (tireWear >= 50 && tireWear <= 80) return "Good Tire Wear";
    else if (tireWear >= 30 && tireWear < 50) return "Conserve Your Tire";
    else return "Box Box Box";
}

char* tireChangeAction(char* tireType) {
    if (strcmp(tireType, "Soft") == 0) return "Mediums Ready";
    else if (strcmp(tireType, "Medium") == 0) return "Box for Softs";
    else return "Invalid Tire Type";
}
