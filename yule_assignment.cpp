#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <regex>
#include <iomanip>
using namespace std;

struct NewsNode
{
    string title, text, subject, date;

    NewsNode *next;

    NewsNode(const string &t = "", const string &te = "", const string &s = "", const string &d = "", NewsNode *n = nullptr)
        : title(t), text(te), subject(s), date(d), next(n) {}
};


NewsNode *parseCSVLineToNode(const string &line)
{
    string title, text, subject, date; // Use correct names
    string field;
    bool insideQuotes = false;
    int fieldIndex = 0;

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
            if (fieldIndex == 0)
                title = field;
            else if (fieldIndex == 1)
                text = field;
            else if (fieldIndex == 2)
                subject = field;

            field.clear();
            fieldIndex++;
        }
        else
        {
            field += c;
        }
    }

    date = field; // Last field

    return new NewsNode(title, text, subject, date); // Now matches struct correctly!
}


void insertDataToNode(NewsNode *&head, string filepath)
{
    ifstream file(filepath);

    if (!file)
    {
        cerr << "Failed to open the file." << endl;
        return;
    }

    string line;

    // Skip the CSV header without printing any message
    if (getline(file, line)){}

    while (getline(file, line))
    {
        if (line.empty())
            continue;

        // Parse the line and create a NewsNode
        NewsNode *newNode = parseCSVLineToNode(line);

        // Insert into the linked list
        if (!head)
        {
            head = newNode;
        }
        else
        {
            NewsNode *temp = head;
            while (temp->next)
                temp = temp->next;
            temp->next = newNode;
        }
    }

    file.close();
    cout << "Data successfully loaded into linked list." << endl;
}


void displayTesting(NewsNode *head, string mode)
{
    if (!head)
        return;

    int count = 0;

    while (head)
    {
        count += 1;
        cout << count << ". ";
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

string convertDateToSortableFormat(const string &date)
{
    unordered_map<string, string> monthMap = {
        {"January", "01"}, {"February", "02"}, {"March", "03"}, {"April", "04"}, {"May", "05"}, {"June", "06"}, {"July", "07"}, {"August", "08"}, {"September", "09"}, {"October", "10"}, {"November", "11"}, {"December", "12"}};

    istringstream ss(date);
    string monthStr, dayStr, yearStr;
    ss >> monthStr >> dayStr >> yearStr;

    // Handle invalid date format
    if (monthMap.find(monthStr) == monthMap.end() || dayStr.empty() || yearStr.empty())
    {
        // cerr << "Error: Invalid date format -> " << date << endl;
        return "9999/99/99"; // Assign a high value to avoid breaking sorting
    }

    // Remove the comma from day
    dayStr.pop_back();

    // Ensure day is two-digit format
    if (dayStr.length() == 1)
        dayStr = "0" + dayStr;

    return yearStr + "/" + monthMap[monthStr] + "/" + dayStr;
}

// Function to merge two sorted linked lists
NewsNode *mergeList(NewsNode *left, NewsNode *right, const string &mode)
{
    NewsNode dummy("", "", "", "");
    NewsNode *tail = &dummy;

    while (left && right)
    {
        bool condition = false;

        if (mode == "title")
            condition = (left->title < right->title);
        else if (mode == "text")
            condition = (left->text < right->text);
        else if (mode == "subject")
            condition = (left->subject < right->subject);
        else if (mode == "date")
        {
            string leftDate = convertDateToSortableFormat(left->date);
            string rightDate = convertDateToSortableFormat(right->date);
            condition = (leftDate < rightDate);
        }

        if (condition)
        {
            tail->next = left;
            left = left->next;
        }
        else
        {
            tail->next = right;
            right = right->next;
        }
        tail = tail->next;
    }

    tail->next = (left) ? left : right; // Attach remaining elements

    return dummy.next;
}

// Merge Sort for linked list
NewsNode *sortNode(NewsNode *head, const string &mode)
{
    if (!head || !head->next)
        return head;

    // Split the linked list into two halves
    NewsNode *slow = head;
    NewsNode *fast = head->next;

    while (fast && fast->next)
    {
        slow = slow->next;
        fast = fast->next->next;
    }

    NewsNode *second = slow->next;
    slow->next = nullptr;

    // Recursively sort both halves
    NewsNode *first = sortNode(head, mode);
    second = sortNode(second, mode);

    return mergeList(first, second, mode);
}

NewsNode *copyList(NewsNode *head)
{
    if (!head)
        return nullptr;

    NewsNode *newHead = new NewsNode{head->title, head->text, head->subject, head->date};
    NewsNode *newTail = newHead;
    NewsNode *current = head->next;

    while (current)
    {
        newTail->next = new NewsNode{current->title, current->text, current->subject, current->date};
        newTail = newTail->next;
        current = current->next;
    }

    return newHead;
}

int getLength(NewsNode *head)
{
    int count = 0;
    while (head)
    {
        count++;
        head = head->next;
    }
    return count;
}

NewsNode *findMidNode(NewsNode *head, int index)
{
    while (index-- > 0 && head)
    {
        head = head->next;
    }
    return head;
}

string categoryDifferentiator(NewsNode *finder, string category)
{
    string category_data;
    if (category == "title")
    {
        category_data = finder->title;
    }
    else if (category == "text")
    {
        category_data = finder->text;
    }
    else if (category == "subject")
    {
        category_data = finder->subject;
    }
    else if (category == "date")
    {
        category_data = finder->date;
    }
    return category_data;
}

int extractYear(const string &date)
{
    if (date.empty()) return -1; // Change 0 to -1 for better handling of invalid data

    regex yearPattern(R"(\b(19|20)\d{2}\b)"); 
    smatch match;

    if (regex_search(date, match, yearPattern))
    {
        return stoi(match.str());
    }

    return -1; // Ensure non-year data doesn't cause incorrect matches
}

NewsNode *binarySearch(NewsNode *head, string target_category, string target)
{
    int left = 0, right = getLength(head) - 1;
    NewsNode *firstMatch = nullptr;

    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        NewsNode *midNode = findMidNode(head, mid);
        string category_data = categoryDifferentiator(midNode, target_category);

        int compare = (target_category == "date") 
                      ? extractYear(category_data) - stoi(target) 
                      : category_data.compare(target);

        if (compare == 0)
        {
            firstMatch = midNode;
            right = mid - 1;
        }
        else if (compare < 0)
        {
            left = mid + 1;
        }
        else
        {
            right = mid - 1;
        }
    }

    if (!firstMatch)
        cout << "No match found for " << target_category << " = " << target << endl;

    return firstMatch;
}

void PrintSearchResult(NewsNode *head, string target_category, string target){
    NewsNode* start = binarySearch(head, target_category, target);

    while (start)
    {
        string category_data = categoryDifferentiator(start, target_category);

        // Stop when the category value no longer matches the target
        if (target_category == "date")
        {
            int category_year = extractYear(category_data);
            int target_year = stoi(target);

            if (category_year != target_year) break;
        }
        else
        {
            if (category_data != target) break;
        }

        // Print the matched result
        cout << "Title: " << start->title << endl;
        cout << "Text: " << start->text << endl;
        cout << "Subject: " << start->subject << endl;
        cout << "Date: " << start->date << endl;
        cout << endl;

        start = start->next;
    }
}

void writeToCSV(NewsNode *head, const string &filename)
{
    ofstream file(filename);

    // Check if file opened successfully
    if (!file.is_open())
    {
        cout << "Error: Unable to open file " << filename << endl;
        return;
    }

    // Write CSV header
    file << "Title,Text,Subject,Date\n";

    // Iterate through the linked list and write data
    NewsNode *current = head;
    while (current)
    {
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

int calculateTotalNewsNode(NewsNode* head)
{
    if (!head) return 0 ;
    int x = 0;

    while (head)
    {
        x ++;
        head = head->next;
    }

    return x;
}

// get month name in string based on month number
string getMonthName(int monthNumber) {  
    static unordered_map<int, string> monthMap = {
        {1, "January"}, {2, "February"}, {3, "March"}, {4, "April"}, {5, "May"}, {6, "June"},
        {7, "July"}, {8, "August"}, {9, "September"}, {10, "October"}, {11, "November"}, {12, "December"}
    };
    return monthMap.count(monthNumber) ? monthMap[monthNumber] : "Invalid Month";
}


// return month number in integer
int getMonthNum(const string &month) {  
    static unordered_map<string, int> monthMap = {
        {"January", 1}, {"February", 2}, {"March", 3}, {"April", 4}, {"May", 5}, {"June", 6},
        {"July", 7}, {"August", 8}, {"September", 9}, {"October", 10}, {"November", 11}, {"December", 12}
    };
    return monthMap.count(month) ? monthMap[month] : -1;
}

void extractNewsCount(NewsNode* head, const string &category, unordered_map<int, int> &countMap) {
    for (NewsNode* temp = binarySearch(sortNode(copyList(head), "subject"), "subject", category); temp; temp = temp->next) {
        stringstream ss(temp->date);
        string monthStr, dayStr, yearStr;
        ss >> monthStr >> dayStr >> yearStr;
        if (yearStr == "2016") countMap[getMonthNum(monthStr)]++;
    }
}

void calculateFakePercentage(NewsNode* fake, NewsNode* real) {
    unordered_map<int, int> fake_count, real_count;
    extractNewsCount(fake, "politics", fake_count);
    extractNewsCount(real, "politicsNews", real_count);

    cout << "Percentage of fake political news in each month of 2016\n\n";
    for (int month = 1; month <= 12; month++) {
        int total = fake_count[month] + real_count[month];
        int fake_percentage = (total == 0) ? 0 : (fake_count[month] * 100) / total;

        cout << left << setw(12) << getMonthName(month) << "| " 
             << string(fake_percentage, '*') << "\t" << fake_percentage << "%" << endl;
    }
    cout << "\nEach '*' represents 1% of the fake political news\n";
}

void mainMenu(NewsNode* fake, NewsNode* real){
    string choice;

    bool exit = false;

    while (!exit){
        cout << "---------------------- Main Menu ----------------------" << endl;
        cout << "1. Display data" << endl;
        cout << "2. Sort data" << endl;
        cout << "3. Search data" << endl;
        cout << "4. Write data to CSV" << endl;
        cout << "5. Calculate total news node" << endl;
        cout << "6. Calculate fake news percentage" << endl;
        cout << "7. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == "1"){
            cout << "-------------- Displaying Menu --------------" << endl;
            cout << "1. Display fake news data" << endl;
            cout << "2. Display true news data" << endl;
            cout << "3. Display both" << endl;
            cout << "4. Return to Main Menu" << endl;
            cout << "Enter your choice: ";
            cin >> choice;

            if (choice == "1"){
                cout << "-------------- Displaying fake news data --------------" << endl;
                displayTesting(fake, "title");
            }
            else if (choice == "2"){
                cout << "-------------- Displaying true news data --------------" << endl;
                displayTesting(real, "title");
            }
            else if (choice == "3"){
                cout << "-------------- Displaying fake news data --------------" << endl;
                displayTesting(fake, "title");
                cout << "-------------- Displaying true news data --------------" << endl;
                displayTesting(real, "title");
            }
            else if (choice == "4"){
                choice = "-1";
                continue;
            }
            else{
                cout << "Invalid choice. Please try again." << endl;
            }
            choice = "-1";
            continue;
        }
        else if (choice == "2"){
            string sort_choice;
            cout << "Enter the category you want to sort (title, text, subject, date): ";
            cin >> sort_choice;

            cout << "Sorting fake news data based on " << sort_choice << "..." << endl;
            fake = sortNode(fake, sort_choice);
            cout << "Sorting true news data based on " << sort_choice << "..." << endl;
            real = sortNode(real, sort_choice);

            cout << "Data sorted successfully." << endl;
        }
        else if (choice == "3"){
            string search_choice, target;
            cout << "Enter the category you want to search (title, text, subject, date): ";
            cin >> search_choice;
            cout << "Enter the target value: ";
            cin.ignore();
            getline(cin, target);

            cout << "Searching for " << search_choice << " = " << target << "..." << endl;
            cout << "-------------- Search Result --------------" << endl;
            cout << "-------------- Fake News --------------" << endl;
            PrintSearchResult(fake, search_choice, target);
            cout << "-------------- True News --------------" << endl;
            PrintSearchResult(real, search_choice, target);
        }
        else if (choice == "4"){
            string filename;
            cout << "Enter the filename you want to write to: ";
            cin >> filename;

            cout << "Writing fake news data to " << filename << "..." << endl;
            writeToCSV(fake, filename);
            cout << "Writing true news data to " << filename << "..." << endl;
            writeToCSV(real, filename);
        }
        else if (choice == "5"){
            cout << "Total news node in fake news data: " << calculateTotalNewsNode(fake) << endl;
            cout << "Total news node in true news data: " << calculateTotalNewsNode(real) << endl;
        }
        else if (choice == "6"){
            calculateFakePercentage(fake, real);
        }
        else if (choice == "7"){
            exit = true;
        }
        else{
            cout << "Invalid choice. Please try again." << endl;
        }

    }    
}

int main()
{
    NewsNode* fakeNewsHead = nullptr;
    NewsNode* trueNewsHead = nullptr; 
    insertDataToNode(fakeNewsHead, "cleaned_fake.csv");
    insertDataToNode(trueNewsHead, "true.csv");
    mainMenu(fakeNewsHead, trueNewsHead);

    return 0;
}