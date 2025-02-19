#include <iostream>
using namespace std;

struct Node{
    int data;
    Node* next;
};

// Insert at end
void insertEnd(Node*& head, int value) {
    Node* newNode = new Node{value, NULL};
    if (!head) {
        head = newNode;
        return;
    }
    Node* temp = head;
    while (temp->next) temp = temp->next;
    temp->next = newNode;
}

// Delete by value
void deleteNode(Node*& head, int value) {
    if (!head) return;
    if (head->data == value) {
        Node* temp = head;
        head = head->next;
        delete temp;
        return;
    }
    Node* temp = head;
    while (temp->next && temp->next->data != value) temp = temp->next;
    if (!temp->next) return; // Value not found
    Node* delNode = temp->next;
    temp->next = temp->next->next;
    delete delNode;
}

// Display the list
void display(Node* head) {
    while (head) {
        cout << head->data << " -> ";
        head = head->next;
    }
    cout << "NULL\n";
}

// Insert at the beginning
void insertFront(Node*& head, int value) {
    Node* newNode = new Node{value, NULL, head};
    if (head) head->prev = newNode;
    head = newNode;
}

// Delete from the end
void deleteEnd(Node*& head) {
    if (!head) return;
    if (!head->next) {
        delete head;
        head = NULL;
        return;
    }
    Node* temp = head;
    while (temp->next) temp = temp->next;
    temp->prev->next = NULL;
    delete temp;
}

// Display forward
void displayForward(Node* head) {
    while (head) {
        cout << head->data << " <-> ";
        head = head->next;
    }
    cout << "NULL\n";
}

// Display reverse
void displayReverse(Node* head) {
    if (!head) return;
    while (head->next) head = head->next;
    while (head) {
        cout << head->data << " <-> ";
        head = head->prev;
    }
    cout << "NULL\n";
}


void insertEnd(Node*& tail, int value) {
    Node* newNode = new Node{value, NULL};
    if (!tail) {
        tail = newNode;
        tail->next = tail; // Pointing to itself (circular property)
        return;
    }
    newNode->next = tail->next;
    tail->next = newNode;
    tail = newNode;
}

// Delete from the front of Circular Singly Linked List
void deleteFront(Node*& tail) {
    if (!tail) return;
    Node* head = tail->next;
    if (head == tail) { // Only one node
        delete head;
        tail = NULL;
        return;
    }
    tail->next = head->next;
    delete head;
}

// Display Circular Singly Linked List
void display(Node* tail) {
    if (!tail) return;
    Node* temp = tail->next;
    do {
        cout << temp->data << " -> ";
        temp = temp->next;
    } while (temp != tail->next);
    cout << "(back to start)\n";
}


int main(){
    Node* head = nullptr;
    Node* tail = nullptr;

    // Q1
    for (int i = 1; i < 4; i++){
        insertEnd(head,i*10);
    }
    deleteNode(head, 20);
    display(head);

    // Q2
    for (int i = 1; i < 4; i++){
        insertEnd(tail,i*10);
    }
    deleteEnd(head);
    displayForward(head);
    displayReverse(head);

    // Q3
    for (int i = 1; i < 4; i++){
        insertEnd(tail,i*10);
    }
    deleteFront(tail);
    display(tail);

    // Q4
    for (int i = 1; i < 4; i++){
        insertEnd(tail,i*10);
    }
    deleteFront(tail);
    displayForward(tail);
    displayReverse(tail);
}