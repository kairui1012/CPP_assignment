#include <iostream>
#include <string>

using namespace std;

struct Contact {
    string name;
    string phoneNumber;
    Contact* next;
};

void insertAtBeginning(Contact** head, string name, string phoneNumber){
    Contact* newContact = new Contact;
    newContact->name = name;
    newContact->phoneNumber = phoneNumber;
    newContact->next = *head;
    *head = newContact;
}

void insertAtEnd(Contact** head, string name, string phoneNumber){
    Contact* newContact = new Contact;
    newContact->name = name;
    newContact->phoneNumber = phoneNumber;
    newContact->next = nullptr;

    if (*head == nullptr){
        *head = newContact;
    } else {
        Contact* current = *head;
        while (current->next != nullptr){
            current = current->next;
        }
        current->next = newContact;
    }
}

void insertAtPosition(Contact** head, string name, string phoneNumber, int position){
    if (position <= 0){
        insertAtBeginning(head, name, phoneNumber);
    }else {
        Contact* newContact = new Contact;
        newContact->name = name;
        newContact->phoneNumber = phoneNumber;

        Contact* current = *head;
        int currentPosition = 0;
        while (current != nullptr && currentPosition < position -1){
            current = current->next;
            currentPosition++;
        }

        if (current == nullptr){
            // Position is beyond the end of the contact book
            delete newContact;
            return;
        }

        newContact->next = current->next;
        current->next = newContact;
    }
}


void deleteContact(Contact** head, string name){
    if (*head == nullptr){
        return;
    }

    if ((*head)->name == name){
        Contact* temp = *head;
        *head = (*head)->next;
        delete temp;
        return;
    }

    Contact* current = *head;
    while (current->next != nullptr && current->next->name != name){
        current = current->next;
    }

    if (current->next != nullptr){
        Contact* temp = current->next;
        current->next = current->next->next;
        delete temp;
    }
}

// Function to search for a contact in the contact book
bool searchContact(Contact* head, string name){
    Contact* current = head;
    while (current != nullptr){
        if (current->name == name){
            return true;
        }
        current = current->next;
    }
    return false;
}

// Function ot reverse the contact book
void reverseContactBook(Contact** head){
    Contact* prev = nullptr;
    Contact* current = *head;

    while (current != nullptr){
        Contact* next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    *head = prev;
}

// Function to find the middle contact in the contact book
Contact* findMiddleContact(Contact* head){
    if (head == nullptr){
        return nullptr;
    }

    Contact* slow = head;
    Contact* fast = head;

    while (fast != nullptr && fast->next != nullptr){
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;
}

bool hasCycle(Contact* head){
    Contact* slow = head;
    Contact* fast = head;

    while (fast != nullptr && fast->next != nullptr){
        slow = slow-> next;
        fast = fast->next->next;
        
        if (slow == fast){
             return true;
        }
    }
    return false;
}

Contact* mergeSortedContactBooks(Contact* head1, Contact* head2){
    if (head1 == nullptr){
        return head2;
    }

    if (head2 ==nullptr){
        return head1;
    }

    Contact* mergeHead = nullptr;
    Contact* tail = nullptr;

    if ( head1->name <= head2->name){
        mergeHead = head1;
        head1 = head1->next;
    }else{
        mergeHead = head2;
        head2 = head2->next;
    }


    tail = mergeHead;

    while (head1 != nullptr && head2 != nullptr){
        if (head1->name <= head2->name){
            tail->next = head1;
            head1 = head1->next;
        }else{
            tail->next = head2;
            head2 = head2->next;
        }
        tail = tail->next;
    }

    if (head1 != nullptr){
        tail->next = head1;
    }

    if (head2 != nullptr){
        tail->next = head2;
    }

    return mergeHead;
}

void displayContactBook(Contact* head){
    Contact* current = head;
    while(current != nullptr){
        cout << "Name: " << current->name << ", Phone Number: " << current->phoneNumber << endl;
        current = current->next;
    };
}


int main(){
    Contact* contactBook = nullptr;

    // Insert contacts at the beginning, end, and specific positions
    insertAtBeginning(&contactBook, "Taylor Swift", "123456789");
    insertAtEnd(&contactBook, "Adele", "987654321");
    insertAtPosition(&contactBook, "Doja Cat", "555555555", 1);

    // Display the contact book
    cout << "Contact Book after initial insertions:" << endl;
    displayContactBook(contactBook);
    cout << endl;

    // Delete a contact from the contact book
    deleteContact(&contactBook, "Adele");

    // Search for a contact in the contact book
    bool isContactFound = searchContact(contactBook, "Taylor Swift");
    if (isContactFound) {
        cout << "Contact found in the contact book." << endl;
    } else {
        cout << "Contact not found in the contact book." << endl;
    }
    cout << endl;

    // Reverse the contact book
    reverseContactBook(&contactBook);
    cout << "Reversed contact book:" << endl;
    displayContactBook(contactBook);
    cout << endl;

    // Find the middle contact in the contact book
    Contact* middleContact = findMiddleContact(contactBook);
    if (middleContact != nullptr) {
        cout << "Middle Contact: " << middleContact->name << ", " << middleContact->phoneNumber << endl;
    }

    // Detect if there is a cycle in the contact book (Assuming there is no cycle)
    bool hasCycleDetected = hasCycle(contactBook);
    if (hasCycleDetected) {
        cout << "Cycle detected in the contact book." << endl;
    } else {
        cout << "No cycle detected in the contact book." << endl;
    }
    cout << endl;

    // Merge two sorted contact books
    Contact* contactBook1 = nullptr;
    Contact* contactBook2 = nullptr;

    insertAtEnd(&contactBook1, "John Doe", "111111111");
    insertAtEnd(&contactBook1, "Mary Smith", "222222222");

    insertAtEnd(&contactBook2, "David Johnson", "333333333");
    insertAtEnd(&contactBook2, "Emily Brown", "444444444");

    Contact* mergedContactBook = mergeSortedContactBooks(contactBook1, contactBook2);
    cout << "Merged Contact Book:" << endl;
    displayContactBook(mergedContactBook);
    cout << endl;

    deleteContact(&contactBook, "Taylor Swift");
    displayContactBook(contactBook);
}
