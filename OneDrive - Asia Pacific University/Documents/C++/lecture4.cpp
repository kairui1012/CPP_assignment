#include <iostream>
using namespace std;

int values[10];
int value[3][4];

int main() {
    double total = 0;
    for (int i = 0; i < 10; i++){               
        cout << "Enter the score for student " << i+1 << ": ";
        cin >> values[i];
    }

    cout << "Scores for the students:" << endl;
    for (int i = 0; i < 10; i++){        
        cout << "Student " << i+1 << ": " << values[i] << endl;
        total += values[i];        
    }

    cout << "Average score: " << total/10 << endl;

    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 4; j++){
            cout << "Enter the value for row " << i+1 << " and column " << j+1 << ": ";
            cin >> value[i][j];
        }
        cout << "\n"*2;
    }

    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 4; j++){
            cout << value[i][j] << "  ";
        }
        cout << endl;
    }
    return 0;
}