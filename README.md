# 📚 LibrarySys — C++ Library Management System

A full-stack **Library Management System** built with **C++** following strict **Object-Oriented Design Principles**. The backend is a pure C++ REST API served by `cpp-httplib`; the frontend is a responsive, dark-themed multi-page web application.

🌐 **Live Demo:** [library-sys.vercel.app](https://library-sys.vercel.app)

---

## 🏗️ Architecture

```
┌─────────────────────────────────────────────────────┐
│                  Frontend (Browser)                  │
│  HTML + CSS + Vanilla JS  ·  5 separate pages        │
│  Auto-detects C++ backend → falls back to localStorage│
└────────────────────┬────────────────────────────────┘
                     │ HTTP / REST
┌────────────────────▼────────────────────────────────┐
│              C++ Backend  (cpp-httplib)               │
│  BookRoutes · MemberRoutes · TransactionRoutes       │
└────────────────────┬────────────────────────────────┘
                     │ Interfaces (IRepository<T>)
┌────────────────────▼────────────────────────────────┐
│            Persistence Layer (JSON files)             │
│  FileBookRepository · FileMemberRepository · …       │
└─────────────────────────────────────────────────────┘
```

## 📁 Project Structure

```
LibrarySys/
├── src/
│   ├── domain/          # Pure C++ OOP — zero external dependencies
│   │   ├── User.h/.cpp          (abstract base)
│   │   ├── Member.h/.cpp        (extends User)
│   │   ├── Book.h/.cpp
│   │   ├── Transaction.h/.cpp
│   │   ├── FineCalculator.h/.cpp
│   │   └── IBookSearch.h        (interface)
│   ├── services/        # Business logic orchestration
│   │   ├── LibraryService.h/.cpp
│   │   └── SearchService.h/.cpp
│   ├── persistence/     # JSON file repositories (IRepository<T>)
│   │   ├── IRepository.h
│   │   ├── FileBookRepository.h/.cpp
│   │   ├── FileMemberRepository.h/.cpp
│   │   └── FileTransactionRepository.h/.cpp
│   ├── web/             # HTTP layer (cpp-httplib)
│   │   ├── WebServer.h/.cpp
│   │   └── routes/
│   │       ├── BookRoutes.h/.cpp
│   │       ├── MemberRoutes.h/.cpp
│   │       └── TransactionRoutes.h/.cpp
│   └── main.cpp         # Composition root — DI wiring
├── frontend/            # Multi-page SPA (served by C++ or Vercel)
│   ├── index.html           Dashboard
│   ├── books.html           Book Catalogue
│   ├── members.html         Member Directory
│   ├── transactions.html    Transaction History
│   ├── borrow.html          Borrow / Return
│   ├── css/style.css
│   └── js/
│       ├── shared.js        API helpers, localStorage fallback, sidebar
│       ├── dashboard.js
│       ├── books.js
│       ├── members.js
│       ├── transactions.js
│       └── borrow.js
├── data/                # JSON data files (auto-created on first run)
│   ├── books.json
│   ├── members.json
│   └── transactions.json
├── third_party/         # Single-header libraries (no install needed)
│   ├── httplib.h        cpp-httplib v0.10.1
│   └── json.hpp         nlohmann/json v3.10.5
├── .vscode/             # VS Code build task & IntelliSense config
├── Dockerfile           # Docker build for Linux deployment (Render etc.)
├── vercel.json          # Vercel routing config
└── .gitignore
```

---

## 🚀 Build & Run

### Prerequisites
- **Compiler:** MinGW GCC (ensure `C:\MinGW\bin` is in your PATH)
- **Editor:** VS Code with C/C++ Extension (recommended)

### Quick Build (VS Code)
1. Press **`Ctrl+Shift+B`**
2. Wait for the terminal to show no errors
3. Open a new terminal and run:
```powershell
.\build\bin\LibrarySystem.exe
```
4. Open **http://localhost:8080** in your browser

### Manual Build (PowerShell)
```powershell
# Create output directory
New-Item -ItemType Directory -Force "build\bin" | Out-Null

# Compile
g++ -std=c++14 -O2 -w -D_WIN32_WINNT=0x0600 `
  -I"src/domain" -I"src/services" -I"src/persistence" `
  -I"src/web" -I"src/web/routes" -I"third_party" `
  src/domain/User.cpp src/domain/Member.cpp `
  src/domain/Book.cpp src/domain/Transaction.cpp src/domain/FineCalculator.cpp `
  src/services/LibraryService.cpp src/services/SearchService.cpp `
  src/persistence/FileBookRepository.cpp src/persistence/FileMemberRepository.cpp `
  src/persistence/FileTransactionRepository.cpp `
  src/web/WebServer.cpp `
  src/web/routes/BookRoutes.cpp src/web/routes/MemberRoutes.cpp `
  src/web/routes/TransactionRoutes.cpp `
  src/main.cpp `
  -o "build\bin\LibrarySystem.exe" -lws2_32

# Run
.\build\bin\LibrarySystem.exe
```

### Docker (Linux / Cloud)
```bash
docker build -t librarysys .
docker run -p 8080:8080 librarysys
```

---

## 🌐 Vercel Deployment (Frontend)

The frontend auto-detects whether the C++ backend is reachable:

| Mode | Backend | Status Badge |
|------|---------|-------------|
| **Local** (`localhost:8080`) | C++ REST API | 🟢 API Connected |
| **Vercel** (no backend) | Browser `localStorage` | 🟡 Demo Mode |

In **Demo Mode**, all CRUD operations (add/delete books, borrow, return, fines) work fully in the browser using `localStorage`. Data persists across page refreshes.

---

## 🌐 REST API

| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/api/books` | List all books |
| GET | `/api/books?q=query` | Search books by title/author |
| GET | `/api/books/:id` | Get a single book |
| POST | `/api/books` | Add a new book |
| DELETE | `/api/books/:id` | Delete a book |
| GET | `/api/members` | List all members |
| GET | `/api/members/:id` | Get a single member |
| POST | `/api/members` | Register a new member |
| DELETE | `/api/members/:id` | Delete a member |
| GET | `/api/transactions` | List all transactions |
| GET | `/api/transactions?active=true` | Active borrows only |
| POST | `/api/borrow` | Issue a book to a member |
| POST | `/api/return` | Return a book (auto-calculates fine) |

---

## 🔑 OOP Design Highlights

| Concept | Where Applied |
|---------|--------------|
| **Abstraction** | `IBookSearch`, `IRepository<T>` interfaces |
| **Encapsulation** | `Transaction::computeFine()` owns all fine logic |
| **Inheritance** | `Member` extends abstract `User` |
| **Polymorphism** | `FileBookRepository` used via `IRepository<Book>*` |
| **Dependency Inversion** | Services depend on interfaces, not concrete files |
| **Single Responsibility** | Domain ≠ Service ≠ Web ≠ Persistence |

---

## 🛠️ Tech Stack

| Layer | Technology |
|-------|-----------|
| Backend | C++14, [cpp-httplib](https://github.com/yhirose/cpp-httplib) v0.10.1 |
| JSON | [nlohmann/json](https://github.com/nlohmann/json) v3.10.5 |
| Frontend | HTML5, Vanilla CSS, Vanilla JavaScript |
| Fonts | Google Fonts — Inter |
| Hosting | Vercel (frontend) + optional Docker (backend) |
| Compiler | MinGW GCC 6.3.0 (Windows) / GCC 12 (Docker/Linux) |
