#include <iostream>
using namespace std;

int main() {
    // Q1
    char studentStatus;

    cout << "Are you APU university student? (Y/N): ";
    cin >> studentStatus;

    if (studentStatus == 'Y') {
        cout << "Congrats, you are entitled" << endl;
    } 

    // Q2
    char grade;
    
    cout << "Enter your grade: "; // Added the missing semicolon here
    cin >> grade;
    
    if (grade >= 80) {
        cout << "A" << endl;
    }else if (grade >= 70){
        cout << "B" <<endl;
    }else if (grade >= 60){
        cout << "C" <<endl;
    }else if (grade >= 50){
        cout << "D" <<endl;
    }else if (grade >= 40){
        cout << "E" <<endl;
    }else{
        cout << "F" <<endl;
    } 

    int age;
    char gender;

    cout << "Pls enter your age: ";
    cin >> age;

    cout << "Pls enter your gender (M/F): ";
    cin >> gender;

    if (age>12 || gender=='F') {
        cout << "You get an ice-cream!!!";
    }
    else {
        cout << "Sorry...";
    }

    return 0;    
}