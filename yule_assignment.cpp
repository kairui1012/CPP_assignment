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

struct monthNode{
    string month;
    int count;
    monthNode* next;
};

NewsNode *parseCSVLineToNode(const string &line)
{
    string title, date, category, content;
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
            // 按照 fieldIndex 直接存入 NewsNode 对应字段
            if (fieldIndex == 0)
                title = field;
            else if (fieldIndex == 1)
                date = field;
            else if (fieldIndex == 2)
                category = field;

            field.clear();
            fieldIndex++;
        }
        else
        {
            field += c;
        }
    }

    // 最后一个字段
    content = field;

    // 直接创建 NewsNode
    return new NewsNode(title, date, category, content);
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

    // 跳过 CSV 头部
    if (getline(file, line))
    {
        cout << "Skipping CSV header: " << line << endl;
    }

    while (getline(file, line))
    {
        if (line.empty())
            continue;

        // 解析行并创建 NewsNode
        NewsNode *newNode = parseCSVLineToNode(line);

        // 插入 Linked List
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

    while (head)
    {
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

NewsNode *binarySearchFirst(NewsNode *head, string target_category, string target)
{
    int left = 0;
    int right = getLength(head) - 1;
    NewsNode *firstMatch = nullptr;

    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        NewsNode *midNode = findMidNode(head, mid);

        string category_data = categoryDifferentiator(midNode, target_category);
        if (target_category == "date")
        {
            int category_year = extractYear(category_data);
            int target_year = stoi(target);

            if (category_year == target_year)
            {
                firstMatch = midNode;
                right = mid - 1;
            }
            else if (category_year < target_year)
            {
                left = mid + 1;
            }
            else
            {
                right = mid - 1;
            }
        }
        else{
            if (category_data == target)
            {
                firstMatch = midNode;
                right = mid - 1;
            }
            else if (category_data < target)
            {
                left = mid + 1;
            }
            else
            {
                right = mid - 1;
            }
        }

    }
    return firstMatch;
}

NewsNode* BinarySearchResult(NewsNode *head, string target_category, string target)
{
    NewsNode *start = binarySearchFirst(head, target_category, target);
    if (!start)
    {
        cout << "No match found for " << target_category << " = " << target << endl;
        return nullptr;
    }
    return start;

}

void PrintSearchResult(NewsNode *head, string target_category, string target){
    NewsNode* start = BinarySearchResult(head, target_category, target);

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

int getMonthNumber(const string &month)  // Convert month name to corresponding number
{
    if (month == "January" || month == "Jan") return 1;
    if (month == "February" || month == "Feb") return 2;
    if (month == "March" || month == "Mar") return 3;
    if (month == "April" || month == "Apr") return 4;
    if (month == "May") return 5;
    if (month == "June" || month == "Jun") return 6;
    if (month == "July" || month == "Jul") return 7;
    if (month == "August" || month == "Aug") return 8;
    if (month == "September" || month == "Sep") return 9;
    if (month == "October" || month == "Oct") return 10;
    if (month == "November" || month == "Nov") return 11;
    if (month == "December" || month == "Dec") return 12;
    return -1;
}

string getMonthName(int monthNumber) {
    switch (monthNumber) {
        case 1: return "January";
        case 2: return "February";
        case 3: return "March";
        case 4: return "April";
        case 5: return "May";
        case 6: return "June";
        case 7: return "July";
        case 8: return "August";
        case 9: return "September";
        case 10: return "October";
        case 11: return "November";
        case 12: return "December";
        default: return "Invalid Month";
    }
}

void calculateFakePercentage(NewsNode* fake, NewsNode* real)
{
    // Step 1: Sort and extract 2016 fake political news
    NewsNode* fake_news = copyList(fake);
    fake_news = sortNode(fake_news, "subject"); 
    NewsNode* fake_politic_head = BinarySearchResult(fake_news, "subject", "politics");

    if (!fake_politic_head) {
        cout << "No fake political news found.\n";
        return;
    }

    // Step 2: Extract 2016 fake political news
    unordered_map<int, int> fake_count;
    unordered_map<int, int> real_count;

    NewsNode* temp = fake_politic_head;
    while (temp) {
        stringstream ss(temp->date);
        string monthStr, dayStr, yearStr;
        ss >> monthStr >> dayStr >> yearStr;

        if (yearStr == "2016") {
            int month = getMonthNumber(monthStr);
            if (month != -1) fake_count[month]++;
        }
        temp = temp->next;
    }

    // Step 3: Sort and extract 2016 real political news
    NewsNode* real_news = copyList(real);
    real_news = sortNode(real_news, "subject");
    NewsNode* real_politic_head = BinarySearchResult(real_news, "subject", "politicsNews");

    if (!real_politic_head) {
        cout << "No real political news found.\n";
        return;
    }

    temp = real_politic_head;
    while (temp) {
        stringstream ss(temp->date);
        string monthStr, dayStr, yearStr;
        ss >> monthStr >> dayStr >> yearStr;

        if (yearStr == "2016") {
            int month = getMonthNumber(monthStr);
            if (month != -1) real_count[month]++;
        }
        temp = temp->next;
    }

    // Step 4: Print percentages
    cout << "Percentage of fake political news in each month of 2016\n\n";

    for (int month = 1; month <= 12; month++) {
        int fake_news_count = fake_count[month];
        int real_news_count = real_count[month];
        int total = fake_news_count + real_news_count;

        cout << endl << fake_news_count << " " << real_news_count << " " << total << endl;
        int fake_percentage = (total == 0) ? 0 : (fake_news_count * 100) / total;

        cout << left << setw(12) << getMonthName(month) << "| ";
        for (int i = 1; i <= fake_percentage; i++) {
            cout << "*";
        }
        cout << "\t" << fake_percentage << "%" << endl;
    }

    cout << "\nEach '*' represents 1% of the fake political news\n";
}



int main()
{
    NewsNode* fakeNewsHead = nullptr;
    NewsNode* trueNewsHead = nullptr;
    insertDataToNode(fakeNewsHead, "C:\\Users\\yeo yu le\\Downloads\\final_fake.csv");
    insertDataToNode(trueNewsHead, "C:\\Users\\yeo yu le\\Downloads\\true.csv");

    calculateFakePercentage(fakeNewsHead, trueNewsHead);

    return 0;
}