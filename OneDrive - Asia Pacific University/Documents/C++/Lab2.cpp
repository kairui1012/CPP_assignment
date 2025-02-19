#include <iostream>
#include <cmath>
using namespace std;

int main() {
    // Q1
    string name;
    cout << "Enter your name: ";
    getline(cin, name); 

    cout << "Hello, " << name << "!" << endl;

    // Q2
    int num;
    cout <<"Enter an integer: ";
    cin >> num;

    if (num % 2 == 0) {
        cout << num << " is an even number." << endl;
    } else {
        cout << num << " is an odd number." << endl;
    }

    // Q3
    int age;

    cout << "Enter your age: ";
    cin >> age;

    if (age >= 65){
        cout << "You are a senior" << endl;
    }
    else if (age >=18){
        cout << "You are an adult" << endl;
    }
    else {
        cout << "You are a child" << endl;
    }

    // Q4
    double kg;
    double m;
    double bmi;

    cout << "Enter your weight in kilograms: ";
    cin >> kg;

    cout << "Enter your height in meters: ";
    cin >> m;

    bmi = kg / pow(m, 2);
    cout << "Your BMI is: " << bmi << endl;

    string classi = "Classification: ";
    if (bmi >= 30){
        classi += "Obesity";
    }
    else if (bmi >= 25){
        classi += "Overweight";
    }
    else if (bmi >= 18.5) {
        classi += "Normal weight";
    }
    else {
        classi += "Underweight";
    }
    cout << classi << endl;

    return 0;
}