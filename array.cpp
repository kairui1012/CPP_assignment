#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cctype>
#include <limits>
#include <math.h>
#include <iomanip>
#include <ctime>
#include <functional>
#include <chrono>
#ifdef _WIN32
    #include <windows.h>
    #include <psapi.h>
#else 
    #include <sys/resource.h>
    #include <unistd.h>
#endif

using namespace std;
using namespace chrono;

const int MAX_ROWS = 24000;
const int MAX_COLS = 4; // title, text, subject, date

const int MAX_WORDS = 10000;
const int MAX_WORD_LEN = 50;

const char *stopWords[] = {
    // articles & prepositions
    "the", "to", "of", "and", "a", "in", "on", "with", "as", "by", "at", "from", "for", "an",
    "about", "after", "over", "into", "up", "no", "now", "other",

    // pronouns
    "he", "she", "it", "we", "they", "i", "you", "his", "her", "their", "our", "its", "my", "your", "them", "us",

    // common verbs
    "is", "are", "was", "were", "be", "been", "has", "have", "had", "do", "does", "did", "can", "could",
    "would", "should", "shall", "may", "might", "must", "will", "says", "say", "said", "make", "made", "go", "went",

    // conjuntions & others
    "that", "this", "there", "which", "who", "whom", "what", "when", "where", "why", "how", "but", "or", "nor",
    "so", "if", "then", "than", "because", "also", "very", "too", "just", "some", "any", "more", "most",

    // miscounted words
    "s", "t", "u", "not", "all", "out",

    // common news vocabulary
    "said", "says", "say", "like", "new", "year", "time", "will", "one", "two", "many"};

const int STOP_WORDS_COUNT = sizeof(stopWords) / sizeof(stopWords[0]);

// check if a word is stop word
bool isStopWord(const char *word)
{
    for (int i = 0; i < STOP_WORDS_COUNT; i++)
    {
        if (strcmp(word, stopWords[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

// binary search for words (findMostCommonWords)
int binarySearch(char words[][MAX_WORD_LEN], int size, const char *target)
{
    int left = 0, right = size - 1;
    while (left <= right)
    {
        int mid = (left + right) / 2;
        int cmp = strcmp(words[mid], target);
        if (cmp == 0)
            return mid;
        else if (cmp < 0)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return -1;
}

// insert new word into sorted array (insertion sort)
void insertWordSorted(char words[][MAX_WORD_LEN], int counts[], int &size, const char *newWord)
{
    if (size >= MAX_WORDS)
        return; // exceeding capacity
    int i;
    for (i = size - 1; i >= 0 && strcmp(words[i], newWord) > 0; i--)
    {
        strcpy(words[i + 1], words[i]); // shift right
        counts[i + 1] = counts[i];
    }
    strcpy(words[i + 1], newWord);
    counts[i + 1] = 1;
    size++;
}

// find top 20 most common words in government fake news
void findMostCommonWords(char ***data, int rowCount)
{
    char words[MAX_WORDS][MAX_WORD_LEN] = {0}; // store words
    int counts[MAX_WORDS] = {0};               // store word frequencies
    int wordCount = 0;

    for (int i = 0; i < rowCount; i++)
    {

        // filter "Government News"
        if (strcmp(data[i][2], "Government News") != 0)
        {
            continue;
        }

        char *textCopy = strdup(data[i][1]); // copy 'text' data
        char *word = strtok(textCopy, " ,.?!:\"();");

        while (word)
        {
            for (int j = 0; word[j]; j++)
                word[j] = tolower(word[j]);
            if (isStopWord(word))
            { // skip stop words
                word = strtok(NULL, " ,.?!:\"();");
                continue;
            }
            int index = binarySearch(words, wordCount, word);
            if (index != -1)
            {
                counts[index]++; // If the word already exists, increase the count
            }
            else
            {
                insertWordSorted(words, counts, wordCount, word);
            }
            word = strtok(NULL, " ,.?!:\"();");
        }
    }

    // sort by frequency and output the top 20 most common words
    for (int i = 0; i < wordCount - 1; i++)
    {
        int maxIdx = i;
        for (int j = i + 1; j < wordCount; j++)
        {
            if (counts[j] > counts[maxIdx])
                maxIdx = j;
        }
        swap(counts[i], counts[maxIdx]);
        char temp[MAX_WORD_LEN];
        strcpy(temp, words[i]);
        strcpy(words[i], words[maxIdx]);
        strcpy(words[maxIdx], temp);
    }

    cout << "-------------------------------------------------------------" << endl;
    cout << "Top 20 Most Common Words in Government Fake News:" << endl;
    for (int i = 0; i < 20 && i < wordCount; i++)
    {
        cout << words[i] << ": " << counts[i] << endl;
    }
}

// convert month into number
int getMonthNumber(const string &month)
{
    const char *months[] = {"January", "February", "March", "April", "May", "June",
                            "July", "August", "September", "October", "November", "December"};
    for (int i = 0; i < 12; i++)
    {
        if (month == months[i])
            return i + 1;
    }
    return 0; // invalid month
}

// extract the date and convert to YYYYMMDD
int extractDate(const char *date)
{
    // 99999999 is bigger than any valid YYYYMMDD, so it is placed last when sorting
    if (date == nullptr)
        return 99999999;

    stringstream ss(date);
    string month, day, year;
    ss >> month >> day >> year;

    if (day.back() == ',')
        day.pop_back(); // remove comma

    int dayNum = atoi(day.c_str());
    int monthNum = getMonthNumber(month);
    int yearNum = atoi(year.c_str());

    if (monthNum == 0)
        return 99999999; // Handle invalid month

    return yearNum * 10000 + monthNum * 100 + dayNum;
}

// merge 2 sub-arrays (sorted by date)
void merge(char ***data, int left, int mid, int right)
{
    int leftSize = mid - left + 1;
    int rightSize = right - mid;

    char ***leftArr = new char **[leftSize];
    char ***rightArr = new char **[rightSize];

    for (int i = 0; i < leftSize; i++)
        leftArr[i] = data[left + i];
    for (int i = 0; i < rightSize; i++)
        rightArr[i] = data[mid + 1 + i];

    int i = 0, j = 0, k = left;

    while (i < leftSize && j < rightSize)
    {
        int date1 = extractDate(leftArr[i][3]);
        int date2 = extractDate(rightArr[j][3]);

        if (date1 <= date2)
            data[k++] = leftArr[i++];
        else
            data[k++] = rightArr[j++];
    }

    while (i < leftSize)
        data[k++] = leftArr[i++];
    while (j < rightSize)
        data[k++] = rightArr[j++];

    delete[] leftArr;
    delete[] rightArr;
}

// sort the data using merge sort
void mergeSort(char ***data, int left, int right)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;
        mergeSort(data, left, mid);
        mergeSort(data, mid + 1, right);
        merge(data, left, mid, right);
    }
}

// parse CSV line
void parseCSVLine(const string &line, char **rowData)
{
    string cell;
    bool insideQuotes = false;
    int col = 0;

    for (size_t i = 0; i < line.length(); i++)
    {
        char c = line[i];

        if (c == '"' && (i == 0 || line[i - 1] != '\\'))
        {
            insideQuotes = !insideQuotes;
        }
        else if (c == ',' && !insideQuotes)
        {
            rowData[col] = new char[cell.size() + 1];
            strcpy(rowData[col], cell.c_str());
            col++;
            cell.clear();
        }
        else
        {
            cell += c;
        }
    }

    if (col < MAX_COLS)
    {
        rowData[col] = new char[cell.size() + 1];
        strcpy(rowData[col], cell.c_str());
    }
}

// read the data from CSV
int readCSV(const string &filename, char ***data)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error: Could not open file " << filename << endl;
        return 0;
    }

    string line;
    int row = 0;
    bool isHeader = true;

    while (getline(file, line) && row < MAX_ROWS)
    {
        if (isHeader)
        {
            isHeader = false;
            continue;
        }

        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        parseCSVLine(line, data[row]);
        row++;
    }

    file.close();
    return row;
}

// display data
void displayData(char ***data, int rowCount)
{
    for (int i = 0; i < rowCount; i++)
    {
        for (int j = 0; j < MAX_COLS; j++)
        {
            cout << "\"" << (data[i][j] ? data[i][j] : "") << "\" | ";
        }
        cout << endl;
    }
}

// free up dynamically allocated memory
// prevent program crash and memory leak
void freeMemory(char ***data, int rowCount)
{
    if (!data)
        return; // avoid releasing a null pointer

    for (int i = 0; i < rowCount; i++)
    {
        if (data[i])
        { // Check if it is empty
            for (int j = 0; j < MAX_COLS; j++)
            {
                if (data[i][j])
                    delete[] data[i][j];
            }
            delete[] data[i];
        }
    }
    delete[] data;
}

// write the sorted data to a CSV file
void writeSortedData(const string &filename, char ***data, int rowCount, const string headers[MAX_COLS])
{
    ofstream outFile(filename);
    if (!outFile.is_open())
    {
        cerr << "Error: Could not open file " << filename << " for writing." << endl;
        return;
    }

    // Write the header
    for (int j = 0; j < MAX_COLS; j++)
    {
        outFile << "\"" << headers[j] << "\"";
        if (j < MAX_COLS - 1)
            outFile << ",";
    }
    outFile << endl;

    // Write the sorted data
    for (int i = 0; i < rowCount; i++)
    {
        for (int j = 0; j < MAX_COLS; j++)
        {
            if (data[i][j])
            {
                outFile << "\"" << data[i][j] << "\"";
            }
            else
            {
                // Handle empty values
                outFile << "\"\"";
            }

            // Separate columns with commas
            if (j < MAX_COLS - 1)
            {
                outFile << ",";
            }
        }
        outFile << endl;
    }

    outFile.close();
    cout << "Sorted output saved to " << filename << endl;
}

// calculate and display the number of articles
void displayArticleCount(int trueCount, int fakeCount)
{
    cout << endl
         << "-------- Total Articles Count --------" << endl;
    cout << "True news articles: " << trueCount << endl;
    cout << "Fake news articles: " << fakeCount << endl;
    cout << "Total articles: " << (trueCount + fakeCount) << endl;
}

// get keyword
string getSearchKeyword() {
    string keyword;
    cout << "Enter keyword/date (e.g., December 27, 2016) to search: ";
    cin.ignore();
    getline(cin, keyword); // allow multiple input
    return keyword;
}

// search and display the result
void searchAndDisplay(char ***data, int rowCount, const string &keyword) {
    bool isDateSearch = false;
    int searchDate = extractDate(keyword.c_str());

    if (searchDate != 99999999) { 
        isDateSearch = true;
    }

    bool found = false;

    if (isDateSearch) {
        // binary search (date)
        int left = 0, right = rowCount - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            int midDate = extractDate(data[mid][3]);

            if (midDate == searchDate) {
                found = true;

                int start = mid, end = mid;
                while (start > 0 && extractDate(data[start - 1][3]) == searchDate) start--;
                while (end < rowCount - 1 && extractDate(data[end + 1][3]) == searchDate) end++;

                cout << "Article found with date " << keyword << ":" << endl;
                for (int i = start; i <= end; i++) {
                    cout << "Title: " << (data[i][0] ? data[i][0] : "N/A") << endl;
                    cout << "Text: " << (data[i][1] ? data[i][1] : "N/A") << endl;
                    cout << "Subject: " << (data[i][2] ? data[i][2] : "N/A") << endl;
                    cout << "Date: " << (data[i][3] ? data[i][3] : "N/A") << endl;
                    cout << "------------------------------------" << endl;
                }
                break;
            } 
            else if (midDate < searchDate) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
    } 
    else {
        // linear search (title, text, subject)
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < 3; j++) {
                if (data[i][j] && strstr(data[i][j], keyword.c_str())) {
                    cout << "Matching article found:\n";
                    cout << "Title: " << (data[i][0] ? data[i][0] : "N/A") << endl;
                    cout << "Text: " << (data[i][1] ? data[i][1] : "N/A") << endl;
                    cout << "Subject: " << (data[i][2] ? data[i][2] : "N/A") << endl;
                    cout << "Date: " << (data[i][3] ? data[i][3] : "N/A") << endl;
                    cout << "------------------------------------" << endl;
                    found = true;
                    break;
                }
            }
        }
    }

    if (!found) {
        cout << "No matching article found." << endl;
    }
}

// Calculate the number of rows by subject and year
int *countByYearAndSubject(char ***Data, int rowCount, const string &subject, int year)
{
    int *monthCounts = new int[12]();

    for (int i = 0; i < rowCount; i++)
    {
        if (Data[i][2] && Data[i][3]) // Subject and Date Not null
        {
            if (strcmp(Data[i][2], subject.c_str()) == 0)
            {
                int dateNum = extractDate(Data[i][3]);
                int yearExtracted = dateNum / 10000;        // get the year
                int monthExtracted = (dateNum / 100) % 100; // get the month

                if (yearExtracted == year && monthExtracted >= 1 && monthExtracted <= 12)
                {
                    monthCounts[monthExtracted - 1]++; // add 1 to specific month
                }
            }
        }
    }
    return monthCounts;
}

// Display the percentage according to month
void displayPercentage(int *fakeCounts, int *trueCounts)
{
    const char *months[] = {"January", "February", "March", "April", "May", "June",
                            "July", "August", "September", "October", "November", "December"};

    for (int i = 0; i < 12; i++)
    {
        int percent = round((float)fakeCounts[i] / (fakeCounts[i] + trueCounts[i]) * 100);

        cout << left << setw(12) << months[i] << " | ";
        for (int j = 0; j < percent; j++)
        {
            cout << "*";
        }
        cout << " " << right << setw(3) << percent << "%" << endl;
        cout << endl;
    }
}

size_t getMemoryUsage() {
    #ifdef _WIN32 // For Windows user
        PROCESS_MEMORY_COUNTERS memInfo;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo))) {
            return memInfo.WorkingSetSize;
        }
        return 0;
    #else
        struct rusage usage; // For Mac/Linux user
        if (getrusage(RUSAGE_SELF, &usage) == 0) {
            return usage.ru_maxrss * 1024;
        }
        return 0;
    #endif
    }

// Calculate the execution time and memory used
void executeWithTimerAndMemory(function<void()> task) {
    auto startTime = high_resolution_clock::now();

    size_t beforeMemory = getMemoryUsage();
    task();
    size_t afterMemory = getMemoryUsage();

    auto endTime = high_resolution_clock::now();
    duration<double> elapsed = endTime - startTime;

    
    size_t memoryUsed = (beforeMemory > 0 && afterMemory > beforeMemory) ? (afterMemory - beforeMemory) : 0;
    double memoryUsedMB = memoryUsed / (1024.0 * 1024.0);

    cout << fixed << setprecision(5)
         << "Execution time: " << elapsed.count() << " seconds, "
         << "Memory used: " << memoryUsedMB << " MB" << endl;
}

char getUserConfirmation(const string &message) {
    char choice;
    do {
        cout << message;
        cin >> choice;
        choice = toupper(choice);

        if (choice != 'Y' && choice != 'N') {
            cout << "Invalid input! Please enter 'Y' or 'N'." << endl;
        }
    } while (choice != 'Y' && choice != 'N');

    return choice;
}


int main()
{
    // initialize dynamic array (true news)
    char ***trueData = new char **[MAX_ROWS];
    for (int i = 0; i < MAX_ROWS; i++)
    {
        trueData[i] = new char *[MAX_COLS];
        for (int j = 0; j < MAX_COLS; j++)
            trueData[i][j] = nullptr;
    }

    // initialize dynamic array (fake news)
    char ***fakeData = new char **[MAX_ROWS];
    for (int i = 0; i < MAX_ROWS; i++)
    {
        fakeData[i] = new char *[MAX_COLS];
        for (int j = 0; j < MAX_COLS; j++)
            fakeData[i][j] = nullptr;
    }

    int trueRowCount = readCSV("true.csv", trueData);
    int fakeRowCount = readCSV("cleaned_fake.csv", fakeData);
    int choice;
    int selectedFile;
    bool isRunning = true;
    string keyword;
    string headers[MAX_COLS] = {"title", "text", "subject", "date"};

    while (isRunning)
    {

        cout << endl;
        cout << "------------------------------ Main Menu ------------------------------" << endl;
        cout << "1. Sort articles by date" << endl;
        cout << "2. Display articles" << endl;
        cout << "3. Calculate total news" << endl;
        cout << "4. Search articles" << endl;
        cout << "5. Calculate fake political news percentage (2016)" << endl;
        cout << "6. Most frequently used words in fake government news" << endl;
        cout << "7. Exit" << endl;

        cin >> choice;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid option! Please enter a number." << endl;
            continue;
        }

        if (choice == 7)
        {
            cout << "Exited";
            break;
        }

        switch (choice)
        {
        case 1:
            cout << endl;
            cout << "Select the file to sort: " << endl;
            cout << "0. Back to menu" << endl;
            cout << "1. True.csv" << endl;
            cout << "2. Fake.csv" << endl;

            cin >> selectedFile;

            if (selectedFile == 1)
            {
                executeWithTimerAndMemory([&](){ 
                    mergeSort(trueData, 0, trueRowCount - 1);
                    writeSortedData("sorted_true.csv", trueData, trueRowCount, headers);
                });

                if (getUserConfirmation("Display the sorted data? (Y/N): ") == 'Y') {
                    executeWithTimerAndMemory([&](){
                        displayData(trueData, trueRowCount);
                    });
                }
            }

            else if (selectedFile == 2)
            {
                executeWithTimerAndMemory([&](){ 
                    mergeSort(fakeData, 0, fakeRowCount - 1);
                    writeSortedData("sorted_fake.csv", fakeData, fakeRowCount, headers);
                });

                if (getUserConfirmation("Display the sorted data? (Y/N): ") == 'Y') {
                    executeWithTimerAndMemory([&](){
                        displayData(trueData, trueRowCount);
                    });
                }
            }
            else if (selectedFile == 0)
            {
                continue;
            }
            else
            {
                cout << "Invalid option! Please try again." << endl;
            }

            break;

        case 2:
            cout << endl;
            cout << "Select the file to display: " << endl;
            cout << "0. Back to menu" << endl;
            cout << "1. True.csv" << endl;
            cout << "2. Fake.csv" << endl;

            cin >> selectedFile;

            if (selectedFile == 1)
            {
                executeWithTimerAndMemory([&](){ 
                    displayData(trueData, trueRowCount);
                });
            }
            else if (selectedFile == 2)
            {
                executeWithTimerAndMemory([&](){ 
                    displayData(fakeData, fakeRowCount);
                });
            }
            else if (selectedFile == 0)
            {
                continue;
            }
            else
            {
                cout << "Invalid option! Please try again." << endl;
            }

            break;

        case 3:
            executeWithTimerAndMemory([&]() { 
                displayArticleCount(trueRowCount, fakeRowCount); 
            });
            break;

        case 4:
            cout << "Select the file to search: " << endl;
            cout << "0. Back to menu" << endl;
            cout << "1. True.csv" << endl;
            cout << "2. Fake.csv" << endl;
            cin >> selectedFile;

            if (selectedFile == 1)
            {
                string keyword = getSearchKeyword();
                executeWithTimerAndMemory([&]() { 
                    mergeSort(trueData, 0, trueRowCount - 1);
                    searchAndDisplay(trueData, trueRowCount, keyword);
                });
            }
            else if (selectedFile == 2)
            {
                string keyword = getSearchKeyword();
                executeWithTimerAndMemory([&]() { 
                    mergeSort(fakeData, 0, fakeRowCount - 1);
                    searchAndDisplay(fakeData, fakeRowCount, keyword);
                });
            }
            else if (selectedFile == 0)
            {
                continue;
            }
            else
            {
                cout << "Invalid option! Please try again." << endl;
            }

            break;

        case 5:
        {
            cout << "Percentage of Fake Political News Articles in 2016" << endl;
            cout << endl;
            executeWithTimerAndMemory([&]() { 
                int *fakeCounts = countByYearAndSubject(fakeData, fakeRowCount, "politics", 2016);
                int *trueCounts = countByYearAndSubject(trueData, trueRowCount, "politicsNews", 2016);
                displayPercentage(fakeCounts, trueCounts);

                delete[] fakeCounts;
                delete[] trueCounts;

                cout << endl;
                cout << "Each '*' represents 1% of fake political news articles." << endl;
            });
            
            break;
        }

        case 6:
            executeWithTimerAndMemory([&]() {
                // restore data from "cleaned_fake.csv"
                // prevents potential side effects from mergeSort()
                fakeRowCount = readCSV("cleaned_fake.csv", fakeData);
                findMostCommonWords(fakeData, fakeRowCount);
            });
            break;

        default:
            cout << "Invalid option!" << endl;
            break;
        }
    }

    freeMemory(trueData, trueRowCount);
    freeMemory(fakeData, fakeRowCount);

    return 0;
}
