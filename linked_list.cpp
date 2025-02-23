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
#include <unordered_map>
#include <chrono>
#include <algorithm>
#ifdef _WIN32
    #include <windows.h>
    #include <psapi.h>
#else
    #include <sys/resource.h>
    #include <unistd.h>
#endif

using namespace std;


struct NewsNode
    {
        string title,text,subject,date;
        NewsNode* next;

        NewsNode(const string &t = "", const string &te = "", const string &s = "", const string &d = "", NewsNode *n = nullptr)
        : title(t), text(te), subject(s), date(d), next(n) {}
    };


struct WordNode
    {
        string text;
        int count;
        WordNode* next;
    };

// Structure representing a node in a Trie (prefix tree)
struct TrieNode {
    char c;             
    int count;                
    TrieNode* nextSibling;     // Pointer to the next sibling node at the same level
    TrieNode* firstChild;      // Pointer to the first child node (deeper level)
    // Constructor to initialize TrieNode
    TrieNode(char ch) : c(ch), count(0), nextSibling(nullptr), firstChild(nullptr) {}
};


string getMonthNumber(const string &month)  // Convert month name to corresponding number
{
    if (month == "January" || month == "Jan") return "01";
    if (month == "February" || month == "Feb") return "02";
    if (month == "March" || month == "Mar") return "03";
    if (month == "April" || month == "Apr") return "04";
    if (month == "May") return "05";
    if (month == "June" || month == "Jun") return "06";
    if (month == "July" || month == "Jul") return "07";
    if (month == "August" || month == "Aug") return "08";
    if (month == "September" || month == "Sep") return "09";
    if (month == "October" || month == "Oct") return "10";
    if (month == "November" || month == "Nov") return "11";
    if (month == "December" || month == "Dec") return "12";
    return "";
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

void printNewsNodes(NewsNode* head) {
    if (!head) {
        cout << "The linked list is empty." << endl;
        return;
    }

    NewsNode* temp = head;
    while (temp) {
        cout << "Title: " << temp->title << endl;
        cout << "Text: " << temp->text << endl;
        cout << "Subject: " << temp->subject << endl;
        cout << "Date: " << temp->date << endl;
        cout << "----------------------" << endl;
        temp = temp->next;
    }
}


class News
{
    public:

    void insertDataToNode(NewsNode*& head,string filename) 
    {

        ifstream file(filename);
    
        if (!file) 
        {
            cerr << "Failed to open the file." << endl;
            return;
        }
    
        string line;
        bool isHeader = true;
    
        while (getline(file, line))
        {
            if (isHeader) { // Skip the CSV header
                isHeader = false;
                continue;
            }
    
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


    // Function to count the total number of nodes in the linked list
    int calculateTotalNewsNode(NewsNode* head)
    {
        if (!head) return 0; // Return 0 if the list is empty
        int x = 0;
        while (head) {
            x++; // Increment counter for each node
            head = head->next; // Move to the next node
        }
        return x;
    }


    // Function to get the first non-space character from a string
    string getFirstChar(const string& str) {
        for (char c : str) {
            if (c != ' ') return string(1, c); // Return the first non-space character as a string
        }
        return ""; // Return empty string if no valid character is found
    }


    bool customCompare(const string& left, const string& right) {
        const char* Left = left.c_str();
        const char* Right = right.c_str();
    
        // 跳过左侧和右侧字符串前导空格
        while (*Left == ' ') Left++;
        while (*Right == ' ') Right++;
    
        // **确保双引号开头的字符串永远排在最前面**
        bool leftIsQuote = (*Left == '"');
        bool rightIsQuote = (*Right == '"');
        
        if (leftIsQuote && !rightIsQuote) return true;  // 左侧是双引号，右侧不是 → 左侧优先
        if (rightIsQuote && !leftIsQuote) return false; // 右侧是双引号，左侧不是 → 右侧优先
    
        // **正常比较逻辑**
        while (*Left && *Right) {
            if (*Left != *Right) {
                return *Left < *Right; // 按字母顺序比较
            }
            Left++;
            Right++;
    
            // 继续跳过空格
            while (*Left == ' ') Left++;
            while (*Right == ' ') Right++;
        }
    
        return *Left == '\0' && *Right != '\0';
    }
    

    // Function to merge two sorted linked lists based on a specified mode
    NewsNode* mergeNewsNode(NewsNode* left, NewsNode* right, string mode)
    {
        NewsNode* dummy = new NewsNode(); // Create a dummy node to simplify merging logic
        NewsNode* tail = dummy; // Pointer to track the last node in the merged list

        bool condition = false; // Boolean flag to store comparison result

        while (left && right) // Traverse both lists while neither is empty
        {
            if (mode == "title") 
                condition = customCompare(left->title, right->title); // Compare by title
            else if (mode == "text") 
                condition = customCompare(left->text, right->text); // Compare by text
            else if (mode == "subject") 
                condition = customCompare(left->subject, right->subject); // Compare by subject
            else if (mode == "date") 
            {
                // Convert dates to a sortable format before comparison
                string leftDate = convertDateToSortableFormat(left->date);
                string rightDate = convertDateToSortableFormat(right->date);
                condition = (leftDate < rightDate); // Compare by date
            }

            if (condition) {
                tail->next = left; // Attach the smaller node to the merged list
                left = left->next; // Move to the next node in the left list
            } else {
                tail->next = right; // Attach the smaller node to the merged list
                right = right->next; // Move to the next node in the right list
            }
            tail = tail->next; // Move the tail pointer forward
        }

        // Attach the remaining nodes from the non-empty list
        tail->next = left ? left : right;

        return dummy->next; // Return the head of the merged list (skip the dummy node)
    }


    NewsNode* sortNewsNode(NewsNode* head, string mode) 
    {
        if (!head || !head->next) 
            return head;  // Base case: 0 or 1 element, already sorted

        // Use slow and fast pointers to find the middle
        NewsNode* slow = head;
        NewsNode* fast = head->next; // Start fast one step ahead

        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }

        // Split the list into two halves
        NewsNode* second = slow->next;
        slow->next = nullptr;  // Cut the first half

        // Recursively sort both halves
        NewsNode* first = sortNewsNode(head, mode);
        second = sortNewsNode(second, mode);
        return mergeNewsNode(first, second, mode);
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


};


// Function to remove duplicate nodes from a linked list based on the subject field,
// while keeping all occurrences of a specified subject.
NewsNode* deleteDuplicates(NewsNode* head, const string& subject) {
    if (!head) return nullptr;  // Handle empty list case

    NewsNode dummy;  // Dummy node to simplify deletion logic
    dummy.next = head;
    NewsNode* prev = &dummy;

    while (head) {
        bool duplicate = false;
        NewsNode* temp = head;

        // Identify consecutive duplicate nodes with the same subject
        while (temp->next && temp->subject == temp->next->subject) {
            temp = temp->next;
            duplicate = true;
        }

        if (duplicate) { 
            if (head->subject == subject) {
                // **Retain all occurrences of the target subject**
                prev->next = head;  
                prev = temp;  // Move prev to the last node of this subject group
            } else {
                // **Remove all duplicate occurrences of non-target subjects**
                prev->next = temp->next;
            }
        } else {
            // **No duplicates, proceed normally**
            prev = head;
        }

        head = temp->next;  // Move to the next subject group
    }

    prev->next = nullptr;  // Ensure the list is correctly terminated
    return dummy.next;
}


class Words
{
    public:

    // Function to check if a character is a delimiter (e.g., punctuation or whitespace)
    bool isDelimiter(char c) {
        static const string delimiters = " \t\n,.\"\'()[]{}<>:;!?/\\-";
        return delimiters.find(c) != string::npos;
    }


    // Function to process the text data from a NewsNode list and populate a WordNode list with words
    void insertDataToWordNode(NewsNode* newsHead, WordNode*& wordHead) 
    {
        if (!newsHead) {
            cout << "No data found" << endl;
            return;
        }

        int lines = 0;
        while (newsHead) {
            string currentWord;
            
            for (char c : newsHead->text) {
                if (isDelimiter(c)) {
                    if (!currentWord.empty()) {
                        // Ensure the word is valid before inserting
                        WordNode* newNode = new WordNode{currentWord, 1, wordHead};
                        wordHead = newNode;
                        currentWord.clear();  // Clear the word buffer for the next word
                    }
                } else {
                    currentWord += tolower(c);  // Convert to lowercase for normalization
                }
            }

            // Handle the last word in the text
            if (!currentWord.empty()) {
                WordNode* newNode = new WordNode{currentWord, 1, wordHead};
                wordHead = newNode;
            }

            lines++;

            // Display progress every 500 lines
            if (lines % 500 == 0) {
                cout << "Processed " << lines << " lines" << endl;
            }

            newsHead = newsHead->next;
        }
    }


    // Function to find a child node with the given character 'c' in a Trie
    TrieNode* findChild(TrieNode* node, char c) {
        while (node) {
            if (node->c == c) return node;  // Return the node if it matches the character
            node = node->nextSibling;  // Move to the next sibling node
        }
        return nullptr;  // Return null if no matching child node is found
    }


    // Function to insert a word into a Trie data structure
    void insertTrie(TrieNode*& root, const string& word) {
        if (!root) root = new TrieNode('\0'); // Ensure the Trie has a root node
        TrieNode* node = root;

        for (char c : word) {
            TrieNode* child = findChild(node->firstChild, c);

            if (!child) {
                // Create a new node and insert it at the front (linked list head insertion)
                child = new TrieNode(c);
                child->nextSibling = node->firstChild;
                node->firstChild = child;
            }
            node = child; // Move to the next character node
        }
        node->count++; // Increment the word occurrence count
    }

    // Function to add words with counts to the linked list
    void addToList(WordNode*& head, WordNode*& tail, const string& word, int count) {
        WordNode* newNode = new WordNode{word, count, nullptr};
        if (!head) {  // If the linked list is empty, initialize it
            head = newNode;
            tail = newNode;
        } else {  // Otherwise, append the new node to the end of the linked list
            tail->next = newNode;
            tail = newNode;
        }
    }

    // Function to traverse the Trie and add words with their counts to a linked list
    void traverseTrie(TrieNode* node, string word, WordNode*& head, WordNode*& tail) {
        if (!node) return;  // Base case: return if the node is null

        string newWord = word + node->c;  // Append the current character to form a word

        if (node->count > 0)  
            addToList(head, tail, newWord, node->count);  // If it is a valid word, add it to the list

        traverseTrie(node->firstChild, newWord, head, tail);  // Traverse child nodes (next character)
        traverseTrie(node->nextSibling, word, head, tail);  // Traverse sibling nodes (same level)
    }

    // Function to calculate word frequency using a Trie and return a sorted linked list
    WordNode* calculateWordCount(WordNode*& wordHead) {
        if (!wordHead) {
            cout << "No data found" << endl;
            return nullptr;
        }

        TrieNode* root = new TrieNode('\0');  // Create a root TrieNode
        WordNode* current = wordHead;

        // Insert all words from the linked list into the Trie
        while (current) {
            insertTrie(root, current->text);
            current = current->next;
        }

        wordHead = nullptr;  // Clear the original list

        WordNode* newHead = nullptr;  // Head of the new sorted linked list
        WordNode* tail = nullptr;  // Tail pointer for appending nodes

        // Call `traverseTrie`, passing `newHead` and `tail`
        traverseTrie(root->firstChild, "", newHead, tail);

        return newHead;  // Return the new sorted linked list
    }
    // Function to merge two sorted linked lists based on word frequency (descending order)
    WordNode* merge(WordNode* left, WordNode* right) {
        
        // If both lists are empty, return nullptr and print message
        if (!left && !right) {
            cout << "Merging two empty lists!" << endl;
            return nullptr;
        }
        if (!left) return right; // If left list is empty, return right
        if (!right) return left; // If right list is empty, return left

        // Merge in descending order based on word count
        if (left->count > right->count) { 
            left->next = merge(left->next, right); // Recursive call for merging
            return left;
        } else {
            right->next = merge(left, right->next); // Recursive call for merging
            return right;
        }
    }


    // Function to split a linked list into two halves
    void splitList(WordNode* head, WordNode*& left, WordNode*& right) {
        // If the list is empty or has only one node, assign it to 'left' and set 'right' to null
        if (!head || !head->next) {
            left = head;
            right = nullptr;
            return;
        }

        // Use the slow and fast pointer technique to find the middle of the list
        WordNode* slow = head;
        WordNode* fast = head->next;

        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }

        // Split the list into two halves
        left = head;
        right = slow->next;
        slow->next = nullptr; // Break the list into two separate parts
    }


    // **Merge Sort Function for Sorting WordNode Linked List**
    WordNode* mergeSort(WordNode* head) 
    {
        // Base case: If the list is empty or has only one element, return as it is already sorted
        if (!head || !head->next) return head;

        WordNode* left;
        WordNode* right;
        
        // Split the list into two halves
        splitList(head, left, right);
        
        // Recursively sort both halves
        left = mergeSort(left);
        right = mergeSort(right);

        // Merge the sorted halves
        return merge(left, right);
    }

    
    // Function to output sorted words and their frequencies to a CSV file
    void outputSortWordsFile(WordNode* head,string filename) 
    {
        // Open the output file in binary mode
        ofstream outFile(filename, ios::binary);
        
        // Check if the file was created successfully
        if (!outFile) {
            cerr << "Failed to create sortedWords.csv" << endl;
            return;
        }

        // Write the UTF-8 BOM (Byte Order Mark) to ensure proper encoding
        outFile << "\xEF\xBB\xBF"; 

        // Write the CSV column headers
        outFile << "Word,Count\n";

        WordNode* current = head;  
        int count = 0;

        // Iterate through the linked list and write each word and its frequency to the file
        while (current) {
            outFile << current->text << "," << current->count << "\n";
            current = current->next;
            count++;
        }

        // Close the file
        outFile.close();

        // Print total words written to the console
        cout << "Total words written to CSV: " << count << endl;
        cout << "Data successfully written to " << filename << endl;
    }
};

class BinarySearch{
    private:
        News news;
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
                category_data = trim(finder->title);
            }
            else if (category == "text")
            {
                category_data = trim(finder->text);
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
        
        string trim(string str) {
            // Remove **ALL** leading spaces (including non-breaking spaces)
            str.erase(str.begin(), find_if(str.begin(), str.end(), [](unsigned char ch) {
                return !isspace(ch) && ch != '\xA0';  // Remove Unicode non-breaking spaces
            }));
        
            // Remove **ALL** trailing spaces
            str.erase(find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
                return !isspace(ch) && ch != '\xA0';
            }).base(), str.end());
        
            return str;
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
                            : category_data.compare(trim(target));
                
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
            for (NewsNode* temp = binarySearch(news.sortNewsNode(copyList(head), "subject"), "subject", category); temp; temp = temp->next) {
                stringstream ss(temp->date);
                string monthStr, dayStr, yearStr;
                ss >> monthStr >> dayStr >> yearStr;
                if (yearStr == "2016") countMap[getMonthNum(monthStr)]++;
            }
        }
        
    public:
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
                cout << "------------------------------------" << endl;
                cout << "Title: " << start->title << endl;
                cout << "Text: " << start->text << endl;
                cout << "Subject: " << start->subject << endl;
                cout << "Date: " << start->date << endl;
                cout << endl;
        
                start = start->next;
            }
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
    
};

size_t getMemoryUsage() {
    #ifdef _WIN32
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
            return pmc.WorkingSetSize;
        }
        return 0;
    #else
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        return usage.ru_maxrss * 1024;
    #endif
}

double measureExecutionTime(function<void()> func, double &memoryUsedMB) {
    size_t memBefore = getMemoryUsage();
    auto start = chrono::high_resolution_clock::now();
    
    func();
    
    auto end = chrono::high_resolution_clock::now();
    size_t memAfter = getMemoryUsage();
    
    memoryUsedMB = (memAfter - memBefore) / (1024.0 * 1024.0);
    return chrono::duration<double>(end - start).count();
}

void fixApostrophe(string &str) {
    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == '\0') {  // Detect null character (corruption)
            str[i] = '\xE2';   // Replace with first byte of ‘ (0xE2)
            str.insert(i + 1, "\x80\x99");  // Insert remaining bytes
        }
    }
}

void linkedlist_Menu() {
    NewsNode* fake = nullptr;
    NewsNode* real = nullptr;
    WordNode* wordHead = nullptr;
    BinarySearch bs;
    News news;
    Words wordsObj;
    string choice;

    double memoryUsedMB;
    double fakeMemoryUsedMB;
    double realMemoryUsedMB;
    double loadTime = measureExecutionTime([&]() {
        news.insertDataToNode(fake, "cleaned_fake.csv");
    },memoryUsedMB);
    double time = measureExecutionTime([&]() {
        news.insertDataToNode(real, "true.csv");
    },realMemoryUsedMB);
    cout << "Execution Time for Loading Fake Data: " << loadTime << " seconds\n";
    cout << "Execution Time for Loading Real Data: " << time << " seconds\n";
    cout << "Memory Usage for Loading Fake Data: " << memoryUsedMB << " MB\n";
    cout << "Memory Usage for Loading Real Data: " << realMemoryUsedMB << " MB\n";

    bool exit = false;
    while (!exit) {
        cout << "---------------------- Main Menu ----------------------" << endl;
        cout << "1. Display data" << endl;
        cout << "2. Sort data" << endl;
        cout << "3. Search data" << endl;
        cout << "4. Write data to CSV" << endl;
        cout << "5. Calculate total news node" << endl;
        cout << "6. Calculate fake news percentage" << endl;
        cout << "7. Filter Frequently Used Words in Fake News (Government Topic)" << endl;
        cout << "8. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == "1") {
            cout << "-------------- Displaying Menu --------------" << endl;
            cout << "1. Display fake news data" << endl;
            cout << "2. Display true news data" << endl;
            cout << "3. Display both" << endl;
            cout << "4. Return to Main Menu" << endl;
            cout << "Enter your choice: ";
            cin >> choice;

            if (choice == "1") {
                double displayTime = measureExecutionTime([&]() {
                    printNewsNodes(fake);
                },memoryUsedMB);
                cout << "Execution Time for Displaying Fake News Data: " << displayTime << " seconds\n";
                cout << "Memory Usage for Displaying Fake News Data: " << memoryUsedMB << " MB\n";
            } else if (choice == "2") {
                double displayTime = measureExecutionTime([&]() {
                    printNewsNodes(real);
                },memoryUsedMB);
                cout << "Execution Time for Displaying True News Data: " << displayTime << " seconds\n";
                cout << "Memory Usage for Displaying True News Data: " << memoryUsedMB << " MB\n";
            } else if (choice == "3") {
                double displayTime = measureExecutionTime([&]() {
                    printNewsNodes(fake);
                    printNewsNodes(real);
                },memoryUsedMB);
                cout << "Execution Time for Displaying Both Fake and True News Data: " << displayTime << " seconds\n";
                cout << "Memory Usage for Displaying Both Fake and True News Data: " << memoryUsedMB << " MB\n";
            } else if (choice == "4") {
                continue;
            } else {
                cout << "Invalid choice. Please try again." << endl;
            }
            continue;
        }
        else if (choice == "2") {
            string sort_choice, write;
            cout << "Enter the category you want to sort (title, text, subject, date): ";
            cin >> sort_choice;
            if (sort_choice != "title" && sort_choice != "text" && sort_choice != "subject" && sort_choice != "date") {
                cout << "Invalid category. Please try again." << endl;
                continue;
            }

            NewsNode* fakeCopy = bs.copyList(fake);
            NewsNode* realCopy = bs.copyList(real);
            cout << "Display the data in: \n\t1. Terminal\n\t2. CSV File \nChoose (1/2): ";
            cin >> write;

            if (write != "1" && write != "2") {
                cout << "Invalid choice. Please try again." << endl;
                choice = "-1";
                continue;
            }
            else{
                double fakeSortTime = measureExecutionTime([&]() {
                    fakeCopy = news.sortNewsNode(fakeCopy, sort_choice);
                },fakeMemoryUsedMB);
    
                double realSortTime = measureExecutionTime([&]() {
                    realCopy = news.sortNewsNode(realCopy, sort_choice);
                },realMemoryUsedMB);
    
                if (write == "1") {  // Display in terminal
                    printNewsNodes(fakeCopy);
                    printNewsNodes(realCopy);
                } else {  // Write to CSV
                    news.writeToCSV(fakeCopy, "sorted_fake_news");
                    news.writeToCSV(realCopy, "sorted_true_news");
                }
    
                // Show sorting execution time AFTER displaying/writing data
                cout << "Sorting completed.\n";
                cout << "Execution Time for Sorting Fake News Data: " << fakeSortTime << " seconds\n";
                cout << "Execution Time for Sorting True News Data: " << realSortTime << " seconds\n";
                cout << "Memory Usage for Sorting Fake News Data: " << fakeMemoryUsedMB << " MB\n";
                cout << "Memory Usage for Sorting True News Data: " << realMemoryUsedMB << " MB\n";
            }
            
        }
        else if (choice == "3") {
            string dataset, search_choice, target; 

            cout << "Enter the dataset you want to search (fake, true): ";
            cin >> dataset;
            if (dataset != "fake" && dataset != "true") {
                cout << "Invalid dataset. Please try again." << endl;
                continue;
            }
            else{
                cout << "Enter the category you want to search (title, text, subject, date): ";
                cin >> search_choice;
                if (search_choice != "title" && search_choice != "text" && search_choice != "subject" && search_choice != "date") {
                    cout << "Invalid category. Please try again." << endl;
                    continue;
                }
                else if (search_choice == "date") {
                    cout << "*Reminder: Date can be search by year only" << endl;
                }
                cout << "Enter the target value: ";
                cin.ignore();
                getline(cin, target);
                fixApostrophe(target);

                NewsNode* fakeCopy = bs.copyList(fake);
                NewsNode* realCopy = bs.copyList(real);
                fakeCopy = news.sortNewsNode(fakeCopy, search_choice);
                realCopy = news.sortNewsNode(realCopy, search_choice);
                double searchTime = measureExecutionTime([&]() {
                    if (dataset == "fake") {
                        bs.PrintSearchResult(fakeCopy, search_choice, target);
                    } else {
                        bs.PrintSearchResult(realCopy, search_choice, target);
                    }
                },memoryUsedMB);
                cout << "Execution Time for Searching: " << searchTime << " seconds\n";
                cout << "Memory Usage for Searching: " << memoryUsedMB << " MB\n";
                }
        }
        else if (choice == "4") {
            string filename;
            cout << "Enter the filename you want to write to: ";
            cin >> filename;

            double fakeWriteTime = measureExecutionTime([&]() {
                news.writeToCSV(fake, filename);
            },fakeMemoryUsedMB);

            double realWriteTime = measureExecutionTime([&]() {
                news.writeToCSV(real, filename);
            },realMemoryUsedMB);

            cout << "Execution Time for Writing Fake News Data to CSV: " << fakeWriteTime << " seconds\n";
            cout << "Execution Time for Writing True News Data to CSV: " << realWriteTime << " seconds\n";
            cout << "Memory Usage for Writing Fake News Data to CSV: " << fakeMemoryUsedMB << " MB\n";
            cout << "Memory Usage for Writing Fake News Data to CSV: " << realMemoryUsedMB << " MB\n";
        }
        else if (choice == "5") {
            double totalTime = measureExecutionTime([&]() {
                cout << "Total Fake News Nodes: " << news.calculateTotalNewsNode(fake) << endl;
                cout << "Total True News Nodes: " << news.calculateTotalNewsNode(real) << endl;
            },memoryUsedMB);
            cout << "Execution Time for Calculating Total News Nodes: " << totalTime << " seconds\n";
            cout << "Memory Usage for Calculating Total News Nodes: " << memoryUsedMB << " MB\n";
        }
        else if (choice == "6") {
            double percentageTime = measureExecutionTime([&]() {
                bs.calculateFakePercentage(fake, real);
            },memoryUsedMB);
            cout << "Execution Time for Calculating Fake News Percentage: " << percentageTime << " seconds\n";
            cout << "Memory Usage for Calculating Fake News Percentage: " << memoryUsedMB << " MB\n";
        }
        else if (choice == "7"){
            NewsNode* fakeNewsHead = bs.copyList(fake);
            fakeNewsHead = news.sortNewsNode(fakeNewsHead, "subject");

            string condition = "Government News";
            cout << "\nNow checking " <<condition<< " word count\n\n ";
            double execution_time = measureExecutionTime([&]() {
                fakeNewsHead = deleteDuplicates(fakeNewsHead, condition);
                wordsObj.insertDataToWordNode(fakeNewsHead, wordHead);
                wordHead = wordsObj.calculateWordCount(wordHead);
            
                cout << "Before Sorting Words...\n";
                wordHead = wordsObj.mergeSort(wordHead);
            },memoryUsedMB);
            wordsObj.outputSortWordsFile(wordHead, "sortedWords.csv");
            cout << "Execution Time for Filtering Frequently Used Words: " << execution_time << " seconds\n";
            cout << "Memory Usage for Filtering Frequently Used Words: " << memoryUsedMB << " MB\n";
        }
        else if (choice == "8") {
            exit = true;
        }
        else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}


int main() {
    linkedlist_Menu();

    return 0;
}