#include <iostream>
#include <regex>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <cctype>
#include <iomanip>
#include <sstream>

using namespace std;

struct NewsNode
{
    string title,text,subject,date;

    NewsNode* next;

};

struct WordNode
{
    string text;
    int count;
    WordNode* next;
};

string convertDateToSortableFormat(const string& date) 
{
    size_t firstDash = date.find('-');
    size_t secondDash = date.find('-', firstDash + 1);

    if (firstDash == string::npos || secondDash == string::npos) {
        return date;
    }

    string day = date.substr(firstDash + 1, secondDash - firstDash - 1);
    string month = date.substr(0, firstDash);
    string year = date.substr(secondDash + 1);

    if (day.length() == 1) day = "0" + day;
    if (month.length() == 1) month = "0" + month;

    return month + "/" + day + "/" + year;
}

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
    ifstream file("/Users/kairuisam/Desktop/c++/Data Cleaning/cleaned_fake.csv");
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

NewsNode* mergeList(NewsNode* left, NewsNode* right, string mode)
{
    NewsNode* dummy = new NewsNode();
    NewsNode* tail = dummy;

    bool condition = false;
        
    while (left && right)
    {
        if (mode == "title") condition = (left->title < right->title);
        else if (mode == "text") condition = (left->text < right->text);
        else if (mode == "subject") condition = (left->subject < right->subject);
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


    tail->next = (left) ? left : right;

    NewsNode* sortedHead = dummy->next;
    delete dummy;
    return sortedHead;
}

NewsNode* sortNode(NewsNode* head, string mode) 
{
    if (!head || !head->next) return head;

    NewsNode* slow = head;
    NewsNode* fast = head->next;

    while (fast && fast->next)
    {
        slow = slow->next;
        fast = fast->next->next;
    }

    NewsNode* second = slow->next;
    slow->next = nullptr;
    
    NewsNode* first = sortNode(head, mode);
    second = sortNode(second, mode);

    return mergeList(first, second, mode);
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

string formatCSVField(const string& field) {
    string formattedField = field;

    size_t pos;
    while ((pos = formattedField.find("\n")) != string::npos) {
        formattedField.replace(pos, 1, " ");
    }
    while ((pos = formattedField.find("\r")) != string::npos) {
        formattedField.replace(pos, 1, " ");
    }

    // 如果字段内有 `,` 或 `"`，需要加引号，并且内部 `"` 需要转义成 `""`
    if (formattedField.find(',') != string::npos || formattedField.find('"') != string::npos) {
        string temp = "\"";
        for (char c : formattedField) {
            if (c == '"') temp += "\"\"";  // CSV 规则，`"` 需要双写
            else temp += c;
        }
        temp += "\"";
        formattedField = temp;
    }

    return formattedField;
}

void outputSortNewsFile(NewsNode* head) 
{
    ofstream outFile("sorted_News.csv", ios::binary);

    if (!outFile) {
        cerr << "Failed to create the file: text.csv" << endl;
        return;
    }

    outFile << "\xEF\xBB\xBF"; 

    outFile << "Title,Text,Subject,Date\n";

    
    while (head) 
    {
        // string formattedDate = convertDateToSortableFormat(head->date);

        outFile << formatCSVField(head->title) << ","
                << formatCSVField(head->text) << ","
                << formatCSVField(head->subject) << ","
                << formatCSVField(head->date) << "\n";
        head = head->next;
    }

    outFile.close();
    cout << "Data successfully written to sorted_News.csv" << endl;
    
}

regex wordPattern(R"([\s,.\(\)\[\]{}<>:;!?\"\'\/-]+)"); 

void insertDataToWordNode(WordNode*& head, const string& word) 
{
    WordNode* current = head;
    WordNode* prev = nullptr;

    while (current) {
        if (current->text == word) 
        {
            current->count++;
            if (prev)
            {
                prev->next = current->next;
                current->next = head;
                head = current;
            }
            return;
        }
        prev = current;
        current = current->next;
    }

    WordNode* newNode = new WordNode{word, 1, head};
    head = newNode;
}

WordNode* mergeTextData(WordNode* left, WordNode* right) {
    WordNode* dummy = new WordNode();
    WordNode* tail = dummy;

    while (left && right) {
        if (left->count >= right->count) { 
            tail->next = left;
            left = left->next;
        } else {
            tail->next = right;
            right = right->next;
        }
        tail = tail->next;
    }

    tail->next = left ? left : right;

    WordNode* mergedHead = dummy->next;
    delete dummy;
    return mergedHead;
}

WordNode* sortWordCount(WordNode* head) 
{
    if (!head || !head->next) return head;

    WordNode* slow = head;
    WordNode* fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    WordNode* second = slow->next;
    slow->next = nullptr;

    WordNode* left = sortWordCount(head);
    WordNode* right = sortWordCount(second);

    return mergeTextData(left, right);
}

void outputSortWordsFile(WordNode* head) {
    ofstream outFile("sorted_Words.csv", ios::binary);
    if (!outFile) {
        cerr << "Failed to create sorted_Words.csv" << endl;
        return;
    }

    outFile << "\xEF\xBB\xBF"; 
    outFile << "Word,Count\n";

    WordNode* current = head;  
    while (current) {
        outFile << formatCSVField(current->text) << ","
                << current->count << "\n";
        current = current->next;
    }

    outFile.close();
    cout << "Word data written to sorted_Words.csv" << endl;
}

WordNode* calculateWords(NewsNode* newsHead) { 
    if (!newsHead)
    {
        cout << "No data" << endl;
        return nullptr; 
    }

    WordNode* wordHead = nullptr;
    int lines = 0;

    while (newsHead) 
    {
        string text = newsHead->text;

        transform(text.begin(), text.end(), text.begin(), ::tolower);

        sregex_token_iterator it(text.begin(), text.end(), wordPattern, -1);
        sregex_token_iterator end;
        for (; it != end; ++it) 
        {
            if (!it->str().empty()) 
            {
                insertDataToWordNode(wordHead, it->str());
            }
        }

        lines++;

        lines++;
        if (lines % 100 == 0) 
        {  
            cout << "Processed " << lines << " lines" << endl;
        }
        
        newsHead = newsHead->next;
    }

    return wordHead; 
}

int main()
{
    NewsNode* newsHead = nullptr;
    insertDataToNode(newsHead); 

    WordNode* wordHead = calculateWords(newsHead);

    if (wordHead) 
    {
        wordHead = sortWordCount(wordHead);
        outputSortWordsFile(wordHead);

        while (wordHead) 
        {
            WordNode* temp = wordHead;
            wordHead = wordHead->next;
            delete temp;
        }
    }

    cout << endl << calculateTotalNewsNode(newsHead) << endl;
    return 0;
}
