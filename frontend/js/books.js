initSidebar('books');

let allBooks = [];

async function handleSearch(q) {
  if (!q.trim()) { renderBooks(allBooks); return; }
  const results = await apiFetch('/api/books?q=' + encodeURIComponent(q));
  renderBooks(Array.isArray(results) ? results : []);
}

function renderBooks(books) {
  const el = document.getElementById('books-container');
  if (!books || books.length === 0) {
    el.innerHTML = '<div class="empty-state"><div class="empty-icon">📭</div><p>No books found.</p></div>';
    return;
  }
  el.innerHTML = `
    <div class="table-wrap">
      <table>
        <thead><tr>
          <th>#</th><th>Title</th><th>Author</th><th>Category</th>
          <th>ISBN</th><th>Copies</th><th>Available</th><th>Status</th><th></th>
        </tr></thead>
        <tbody>${books.map(b => `
          <tr>
            <td style="color:var(--text-muted)">${b.id}</td>
            <td><strong>${esc(b.title)}</strong></td>
            <td>${esc(b.author)}</td>
            <td><span class="badge badge-purple">${esc(b.category)}</span></td>
            <td style="color:var(--text-muted);font-family:monospace;font-size:12px">${esc(b.isbn)}</td>
            <td>${b.totalCopies}</td>
            <td><strong style="color:var(--success)">${b.availableCopies}</strong></td>
            <td><span class="badge ${b.availableCopies > 0 ? 'badge-green' : 'badge-red'}">${b.status}</span></td>
            <td><button class="btn btn-danger" onclick="deleteBook(${b.id})">Delete</button></td>
          </tr>`).join('')}
        </tbody>
      </table>
    </div>`;
}

async function handleAddBook(e) {
  e.preventDefault();
  const book = {
    title:    document.getElementById('book-title').value,
    author:   document.getElementById('book-author').value,
    isbn:     document.getElementById('book-isbn').value,
    category: document.getElementById('book-category').value || 'General',
    copies:   parseInt(document.getElementById('book-copies').value) || 1
  };
  const res = await apiFetch('/api/books', { method:'POST', body: JSON.stringify(book) });
  if (res.id) {
    showToast('Book added successfully!', 'success');
    closeModal('add-book-modal');
    e.target.reset();
    loadBooks();
  } else {
    showToast(res.error || 'Failed to add book', 'error');
  }
}

async function deleteBook(id) {
  if (!confirm('Delete this book?')) return;
  await apiFetch('/api/books/' + id, { method:'DELETE' });
  showToast('Book deleted', 'success');
  loadBooks();
}

async function loadBooks() {
  await detectMode();
  allBooks = await apiFetch('/api/books');
  renderBooks(allBooks);
}

// Handle search query from URL (from dashboard)
const urlQ = new URLSearchParams(location.search).get('q');
if (urlQ) { document.getElementById('search-input').value = urlQ; handleSearch(urlQ); }
else loadBooks();
