#include <iostream>
using namespace std;
#include <cstring>

class Book{
    private:
        char title[100];
        char author[50];
        float price; 

    public:
        void DisplayDetails(char Title[], char Author[], float Price){
            cout << "Book Details: " << endl;
            cout << "Title: " << Title << endl;
            cout << "Author: " << Author << endl;
            cout << "Price: " << Price << endl;
        };
};

class Rectangle{
    private:
        float length;
        float width;
    public:
        Rectangle(float l, float w){
            length = l;
            width = w;
        };

        float getPerimeter(){
            return 2 * (length + width);
        };

        float getArea(){
            return length * width;
        }
};   

class Date{
    private:
        int day;
        int month;
        int year;
    public:
        Date(int d, int m, int y){
            day = d;
            month = m;
            year = y;
        };

        void DisplayDate(){
            cout << "Date: " << day << "/" << month << "/" << year << endl;
        };
};

class Employee{
    private:
        char name[50];
        int id;
        float salary;

    public:
        Employee(char Name[], int Id, float Salary){
            strcpy(name, Name);
            id = Id;
            salary = Salary;
        };

        void DisplayDetails(){
            cout << "Employee Details: " << endl;
            cout << "Name: " << name << endl;
            cout << "ID: " << id << endl;
            cout << "Salary: " << salary << endl;
        };
};

int main(){
    // Class
    // Q1
    Book book1;
    char title[100];
    char author[50];
    float price;

    cout << "Enter the title of the book: ";
    cin.getline(title, 100);

    cout << "Enter the author of the book: ";
    cin.getline(author, 100);

    cout << "Enter the price of the book: ";
    cin >> price;

    book1.DisplayDetails(title, author, price);

    // Q2
    float length, width;

    cout << "Enter the length of the rectangle: ";
    cin >> length;

    cout << "Enter the width of the rectangle: ";
    cin >> width;
    Rectangle rec(length, width);

    cout << "Area: " << rec.getArea() << endl;
    cout << "Perimeter: " << rec.getPerimeter() << endl;

    // Q3
    int day, month, year;

    cout << "Enter day: ";
    cin >> day;

    cout << "Enter month: ";   
    cin >> month;

    cout << "Enter year: ";
    cin >> year;

    Date date(day, month, year);
    date.DisplayDate();

    // Q4
    char name[50];
    int id;
    float salary;

    cout << "Enter employee name: ";
    cin.getline(name, 50);

    cout << "Enter employee ID: " ;
    cin >> id;

    cout << "Enter employee salary: ";
    cin >> salary;

    Employee emp(name, id, salary);
    emp.DisplayDetails();


    // Pointer
    // Q1
    int* z;
    int m =10, n, o;

    cout << "Here is m=10, n and o are two integer variable and *z is an integer\n\n";
    z = &m;
    cout << "z stores the address of m = " << z << endl;
    cout << "*z stores the value of m = " << *z << endl;
    cout << "&m stores the address of m = " << &m << endl;
    cout << "&n stores the address of n = " << &n << endl;
    cout << "&o stores the address of o = " << &o << endl;
    cout << "&z stores the address of z = " << z << endl;

    // Q2
    int* ab;
    int m=29;

    cout << "Here in the declaration ab = int pointer, int m = 29" << endl;
    cout << "Address of m: " << &m << endl;
    cout << "Value of m: " << m << "\n\n";

    cout << "Now ab is assigned with the address of m." << endl;
    ab = &m;
    cout << "Address of pointer ab: " << ab << endl;
    cout << "Content of pointer ab: " << *ab << "\n\n";

    cout << "The value of m assigned to 34 now" << endl;
    m = 34;
    cout << "Address of pointer ab: " << ab << endl;
    cout << "Content of pointer ab: " << *ab << "\n\n";

    cout << "The pointer variable ab is assigned the value 7 now" << endl;
    m = 7;
    cout << "Address of m: " << &m << endl;
    cout << "Value of m: " << m << endl;

    
    // Q3
    int num1, num2;
    int * ptr1, *ptr2;

    ptr1 = &num1;
    ptr2 = &num2;

    cout << "Input the first number: " ;
    cin >> num1;

    cout << "Input the second number: " ;
    cin >> num2;

    int total = *ptr1 + *ptr2;
    cout << "The sum of the entered number is: " << total << endl;


    return 0;
}