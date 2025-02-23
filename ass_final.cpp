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


string convertDateToSortableFormat(const string& inputDate) {
    istringstream dateStream(inputDate);
    string monthName, day, year;
    
    // Extract month name, day, and year from the input string
    dateStream >> monthName >> day >> year;

    // Convert month name to its numeric representation
    string month = getMonthNumber(monthName);

    // Handle invalid date format
    if (month.empty() || day.empty() || year.empty()) {
        return "9999/99/99"; // Assign a high value to prevent sorting issues
    }

    // Remove the comma from the day string
    day.pop_back();

    // Ensure the day is in two-digit format (e.g., "5" -> "05")
    if (day.length() == 1) {
        day = "0" + day;
    }

    // Return the formatted date in "YYYY/MM/DD" format
    return year + "/" + month + "/" + day;
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

// Function to format a field to ensure proper CSV formatting
string formatCSVField(const string& field) 
{
    string formattedField = field;
    if (!formattedField.empty() && formattedField.front() == ' ') {
        formattedField.erase(0, formattedField.find_first_not_of(' '));
    }

    // 替换换行符 `\n` 和 `\r` 为 ` `
    replace(formattedField.begin(), formattedField.end(), '\n', ' ');
    replace(formattedField.begin(), formattedField.end(), '\r', ' ');

    // 如果字段内有 `,` 或 `"`，需要加引号，并且内部 `"` 需要转义成 `""`
    if (formattedField.find(',') != string::npos || formattedField.find('"') != string::npos) {
        ostringstream temp;
        temp << '"';
        for (char c : formattedField) {
            if (c == '"') temp << "\"\"";  // CSV 规则，`"` 需要双写
            else temp << c;
        }
        temp << '"';
        return temp.str();
    }

    return formattedField;
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
    
        // Skip leading spaces in both left and right strings
        while (*Left == ' ') Left++;
        while (*Right == ' ') Right++;
    
        // Ensure strings starting with a double quote always come first
        bool leftIsQuote = (*Left == '"');
        bool rightIsQuote = (*Right == '"');
    
        if (leftIsQuote && !rightIsQuote) return true;  // Left starts with a quote, right does not → Left comes first
        if (rightIsQuote && !leftIsQuote) return false; // Right starts with a quote, left does not → Right comes first
    
        // Standard comparison logic
        while (*Left && *Right) {
            if (*Left != *Right) {
                return *Left < *Right; // Compare characters in lexicographical order
            }
            Left++;
            Right++;
    
            // Continue skipping spaces
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
        // Close the file after writing

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



int main() {
    NewsNode* newsHead = nullptr;
    WordNode* wordHead = nullptr;
    NewsNode* trueNewsHead = nullptr;
    News newsObj;
    Words wordsObj;

    cout << "\n================= Program Start =================\n";

    clock_t start = clock();
    cout << "\nNow Running...\n";
    newsObj.insertDataToNode(newsHead, "cleaned_fake.csv");
    newsObj.insertDataToNode(trueNewsHead, "true.csv");

    cout << "\nTotal Fake News Nodes: " << newsObj.calculateTotalNewsNode(newsHead) << endl;
    cout << "\nTotal True News Nodes: " << newsObj.calculateTotalNewsNode(trueNewsHead) << endl;

    clock_t end = clock();
    double duration = double(end - start) / CLOCKS_PER_SEC;
    cout << "Execution Time: " << duration << " seconds\n";

    cout << "\n=================================================\n";

    start = clock();
    cout << "\nNow Running First Question\n";
    cout << "\nTotal News Nodes: " << newsObj.calculateTotalNewsNode(newsHead) << endl;
    newsHead = newsObj.sortNewsNode(newsHead, "year");
    trueNewsHead = newsObj.sortNewsNode(trueNewsHead, "year");
    //printNewsNodes(newsHead);
    newsObj.writeToCSV(newsHead, "sortedNews.csv");
    newsObj.writeToCSV(trueNewsHead, "sortedTrueNews.csv");
    // printNewsNodes(newsHead);
    cout << "\nTotal News Nodes: " << newsObj.calculateTotalNewsNode(newsHead) << endl;
    cout << "\nTotal True News Nodes: " << newsObj.calculateTotalNewsNode(trueNewsHead) << endl;

    end = clock();
    duration = double(end - start) / CLOCKS_PER_SEC;
    cout << "Execution Time: " << duration << " seconds\n";

    cout << "\n=================================================\n";

    // 第二题在这里进行
    newsHead = newsObj.sortNewsNode(newsHead, "subject");

    start = clock();
    string condition = "Government News";
    cout << "\nNow Running Third Question\n\n";
    cout << "\nNow is check " <<condition<< " word count\n\n ";
    newsHead = deleteDuplicates(newsHead, condition);

    wordsObj.insertDataToWordNode(newsHead, wordHead);
    wordHead = wordsObj.calculateWordCount(wordHead);

    cout << "Before Sorting Words...\n";
    wordHead = wordsObj.mergeSort(wordHead);
    wordsObj.outputSortWordsFile(wordHead, "sortedWords.csv");

    end = clock();
    duration = double(end - start) / CLOCKS_PER_SEC;
    cout << "\nTotal News Nodes: " << newsObj.calculateTotalNewsNode(newsHead) << endl;
    cout << "Execution Time: " << duration << " seconds\n";

    cout << "\n================= Program End =================\n";

    return 0;
}