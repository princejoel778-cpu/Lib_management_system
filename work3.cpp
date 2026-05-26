#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <ctime>

using namespace std;

struct Book {
    int id;
    string title;
    string author;
    string isbn;
    string genre;
    bool isIssued;
    int issuedToMemberId;

    Book(int id, string title, string author, string isbn, string genre)
        : id(id), title(title), author(author), isbn(isbn), genre(genre),
          isIssued(false), issuedToMemberId(-1) {}
};

struct Member {
    int id;
    string name;
    string email;
    string phone;

    Member(int id, string name, string email, string phone)
        : id(id), name(name), email(email), phone(phone) {}
};

struct BorrowRecord {
    int recordId;
    int bookId;
    string bookTitle;
    int memberId;
    string memberName;
    string issueDate;
    string dueDate;
    string returnDate;
    bool returned;

    BorrowRecord(int rid, int bid, string btitle, int mid, string mname,
                 string idate, string ddate)
        : recordId(rid), bookId(bid), bookTitle(btitle), memberId(mid),
          memberName(mname), issueDate(idate), dueDate(ddate),
          returnDate(""), returned(false) {}
};

class LibrarySystem {
    vector<Book> books;
    vector<Member> members;
    vector<BorrowRecord> records;
    int bookIdCounter;
    int memberIdCounter;
    int recordIdCounter;

    string getCurrentDate() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char buf[11];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d",
                 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
        return string(buf);
    }

    string getDueDate(int days = 14) {
        time_t now = time(0) + days * 86400;
        tm* ltm = localtime(&now);
        char buf[11];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d",
                 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
        return string(buf);
    }

    void printLine(int width = 70) {
        cout << string(width, '-') << "\n";
    }

    void printHeader(const string& title) {
        cout << "\n";
        printLine();
        cout << "  " << title << "\n";
        printLine();
    }

public:
    LibrarySystem() : bookIdCounter(1), memberIdCounter(1), recordIdCounter(1) {
        books.emplace_back(bookIdCounter++, "The Great Gatsby", "F. Scott Fitzgerald", "978-0743273565", "Fiction");
        books.emplace_back(bookIdCounter++, "To Kill a Mockingbird", "Harper Lee", "978-0061935466", "Classic");
        books.emplace_back(bookIdCounter++, "Clean Code", "Robert C. Martin", "978-0132350884", "Technology");
        members.emplace_back(memberIdCounter++, "Alice Johnson", "alice@email.com", "9876543210");
        members.emplace_back(memberIdCounter++, "Bob Smith", "bob@email.com", "9123456789");
    }

    void addBook() {
        printHeader("ADD NEW BOOK");
        string title, author, isbn, genre;
        cin.ignore();
        cout << "  Title  : "; getline(cin, title);
        cout << "  Author : "; getline(cin, author);
        cout << "  ISBN   : "; getline(cin, isbn);
        cout << "  Genre  : "; getline(cin, genre);

        if (title.empty() || author.empty()) {
            cout << "\n  [Error] Title and Author are required.\n";
            return;
        }
        books.emplace_back(bookIdCounter++, title, author,
                           isbn.empty() ? "N/A" : isbn,
                           genre.empty() ? "General" : genre);
        cout << "\n  [OK] Book added with ID: B" << setw(3) << setfill('0') << (bookIdCounter - 1) << "\n";
    }

    void addMember() {
        printHeader("ADD NEW MEMBER");
        string name, email, phone;
        cin.ignore();
        cout << "  Name   : "; getline(cin, name);
        cout << "  Email  : "; getline(cin, email);
        cout << "  Phone  : "; getline(cin, phone);

        if (name.empty()) {
            cout << "\n  [Error] Member name is required.\n";
            return;
        }
        members.emplace_back(memberIdCounter++, name,
                             email.empty() ? "N/A" : email,
                             phone.empty() ? "N/A" : phone);
        cout << "\n  [OK] Member added with ID: M" << setw(3) << setfill('0') << (memberIdCounter - 1) << "\n";
    }

    void listBooks(const vector<Book>& bookList) {
        if (bookList.empty()) {
            cout << "\n  No books found.\n";
            return;
        }
        cout << "\n  " << left
             << setw(5) << "ID"
             << setw(28) << "Title"
             << setw(22) << "Author"
             << setw(12) << "Genre"
             << setw(10) << "Status" << "\n";
        printLine();
        for (const auto& b : bookList) {
            cout << "  "
                 << setw(5) << ("B" + to_string(b.id))
                 << setw(28) << (b.title.length() > 26 ? b.title.substr(0, 25) + ".." : b.title)
                 << setw(22) << (b.author.length() > 20 ? b.author.substr(0, 19) + ".." : b.author)
                 << setw(12) << b.genre
                 << setw(10) << (b.isIssued ? "Issued" : "Available") << "\n";
        }
    }

    void viewAllBooks() {
        printHeader("ALL BOOKS");
        listBooks(books);
    }

    void viewAllMembers() {
        printHeader("ALL MEMBERS");
        if (members.empty()) {
            cout << "\n  No members found.\n";
            return;
        }
        cout << "\n  " << left
             << setw(6)  << "ID"
             << setw(22) << "Name"
             << setw(26) << "Email"
             << setw(14) << "Phone" << "\n";
        printLine();
        for (const auto& m : members) {
            cout << "  "
                 << setw(6)  << ("M" + to_string(m.id))
                 << setw(22) << m.name
                 << setw(26) << m.email
                 << setw(14) << m.phone << "\n";
        }
    }

    void issueBook() {
        printHeader("ISSUE BOOK");

        vector<Book*> available;
        for (auto& b : books)
            if (!b.isIssued) available.push_back(&b);

        if (available.empty()) {
            cout << "\n  [Error] No books available for issue.\n";
            return;
        }
        if (members.empty()) {
            cout << "\n  [Error] No members registered.\n";
            return;
        }

        cout << "\n  Available Books:\n";
        for (auto* b : available)
            cout << "    B" << b->id << " - " << b->title << " by " << b->author << "\n";

        cout << "\n  Registered Members:\n";
        for (const auto& m : members)
            cout << "    M" << m.id << " - " << m.name << "\n";

        cout << "\n  Enter Book ID   : B"; int bid; cin >> bid;
        cout << "  Enter Member ID : M"; int mid; cin >> mid;

        auto bookIt = find_if(books.begin(), books.end(),
                              [bid](const Book& b) { return b.id == bid; });
        auto memIt  = find_if(members.begin(), members.end(),
                              [mid](const Member& m) { return m.id == mid; });

        if (bookIt == books.end()) { cout << "\n  [Error] Book not found.\n"; return; }
        if (memIt  == members.end()) { cout << "\n  [Error] Member not found.\n"; return; }
        if (bookIt->isIssued) { cout << "\n  [Error] Book is already issued.\n"; return; }

        bookIt->isIssued = true;
        bookIt->issuedToMemberId = mid;

        string today = getCurrentDate();
        string due   = getDueDate(14);

        records.emplace_back(recordIdCounter++, bid, bookIt->title,
                             mid, memIt->name, today, due);

        cout << "\n  [OK] \"" << bookIt->title << "\" issued to " << memIt->name << "\n";
        cout << "       Issue Date : " << today << "\n";
        cout << "       Due Date   : " << due   << "\n";
    }

    void returnBook() {
        printHeader("RETURN BOOK");

        vector<BorrowRecord*> active;
        for (auto& r : records)
            if (!r.returned) active.push_back(&r);

        if (active.empty()) {
            cout << "\n  No books currently issued.\n";
            return;
        }

        cout << "\n  Currently Issued:\n";
        cout << "  " << left << setw(6) << "RecID" << setw(28) << "Book" << setw(18) << "Member" << setw(12) << "Due\n";
        printLine();
        for (auto* r : active)
            cout << "  " << setw(6) << r->recordId
                 << setw(28) << (r->bookTitle.length() > 26 ? r->bookTitle.substr(0,25)+".." : r->bookTitle)
                 << setw(18) << r->memberName
                 << setw(12) << r->dueDate << "\n";

        cout << "\n  Enter Record ID to return: "; int rid; cin >> rid;

        auto recIt = find_if(records.begin(), records.end(),
                             [rid](const BorrowRecord& r) { return r.recordId == rid && !r.returned; });

        if (recIt == records.end()) { cout << "\n  [Error] Record not found or already returned.\n"; return; }

        recIt->returned   = true;
        recIt->returnDate = getCurrentDate();

        auto bookIt = find_if(books.begin(), books.end(),
                              [&](const Book& b) { return b.id == recIt->bookId; });
        if (bookIt != books.end()) {
            bookIt->isIssued = false;
            bookIt->issuedToMemberId = -1;
        }

        cout << "\n  [OK] \"" << recIt->bookTitle << "\" returned by " << recIt->memberName << "\n";
        cout << "       Return Date: " << recIt->returnDate << "\n";
    }

    void searchBooks() {
        printHeader("SEARCH BOOKS");
        cin.ignore();
        string query;
        cout << "  Enter title or author to search: ";
        getline(cin, query);

        if (query.empty()) { cout << "\n  [Error] Search query cannot be empty.\n"; return; }

        string lq = query;
        transform(lq.begin(), lq.end(), lq.begin(), ::tolower);

        vector<Book> results;
        for (const auto& b : books) {
            string lt = b.title, la = b.author;
            transform(lt.begin(), lt.end(), lt.begin(), ::tolower);
            transform(la.begin(), la.end(), la.begin(), ::tolower);
            if (lt.find(lq) != string::npos || la.find(lq) != string::npos)
                results.push_back(b);
        }

        cout << "\n  Found " << results.size() << " result(s) for \"" << query << "\":\n";
        listBooks(results);
    }

    void viewBorrowRecords() {
        printHeader("BORROW RECORDS");
        if (records.empty()) {
            cout << "\n  No records found.\n";
            return;
        }
        cout << "\n  " << left
             << setw(5)  << "ID"
             << setw(26) << "Book"
             << setw(18) << "Member"
             << setw(12) << "Issued"
             << setw(12) << "Due"
             << setw(12) << "Returned"
             << setw(10) << "Status" << "\n";
        printLine(95);
        for (const auto& r : records) {
            cout << "  "
                 << setw(5)  << r.recordId
                 << setw(26) << (r.bookTitle.length() > 24 ? r.bookTitle.substr(0,23)+".." : r.bookTitle)
                 << setw(18) << r.memberName
                 << setw(12) << r.issueDate
                 << setw(12) << r.dueDate
                 << setw(12) << (r.returned ? r.returnDate : "—")
                 << setw(10) << (r.returned ? "Returned" : "Active") << "\n";
        }
    }

    void deleteBook() {
        printHeader("DELETE BOOK");
        viewAllBooks();
        cout << "\n  Enter Book ID to delete: B"; int bid; cin >> bid;

        auto it = find_if(books.begin(), books.end(),
                          [bid](const Book& b) { return b.id == bid; });
        if (it == books.end()) { cout << "\n  [Error] Book not found.\n"; return; }
        if (it->isIssued)       { cout << "\n  [Error] Cannot delete an issued book.\n"; return; }

        cout << "  Confirm delete \"" << it->title << "\"? (y/n): ";
        char ch; cin >> ch;
        if (ch == 'y' || ch == 'Y') {
            books.erase(it);
            cout << "  [OK] Book deleted.\n";
        } else {
            cout << "  Cancelled.\n";
        }
    }

    void showStats() {
        printHeader("LIBRARY STATISTICS");
        int issued   = count_if(books.begin(), books.end(), [](const Book& b) { return b.isIssued; });
        int available = (int)books.size() - issued;
        int activeRec = count_if(records.begin(), records.end(), [](const BorrowRecord& r) { return !r.returned; });

        cout << "\n  Total Books    : " << books.size()
             << "\n  Available      : " << available
             << "\n  Issued         : " << issued
             << "\n  Total Members  : " << members.size()
             << "\n  Active Borrows : " << activeRec
             << "\n  Total Records  : " << records.size() << "\n";
    }

    void run() {
        cout << "\n========================================\n";
        cout << "     LIBRARY MANAGEMENT SYSTEM\n";
        cout << "========================================\n";

        while (true) {
            cout << "\n  MAIN MENU\n";
            printLine(40);
            cout << "  1. View All Books\n";
            cout << "  2. Add Book\n";
            cout << "  3. Delete Book\n";
            cout << "  4. View All Members\n";
            cout << "  5. Add Member\n";
            cout << "  6. Issue Book\n";
            cout << "  7. Return Book\n";
            cout << "  8. Search Books\n";
            cout << "  9. View Borrow Records\n";
            cout << "  10. Library Statistics\n";
            cout << "  0. Exit\n";
            printLine(40);
            cout << "  Choice: ";

            int choice;
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "  [Error] Invalid input.\n";
                continue;
            }

            switch (choice) {
                case 1: viewAllBooks();      break;
                case 2: addBook();           break;
                case 3: deleteBook();        break;
                case 4: viewAllMembers();    break;
                case 5: addMember();         break;
                case 6: issueBook();         break;
                case 7: returnBook();        break;
                case 8: searchBooks();       break;
                case 9: viewBorrowRecords(); break;
                case 10: showStats();        break;
                case 0:
                    cout << "\n  Goodbye!\n\n";
                    return;
                default:
                    cout << "\n  [Error] Invalid choice. Enter 0-10.\n";
            }
        }
    }
};

int main() {
    LibrarySystem library;
    library.run();
    return 0;
}