#include<iostream>
#include<string>
using namespace std;

struct Song {
    int no;
    string artist;
    string title;
    Song *next;
};

class MusicPlayer {
private:
    Song *head;

public:
    MusicPlayer() : head(NULL) {}

    void addSong(int no, string artist, string title) {
        Song *newSong = new Song;
        newSong->no = no;
        newSong->artist = artist;
        newSong->title = title;

        if(head == NULL) {
            head = newSong;
            newSong->next = head; //circular
        } else {
            Song *temp = head;
            while(temp->next != head) { //circular
                temp = temp->next;
            }
            temp->next = newSong;
            newSong->next = head; //circular
        }
    }

    void displaySongList() {
        Song *temp = head;
        do { //circular
            cout << "No: " << temp->no << ", Artist: " << temp->artist << ", Song: " << temp->title << endl;
            temp = temp->next;
        } while(temp != head); //circular
    }
};

int main() {
    MusicPlayer player;

    player.addSong(1, "Taylor Swift", "Shake It Off");
    player.addSong(2, "Adele", "Hello");
    player.addSong(3, "Doja Cats", "Woman");

    player.displaySongList();
} 
