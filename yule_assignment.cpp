#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>
using namespace std;

struct NewsNode
{
    string title, text, subject, date;

    NewsNode* next;

    NewsNode(const string& t = "", const string& te = "", const string& s = "", const string& d = "", NewsNode* n = nullptr)
        : title(t), text(te), subject(s), date(d), next(n) {}
};

int parseCSVLine(const string &line, string fields[], int expectedColumns)
{
    int columnIndex = 0;
    bool insideQuotes = false;
    string field;
    
    for (size_t i = 0; i < line.length(); i++) 
    {
        char c = line[i];

        if (c == '"') 
        {
            if (insideQuotes && i + 1 < line.length() && line[i + 1] == '"') 
            {
                field += '"';
                i++;
            } 
            else 
            {
                insideQuotes = !insideQuotes;
            }
        } 
        else if (c == ',' && !insideQuotes)
        {
            if (columnIndex < expectedColumns) 
            {
                fields[columnIndex++] = field;
                field.clear();
            }
        } 
        else
        {
            field += c;
        }
    }

    if (!field.empty() && columnIndex < expectedColumns) 
    {
        fields[columnIndex++] = field;
    }

    return columnIndex;
}

void insertDataToNode(NewsNode*& head) 
{
    ifstream file("C:\\Users\\yeo yu le\\Downloads\\cleaned_fake3.0.csv");
    const int MAX_COLUMNS = 4;

    if (!file) 
    {
        cerr << "Failed to open the file." << endl;
        return;
    }

    string line;
    string fields[MAX_COLUMNS];

    if (getline(file, line)) 
    {
        cout << "Skipping CSV header: " << line << endl; 
    }

    bool isHeader = true;

    while (getline(file, line))
    {
        if (line.empty()) continue;
        
        int columnCount = parseCSVLine(line, fields, MAX_COLUMNS);
        if (columnCount != MAX_COLUMNS) continue;
        
        NewsNode* newNode = new NewsNode{fields[0], fields[1], fields[2], fields[3], nullptr};

        if (!head) 
        {
            head = newNode;
        } 
        else 
        {
            NewsNode* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newNode;
        }
    }

    file.close();
    cout << "Data successfully loaded into linked list." << endl;
}

void displayTesting(NewsNode* head, string mode)
{
    if (!head) return;

    while (head) {
        if (mode == "title")
            cout << head->title << endl;
        else if (mode == "text")
            cout << head->text << endl;
        else if (mode == "subject")
            cout << head->subject << endl;
        else if (mode == "date")
            cout << head->date << endl;

        head = head->next;
    }
}

string convertDateToSortableFormat(const string& date) {
    unordered_map<string, string> monthMap = {
        {"January", "01"}, {"February", "02"}, {"March", "03"}, {"April", "04"},
        {"May", "05"}, {"June", "06"}, {"July", "07"}, {"August", "08"},
        {"September", "09"}, {"October", "10"}, {"November", "11"}, {"December", "12"}
    };

    istringstream ss(date);
    string monthStr, dayStr, yearStr;
    ss >> monthStr >> dayStr >> yearStr;

    // Handle invalid date format
    if (monthMap.find(monthStr) == monthMap.end() || dayStr.empty() || yearStr.empty()) {
        cerr << "Error: Invalid date format -> " << date << endl;
        return "9999/99/99"; // Assign a high value to avoid breaking sorting
    }

    // Remove the comma from day
    dayStr.pop_back();
    
    // Ensure day is two-digit format
    if (dayStr.length() == 1) dayStr = "0" + dayStr;

    return yearStr + "/" + monthMap[monthStr] + "/" + dayStr;
}

// Function to merge two sorted linked lists
NewsNode* mergeList(NewsNode* left, NewsNode* right, const string& mode) {
    NewsNode dummy("", "", "", "");
    NewsNode* tail = &dummy;

    while (left && right) {
        bool condition = false;

        if (mode == "title") condition = (left->title < right->title);
        else if (mode == "text") condition = (left->text < right->text);
        else if (mode == "subject") condition = (left->subject < right->subject);
        else if (mode == "date") {
            string leftDate = convertDateToSortableFormat(left->date);
            string rightDate = convertDateToSortableFormat(right->date);
            condition = (leftDate < rightDate);
        }

        if (condition) {
            tail->next = left;
            left = left->next;
        } else {
            tail->next = right;
            right = right->next;
        }
        tail = tail->next;
    }

    tail->next = (left) ? left : right; // Attach remaining elements

    return dummy.next;
}

// Merge Sort for linked list
NewsNode* sortNode(NewsNode* head, const string& mode) {
    if (!head || !head->next) return head;

    // Split the linked list into two halves
    NewsNode* slow = head;
    NewsNode* fast = head->next;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    NewsNode* second = slow->next;
    slow->next = nullptr;

    // Recursively sort both halves
    NewsNode* first = sortNode(head, mode);
    second = sortNode(second, mode);

    return mergeList(first, second, mode);
}


NewsNode* copyList(NewsNode* head) {
    if (!head) return nullptr;

    NewsNode* newHead = new NewsNode{head->title, head->text, head->subject, head->date};
    NewsNode* newTail = newHead;
    NewsNode* current = head->next;

    while (current) {
        newTail->next = new NewsNode{current->title, current->text, current->subject, current->date};
        newTail = newTail->next;
        current = current->next;
    }

    return newHead;
}

// Binary Search find the first matching result's pointer
NewsNode* findFirst(NewsNode* head, const string& target) {
    // Base case: empty list
    if (!head) return nullptr;

    // Find the middle node using slow and fast pointers
    NewsNode* slow = head;
    NewsNode* fast = head->next;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // Split the list into two halves
    NewsNode* second = slow->next;
    slow->next = nullptr;
    NewsNode* first = head;

    // Extract the year from the middle node's date
    string midYear;
    if (!slow->date.empty() && slow->date.length() >= 4) {
        midYear = slow->date.substr(slow->date.length() - 4, 4);
    } else {
        // Handle invalid date format
        return nullptr;
    }

    // Compare midYear with target
    if (midYear < target) {
        // Search in the second half
        return findFirst(second, target);
    } else if (midYear > target) {
        // Search in the first half
        return findFirst(first, target);
    } else {
        // midYear == target: search in the first half for the earliest match
        NewsNode* firstMatch = findFirst(first, target);
        return (firstMatch) ? firstMatch : slow;
    }
}

// Binary Search find the last matching result's pointer
NewsNode* findLast(NewsNode* head, string target) {
    if (!head) return nullptr;

    NewsNode* slow = head;
    NewsNode* fast = head->next;
    NewsNode* lastMatch = nullptr;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    NewsNode* second = slow->next;
    slow->next = nullptr;
    NewsNode* first = head;

    string midYear = (slow->date).substr(slow->date.length() - 4, 4);

    if (midYear < target) {
        return findLast(second, target);
    } 
    else if (midYear > target) {
        return findLast(first, target);
    } 
    else {
        lastMatch = findLast(second, target);  // 继续往右找最晚的
        return (lastMatch) ? lastMatch : slow;
    }
}

void printRange(NewsNode* start, NewsNode* end) {
    if (!start || !end) {
        cout << "No matching news found!" << endl;
        return;
    }

    NewsNode* current = start;
    while (current && current != end->next) {  // 遍历从 start 到 end
        cout << "Date: " << current->date << ", Title: " << current->title << endl;
        current = current->next;
    }
}


void BinarySearch(NewsNode* head, string target){
    if (!head) return;
    NewsNode* tempHead = copyList(head);

    tempHead = sortNode(tempHead, "date");

    NewsNode* first = findFirst(tempHead, target);
    NewsNode* last = findLast(tempHead, target);

    printRange(first, last);
}

void writeToCSV(NewsNode* head, const string& filename) {
    ofstream file(filename);

    // Check if file opened successfully
    if (!file.is_open()) {
        cout << "Error: Unable to open file " << filename << endl;
        return;
    }

    // Write CSV header
    file << "Title,Text,Subject,Date\n";

    // Iterate through the linked list and write data
    NewsNode* current = head;
    while (current) {
        file << "\"" << current->title << "\","
             << "\"" << current->text << "\","
             << "\"" << current->subject << "\","
             << "\"" << current->date << "\"\n";
        current = current->next;
    }

    // Close the file
    file.close();
    cout << "Data successfully written to " << filename << endl;
}

int main(){
    NewsNode* newsHead = nullptr;
    insertDataToNode(newsHead);

    cout << "Start Sorting" << endl;
    newsHead = sortNode(newsHead, "date");

    cout << "end Sorting" << endl;
    writeToCSV(newsHead, "sorted_News.csv");

    //BinarySearch(newsHead, "2016");
    return 0;
}