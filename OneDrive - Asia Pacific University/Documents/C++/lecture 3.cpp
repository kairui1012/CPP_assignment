#include <iostream>
using namespace std;


void numAddition1(int num1, int num2){
    cout << "Sum: " << num1 + num2 << endl;
}

int numAddition2(){
    cout << "Enter 1st number: ";
    int num1;
    cin >> num1;

    cout << "Enter 2nd number: ";
    int num2;
    cin >> num2;

    return num1 + num2;
}

void numAddition3(){
    cout << "Enter 1st number: ";
    int num1;
    cin >> num1;

    cout << "Enter 2nd number: ";
    int num2;
    cin >> num2;

    cout << "Sum: " << num1 + num2 << endl;        
}

int main() {
    // Q1
    numAddition1(5, 10);

    // Q2
    int total = numAddition2();
    cout << "Sum: " << total << endl;

    // Q3
    numAddition3();

    return 0;
}