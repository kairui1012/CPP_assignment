#include <iostream>
using namespace std;

// Class definition
class Car{
    public:
        string make;
        string model;
        int year;

    void drive(){
        cout << "Driving the " << make << " " << model << endl;
    }

    void reverse(){
        cout << make << " " << model << " is reversing now"<< endl;
    }
};

int main(){
    // Car myCar, fatherCar, motherCar;

    // myCar.make = "Toyota";
    // myCar.model = "Camry";
    // myCar.year = 2022;

    // // Accessing object's properties and calling member function
    // cout << "Car details: " << myCar.make << " " << myCar.model << " " << myCar.year << endl;
    // myCar.drive();


    // fatherCar.make = "Perodua";
    // fatherCar.model = "Ativa";
    // fatherCar.year = 2022;

    // motherCar.make = "Honda";
    // motherCar.model = "Jazz";
    // motherCar.year = 2019;

    // fatherCar.drive();
    // motherCar.drive();
    // motherCar.reverse();

    // int * ptr;
    // int value = 10;
    // ptr = &value;
    // *ptr = 15;
    // cout << *ptr;

    // Q1
    int *ptr;
    int value = 5;

    ptr = &value;
    cout << "The value of the num is: "<< *ptr << endl;

    // Q2
    cout << "Before increment: " << *ptr << endl;
    *ptr+=1;
    cout << "After increment: " << *ptr << endl;

    // Q3
    int num1,num2;
    int *ptr1,*ptr2;

    cout << "Enter the first number: ";
    cin >> num1;

    cout << "Enter the second number: ";
    cin >> num2;

    ptr1 = &num1;
    ptr2 = &num2;

    if (*ptr2 > *ptr1){
        cout << "The maximum number is: " << *ptr2 << endl;
    }
    else if (*ptr2 == *ptr1){
        cout << "The numbers are equal" << endl;
    }
    else {
        cout << "The maximum number is: " << *ptr1 << endl;
    }

}