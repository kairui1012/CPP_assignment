#include <iostream>
using namespace std;

// Functions 
// Q1
void printer(){
    cout << "I love C programming so freaking much yayy" << endl;
}

// Q2
void numChecker(int number){
    if (number % 2 != 0){
        cout << number << " is odd" << endl;
    }else{
        cout << number << " is even" << endl;
    }
}

// Q3
int numberPrinter(){
    int num;

    cout << "Enter a number: ";
    cin >> num;

    return num; 
}

// Q4
int sum(int num1, int num2){
    return num1 + num2;
}


// Q5
double perimeter(double length1, double length2, double length3){
    return length1 + length2 + length3;
}

int main() {
    // Loop
    //Q1
    int sum = 0;
    for (int i = 1; i<=50; i++){
        sum += i;
    }
    cout << "The sum of the first 50 numbers is: " << sum << endl;

    // Q2
    int even = 2;
    while (even <=100){
        if (even % 2 == 0){
            cout << even << endl;
        }
        even++;
    }

    // Q3
    int num;
    int store;
    do{
        cout << "Enter a number(Negative to stop): ";
        cin >> num;
        if (num >= 0){
            store = num;
        }
    }while (num >= 0);
    cout << "Last non-negative number is: " << store << endl;

    // Q4
    int rows;
    int columns;
    cout << "Enter the number of rows: ";
    cin >> rows;

    cout << "Enter the number of columns: ";
    cin >> columns;

    for (int i = 1; i <= rows; i++){
        for (int j = 1; j <= columns; j++){
            cout << "X\t\t";
        }
        cout << endl;
    }

    // Q5
    int password;
    int attempt = 1;

    do{ 
        cout << "Enter the password: ";
        cin >> password;

        if (password == 1234){
            cout << "Withdrawal successful" << endl;
            break;
        }else {
            cout << "Incorrect pin" << endl;
        }
        attempt++;
    }while (attempt < 4);
    if (attempt > 3 && password != 1234){
        cout << "Card blocked" << endl;
    }


    return 0;
}