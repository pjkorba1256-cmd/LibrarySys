# 📚 LibrarySys — C++ Library Management System

A full-stack library management system built with **C++** and strict **Object-Oriented Design Principles**. The backend is a pure C++ REST API; the frontend is a premium dark-themed web SPA.

---

## 🏗️ Architecture

```
Domain Layer (Book, Member, Transaction)
    ↑ injected into
Service Layer (LibraryService, SearchService)
    ↑ injected into
Web Layer (REST API via cpp-httplib)
    ↑ serves
Frontend (Vanilla JS SPA)

Persistence Layer (FileBookRepository etc.) ← pluggable via IRepository<T>
```

## 📁 Project Structure

```
LibrarySystem/
├── src/
│   ├── domain/          # Pure C++ OOP — no external dependencies
│   ├── services/        # Orchestration (LibraryService, SearchService)
│   ├── persistence/     # JSON file repositories (swap to SQLite easily)
│   ├── web/             # HTTP routes (BookRoutes, MemberRoutes, etc.)
│   └── main.cpp         # Composition root (Dependency Injection)
├── frontend/            # HTML/CSS/JS SPA (served by C++ backend)
├── data/                # JSON data files (books, members, transactions)
├── third_party/         # httplib.h + json.hpp (single-header, no install needed)
└── .vscode/             # Tasks and build config
```

## 🚀 Build & Run

### Prerequisites
- VS Code with C/C++ Extension
- MinGW GCC (ensure `C:\MinGW\bin` is in your PATH)

### Quick Build (VS Code)
1. Press `Ctrl+Shift+B` to run the Build Task.
2. Wait for the terminal to display `Build finished successfully`.
3. Open a new terminal and run:
   ```bash
   .\build\bin\LibrarySystem.exe
   ```

### Manual Build (Command Line)
```bash
if (!(Test-Path "build\bin")) { New-Item -ItemType Directory -Path "build\bin" -Force }
g++ -std=c++14 -O2 -w -D_WIN32_WINNT=0x0600 -I"src/domain" -I"src/services" -I"src/persistence" -I"src/web" -I"src/web/routes" -I"third_party" src/domain/User.cpp src/domain/Member.cpp src/domain/Book.cpp src/domain/Transaction.cpp src/domain/FineCalculator.cpp src/services/LibraryService.cpp src/services/SearchService.cpp src/services/MemberService.cpp src/persistence/FileBookRepository.cpp src/persistence/FileMemberRepository.cpp src/persistence/FileTransactionRepository.cpp src/web/WebServer.cpp src/web/routes/BookRoutes.cpp src/web/routes/MemberRoutes.cpp src/web/routes/TransactionRoutes.cpp src/main.cpp -o "build\bin\LibrarySystem.exe" -lws2_32
.\build\bin\LibrarySystem.exe
```

Then open **http://localhost:8080** in your browser.

---

## 🌐 REST API

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/api/books` | List all books |
| GET | `/api/books?q=query` | Search books |
| GET | `/api/books/:id` | Get single book |
| POST | `/api/books` | Add new book |
| DELETE | `/api/books/:id` | Delete book |
| GET | `/api/members` | List all members |
| GET | `/api/members/:id` | Get single member |
| POST | `/api/members` | Register member |
| DELETE | `/api/members/:id` | Delete member |
| GET | `/api/transactions` | List all transactions |
| GET | `/api/transactions?active=true` | Active borrows only |
| POST | `/api/borrow` | Borrow a book |
| POST | `/api/return` | Return a book |
| POST | `/api/lost` | Mark book as lost |

---

## 🔑 OOP Design Highlights

| Concept | Where Applied |
|---------|--------------|
| **Abstraction** | `IBookSearch`, `IRepository<T>` |
| **Encapsulation** | `Transaction::computeFine()` owns all fine logic |
| **Inheritance** | `Member` extends abstract `User` |
| **Polymorphism** | `FileBookRepository` used via `IRepository<Book>*` |
| **Dependency Inversion** | Services depend on interfaces, not concrete files |
| **Single Responsibility** | Domain ≠ Service ≠ Web ≠ Persistence |
