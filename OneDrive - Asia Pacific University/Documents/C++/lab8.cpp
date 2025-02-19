#include <iostream>
using namespace std;

struct Node{
    int data;
    Node* next;
};

// Function to insert at the end
void insert(Node*& head, int value) {
    Node* newNode = new Node{value, nullptr};
    if (!head) {
        head = newNode;
        return;
    }
    Node* temp = head;
    while (temp->next) {
        temp = temp->next;
    }
    temp->next = newNode;
}

// Function to delete a node by value
void deleteNode(Node*& head, int value) {
    if (!head) return;

    // If head is to be deleted
    if (head->data == value) {
        Node* temp = head;
        head = head->next;
        delete temp;
        return;
    }

    Node* temp = head;
    while (temp->next && temp->next->data != value) {
        temp = temp->next;
    }

    if (temp->next) {
        Node* toDelete = temp->next;
        temp->next = temp->next->next;
        delete toDelete;
    }
}

// Function to print the linked list
void printList(Node* head) {
    Node* temp = head;
    while (temp) {
        cout << temp->data << " -> ";
        temp = temp->next;
    }
    cout << "NULL" << endl;
}

int main(){
    Node* head = nullptr;
    insert(head,10);
    insert(head,20);
    insert(head,30);

    // Original list
    cout << "Original list: ";
    printList(head);

    // Updated list
    deleteNode(head, 20);
    cout << "Updated list: ";
    printList(head);
}