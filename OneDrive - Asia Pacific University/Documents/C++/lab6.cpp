#include <iostream>
#include <algorithm>
#include <cctype>
using namespace std;

struct Contact{
    string name;
    string phoneNumber;
    Contact* nextContact;
};

void addContact(Contact* &head, string name, string phoneNumber){
    // Create a new contact node
    Contact* newContact = new Contact;
    newContact->name = name;
    newContact->phoneNumber = phoneNumber;
    newContact->nextContact = nullptr;

    // If the list is empty, make the new contact the head
    if (head == nullptr){
        head = newContact;
        return;
    }

    // Traverse to the last contact
    Contact* current = head;
    while (current->nextContact != nullptr){
        current = current->nextContact;
    }

    // Add the new contact at the end
    current->nextContact = newContact;
}

void displayContacts(Contact* head){
    // Check if contact book is empty
    if (head == nullptr){
        cout << "Contact book is empty." << endl;
        return;
    }

    // Counter to track number of contacts
    int contactCount = 0;

    // Traverse and display contacts
    Contact* current = head;
    cout << "---- Contact Book -----" << endl;
    while (current != nullptr){
        // Display contact details 
        cout << "Contact#" << ++contactCount << endl;
        cout << "Name: " << current->name << endl;
        cout << "Phone Number: " << current->phoneNumber << endl;
        cout << "-----------------------" << endl;

        // Move to the next contact
        current = current->nextContact;
    }

    // Display total contacts
    cout << "Total Contacts: " << contactCount << endl;
}

// Helper function for case-insensitive comparison
string toLowercase(string str){
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

Contact* searchContact(Contact* head, const string& searchName){
    // Check if contact book is empty
    if (head == nullptr){
        cout << "Contact book is empty." << endl;
        return nullptr;
    }

    // Traverse the contact list
    Contact* current = head;
    int contactIndex = 1;

    while (current != nullptr){
        // Case-insensitive name comparison
        if (toLowercase(current->name) == toLowercase(searchName)){
            cout << "Contact Found!" << endl;
            cout << "Name: " << current->name << endl;
            cout << "Phone: " << current->phoneNumber << endl;
            cout << "Position: " << contactIndex << endl;
        
            return current;
        }

        current = current->nextContact;
        contactIndex++;            
    }

    // Contact not found
    cout << "Contact " << searchName << " not found." << endl;
    return nullptr;
}

// Function to count total contacts
int countContact(Contact* head){
    // Initialize contact counter
    int totalContacts = 0;

    // Traverse the contact list
    Contact* current = head;
    while (current != nullptr){
        totalContacts++;
        current = current->nextContact;
    }

    return totalContacts;
}

// Overloaded function with additional details
void countContacts(Contact* head, bool verbose){
    int totalContacts = 0;
    int mobileContacts = 0;
    int landlineContacts = 0;

    // Traverse the contact list
    Contact* current = head;
    while (current != nullptr){
        totalContacts++;

        // Optional: Count phone number types
        if (current->phoneNumber.find("-") != string::npos){
            mobileContacts++;
        } else {
            landlineContacts++;
        }

        current = current->nextContact;
    }

    // Verbose output
    if (verbose){
        cout << "--- Contact Statistics ---" << endl;
        cout << "Total Contacts: " << totalContacts << endl;
        cout << "Mobile Contacts: " << mobileContacts << endl;
        cout << "Landline Contacts: " << landlineContacts << endl;

    }
}

void deleteContact(Contact** head){
    if (*head == nullptr){
        cout << "Contact book is empty." << endl;
        return;
    }

    Contact* temp = *head;
    while (temp != nullptr){
        Contact* next = temp->nextContact;
        delete temp;
        temp = next;
    }

    *head = nullptr;
    cout << "Contact book deleted." << endl;
}

int main(){
    Contact* contactBook = nullptr;

    // Adding contacts
    addContact(contactBook, "John Doe", "123-456-7890");
    addContact(contactBook, "Jane Smith", "987-654-3210");
    addContact(contactBook, "Alice Johnson", "456-789-0123");

    // Displaying contacts
    displayContacts(contactBook);

    searchContact(contactBook, "Jane Smith");
    searchContact(contactBook, "Bob Brown");

    // Basic count
    int totalContacts = countContact(contactBook);
    cout << "Total Contacts: " << totalContacts << endl;

    // Verbose count with details
    countContacts(contactBook, true);

    deleteContact(&contactBook);
    displayContacts(contactBook);

    
    return 0;
}