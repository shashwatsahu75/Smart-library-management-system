#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;

static inline string intToString(int v) {
    ostringstream oss;
    oss << v;
    return oss.str();
}

static inline int stringToInt(const string &s) {
#if __cplusplus >= 201103L
    try { return stoi(s); } catch(...) { return atoi(s.c_str()); }
#else
    return atoi(s.c_str());
#endif
}

const int RENT_PER_DAY = 10;
const int FINE_PER_DAY = 15;

class Book {
private:
    int id;
    string title;
    string author;
    int price;
    int qty;
    int issued;
public:
    Book() { id=0; price=0; qty=0; issued=0; }
    Book(int i, string t, string a, int p, int q, int is=0) { id=i; title=t; author=a; price=p; qty=q; issued=is; }
    int getId() { return id; }
    string getTitle() { return title; }
    string getAuthor() { return author; }
    int getPrice() { return price; }
    int getAvailable() { return qty - issued; }
    bool isAvailable() { return (qty - issued) > 0; }
    void addIssue() { issued++; }
    void returnIssue() { if (issued>0) issued--; }
    void display() { cout << left << setw(8) << id << setw(25) << title << setw(20) << author << setw(10) << price << setw(10) << getAvailable() << "\n"; }
    string toFileString() { return intToString(id) + "|" + title + "|" + author + "|" + intToString(price) + "|" + intToString(qty) + "|" + intToString(issued); }
    void fromFileString(const string &line) {
        int pos1 = line.find('|'), pos2, start=0;
        id = stringToInt(line.substr(0,pos1));
        start = pos1+1;
        pos2 = line.find('|',start);
        title = line.substr(start,pos2-start);
        start = pos2+1;
        pos2 = line.find('|',start);
        author = line.substr(start,pos2-start);
        start = pos2+1;
        pos2 = line.find('|',start);
        price = stringToInt(line.substr(start,pos2-start));
        start = pos2+1;
        pos2 = line.find('|',start);
        qty = stringToInt(line.substr(start,pos2-start));
        start = pos2+1;
        issued = stringToInt(line.substr(start));
    }
};

class Person {
protected:
    string name;
public:
    Person() {}
    Person(string n) { name = n; }
};

class Admin : public Person {
private:
    string username;
    string password;
public:
    Admin() : Person("admin") { username="admin"; password="1234"; }
    bool authenticate(const string &u,const string &p) { return u==username && p==password; }
    string getUsername() { return username; }
    string getPassword() { return password; }
    void setUsername(const string &u) { username = u; }
    void setPassword(const string &p) { password = p; }
    string toFileString() { return username + "|" + password; }
    void fromFileString(const string &line) {
        int pos = line.find('|');
        if(pos==string::npos) { username = line; password = ""; return; }
        username = line.substr(0,pos);
        password = line.substr(pos+1);
    }
};

class IssueRecord {
public:
    int bookId;
    int expectedDays;
    IssueRecord(int b=0,int d=0) { bookId=b; expectedDays=d; }
};

class Library {
private:
    vector<Book> books;
    vector<IssueRecord> issues;
    Admin admin;
    string filename;
    string adminFilename;
    void saveBooks() {
        ofstream fout(filename.c_str());
        for(size_t i=0;i<books.size();++i) fout << books[i].toFileString() << "\n";
        fout.close();
    }
    void loadBooks() {
        ifstream fin(filename.c_str());
        if(!fin) { preload(); saveBooks(); return; }
        books.clear();
        string line;
        while(getline(fin,line)) {
            if(line.empty()) continue;
            Book b;
            b.fromFileString(line);
            books.push_back(b);
        }
        fin.close();
        if(books.empty()) { preload(); saveBooks(); }
    }
    void preload() {
        books.push_back(Book(101,"C Programming","Dennis Ritchie",450,5));
        books.push_back(Book(102,"Digital Electronics","Morris Mano",600,3));
        books.push_back(Book(103,"Data Structures","Mark Weiss",550,4));
        books.push_back(Book(104,"Operating Systems","Galvin",700,5));
        books.push_back(Book(105,"Database Systems","Navathe",500,4));
    }
    int findBookIndex(int id) {
        for(size_t i=0;i<books.size();++i) if(books[i].getId()==id) return (int)i;
        return -1;
    }
    void displayHeader() { cout << left << setw(8) << "ID" << setw(25) << "Title" << setw(20) << "Author" << setw(10) << "Price" << setw(10) << "Avail" << "\n"; }
    void saveAdmin() {
        ofstream fout(adminFilename.c_str());
        fout << admin.toFileString() << "\n";
        fout.close();
    }
    void loadAdmin() {
        ifstream fin(adminFilename.c_str());
        if(!fin) { saveAdmin(); return; }
        string line;
        if(getline(fin,line)) {
            if(!line.empty()) admin.fromFileString(line);
        }
        fin.close();
    }
    void changeAdminCredentials() {
        string currentPwd;
        cout << "Enter current password: ";
        cin >> currentPwd;
        if(currentPwd != admin.getPassword()) { cout << "Incorrect password. Credentials not changed.\n"; return; }
        cin.ignore();
        string newUsername;
        cout << "Enter new username (leave blank to keep current): ";
        getline(cin,newUsername);
        string newPassword, confirmPassword;
        cout << "Enter new password: ";
        getline(cin,newPassword);
        cout << "Confirm new password: ";
        getline(cin,confirmPassword);
        if(newPassword.empty()) { cout << "New password cannot be empty. Credentials not changed.\n"; return; }
        if(newPassword != confirmPassword) { cout << "Passwords do not match. Credentials not changed.\n"; return; }
        if(!newUsername.empty()) admin.setUsername(newUsername);
        admin.setPassword(newPassword);
        saveAdmin();
        cout << "Admin credentials updated successfully!\n";
    }
public:
    Library() : filename("books.txt"), adminFilename("admin.txt") { loadAdmin(); loadBooks(); }
    void displayBooks() {
        if(books.empty()) { cout << "No books available.\n"; return; }
        displayHeader();
        for(size_t i=0;i<books.size();++i) books[i].display();
    }
    void addBook() {
        int id,price,qty;
        string title,author;
        cout << "Enter Book ID: ";
        cin >> id;
        if(findBookIndex(id)!=-1) { cout << "Book ID already exists.\n"; return; }
        cin.ignore();
        cout << "Enter Title: ";
        getline(cin,title);
        cout << "Enter Author: ";
        getline(cin,author);
        cout << "Enter Price: ";
        cin >> price;
        cout << "Enter Quantity: ";
        cin >> qty;
        books.push_back(Book(id,title,author,price,qty));
        saveBooks();
        cout << "Book Added Successfully!\n";
    }
    void adminMenu() {
        while(1) {
            cout << "\n========== ADMIN PAGE ==========\n";
            cout << "1. View Books\n2. Add Book\n3. Update Admin Credentials\n4. Logout\n";
            cout << "Enter choice: ";
            int ch; cin >> ch;
            if(ch==1) displayBooks();
            else if(ch==2) addBook();
            else if(ch==3) changeAdminCredentials();
            else if(ch==4) break;
            else cout << "Invalid choice.\n";
        }
    }
    void buyBook() {
        int id;
        cout << "Enter Book ID to buy: ";
        cin >> id;
        int idx = findBookIndex(id);
        if(idx==-1) { cout << "Book not found.\n"; return; }
        if(!books[idx].isAvailable()) { cout << "No copies available.\n"; return; }
        int days;
        cout << "For how many days you want the book: ";
        cin >> days;
        if(days<=0) { cout << "Invalid days.\n"; return; }
        int amount = days*RENT_PER_DAY;
        cout << "Total amount: ?" << amount << "\n";
        books[idx].addIssue();
        issues.push_back(IssueRecord(id,days));
        saveBooks();
        cout << "Book purchased successfully!\n";
    }
    void returnBook() {
        int id;
        cout << "Enter Book ID to return: ";
        cin >> id;
        int idx = findBookIndex(id);
        if(idx==-1) { cout << "Book not found.\n"; return; }
        int recIndex=-1;
        for(int i=0;i<(int)issues.size();++i) if(issues[i].bookId==id) { recIndex=i; break; }
        if(recIndex==-1) { cout << "No record found.\n"; return; }
        int keptDays;
        cout << "How many days did you keep the book: ";
        cin >> keptDays;
        int expected = issues[recIndex].expectedDays;
        if(keptDays>expected) {
            int delay = keptDays-expected;
            int fine = delay*FINE_PER_DAY;
            cout << "You delayed by " << delay << " days.\n";
            cout << "Fine: ?" << fine << "\n";
        } else cout << "No fine.\n";
        issues.erase(issues.begin()+recIndex);
        books[idx].returnIssue();
        saveBooks();
        cout << "Book returned successfully!\n";
    }
    void userMenu() {
        while(1) {
            cout << "\n=========== USER PAGE ===========\n";
            cout << "1. View Books\n2. Buy Book\n3. Return Book\n4. Back\n";
            cout << "Enter choice: ";
            int ch; cin >> ch;
            if(ch==1) displayBooks();
            else if(ch==2) buyBook();
            else if(ch==3) returnBook();
            else if(ch==4) break;
            else cout << "Invalid choice.\n";
        }
    }
    void run() {
        while(1) {
            cout << "\n===============LIBRARY MANAGEMENT SYSTEM================\n";
            cout << "1. Admin Page\n2. User Page\n3. Exit\n";
            cout << "Enter choice: ";
            int ch; cin >> ch;
            if(ch==1) {
                string u,p;
                cout << "Enter username: ";
                cin >> u;
                cout << "Enter password: ";
                cin >> p;
                if(admin.authenticate(u,p)) adminMenu();
                else cout << "Invalid login....\n";
            } else if(ch==2) userMenu();
            else if(ch==3) { cout<<"========= --Thank you for using the system-- ======"; break;}
            else cout << "Invalid choice.\n";
        }
    }
};

int main() {
    Library lib;
    lib.run();
    return 0;
}