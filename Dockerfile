# ── Build stage ───────────────────────────────────────────────
FROM gcc:12 AS builder

WORKDIR /app
COPY . .

# Create data directory
RUN mkdir -p data

# Compile with full C++17 + pthread (Linux GCC 12 has full thread support)
RUN g++ -std=c++17 -O2 -w \
    -Isrc/domain \
    -Isrc/services \
    -Isrc/persistence \
    -Isrc/web \
    -Isrc/web/routes \
    -Ithird_party \
    src/domain/User.cpp \
    src/domain/Member.cpp \
    src/domain/Book.cpp \
    src/domain/Transaction.cpp \
    src/domain/FineCalculator.cpp \
    src/services/LibraryService.cpp \
    src/services/SearchService.cpp \
    src/services/MemberService.cpp \
    src/persistence/FileBookRepository.cpp \
    src/persistence/FileMemberRepository.cpp \
    src/persistence/FileTransactionRepository.cpp \
    src/web/WebServer.cpp \
    src/web/routes/BookRoutes.cpp \
    src/web/routes/MemberRoutes.cpp \
    src/web/routes/TransactionRoutes.cpp \
    src/main.cpp \
    -o LibrarySystem \
    -lpthread

# ── Runtime stage ─────────────────────────────────────────────
FROM debian:bookworm-slim

WORKDIR /app

# Copy binary, frontend and initial data
COPY --from=builder /app/LibrarySystem .
COPY --from=builder /app/frontend ./frontend
COPY --from=builder /app/data ./data

# Render injects $PORT; we read it in main.cpp
EXPOSE 8080

CMD ["./LibrarySystem"]
