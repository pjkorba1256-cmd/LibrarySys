// ── API base URL (same origin when served by C++ backend) ────
const API = '';

// ════════════════════════════════════════════════════════════
//  Navigation
// ════════════════════════════════════════════════════════════
function showPage(name) {
  document.querySelectorAll('.page').forEach(p => p.classList.remove('active'));
  document.querySelectorAll('.nav-btn').forEach(b => b.classList.remove('active'));
  document.getElementById('page-' + name).classList.add('active');
  document.getElementById('nav-' + name).classList.add('active');
  const titles = { dashboard:'Dashboard', books:'Books', members:'Members', transactions:'Transactions', borrow:'Borrow / Return' };
  document.getElementById('page-title').textContent = titles[name] || name;
  if (name === 'books')        loadBooks();
  if (name === 'members')      loadMembers();
  if (name === 'transactions') loadTransactions();
}

// ════════════════════════════════════════════════════════════
//  API Helpers
// ════════════════════════════════════════════════════════════
async function apiFetch(path, options = {}) {
  const res = await fetch(API + path, {
    headers: { 'Content-Type': 'application/json' },
    ...options
  });
  return res.json();
}

// ════════════════════════════════════════════════════════════
//  Toast
// ════════════════════════════════════════════════════════════
function showToast(msg, type = 'info') {
  const t = document.getElementById('toast');
  t.textContent = msg;
  t.style.color = type === 'success' ? '#10b981' : type === 'error' ? '#ef4444' : '#e2e8f0';
  t.style.borderColor = type === 'success' ? 'rgba(16,185,129,0.3)' : type === 'error' ? 'rgba(239,68,68,0.3)' : '#2a2d3e';
  t.classList.remove('hidden');
  setTimeout(() => t.classList.add('hidden'), 3000);
}

// ════════════════════════════════════════════════════════════
//  Modal
// ════════════════════════════════════════════════════════════
function openModal(id)  { document.getElementById(id).classList.remove('hidden'); }
function closeModal(id) { document.getElementById(id).classList.add('hidden'); }
document.querySelectorAll('.modal-overlay').forEach(o => {
  o.addEventListener('click', e => { if (e.target === o) o.classList.add('hidden'); });
});

// ════════════════════════════════════════════════════════════
//  Dashboard
// ════════════════════════════════════════════════════════════
async function loadDashboard() {
  try {
    const [books, members, transactions] = await Promise.all([
      apiFetch('/api/books'),
      apiFetch('/api/members'),
      apiFetch('/api/transactions?active=true')
    ]);

    // Stats
    const totalBooks     = books.length;
    const available      = books.reduce((s, b) => s + (b.availableCopies || 0), 0);
    const totalMembers   = members.length;
    const activeBorrows  = Array.isArray(transactions) ? transactions.length : 0;

    document.getElementById('stat-books').textContent        = totalBooks;
    document.getElementById('stat-available').textContent    = available;
    document.getElementById('stat-members').textContent      = totalMembers;
    document.getElementById('stat-active-borrows').textContent = activeBorrows;

    // Recent books list
    const bookList = document.getElementById('recent-books-list');
    bookList.innerHTML = '';
    books.slice(0, 5).forEach(b => {
      bookList.innerHTML += `
        <div class="item-row">
          <div>
            <div class="item-label">${esc(b.title)}</div>
            <div class="item-sub">${esc(b.author)} · ${esc(b.category)}</div>
          </div>
          <span class="badge ${b.availableCopies > 0 ? 'badge-green' : 'badge-red'}">
            ${b.availableCopies > 0 ? b.availableCopies + ' avail' : 'Borrowed'}
          </span>
        </div>`;
    });

    // Active borrows list
    const borrowList = document.getElementById('active-borrows-list');
    borrowList.innerHTML = '';
    if (Array.isArray(transactions) && transactions.length > 0) {
      transactions.slice(0, 5).forEach(tx => {
        const overdue = tx.isOverdue ? '<span class="badge badge-red">Overdue</span>' : '<span class="badge badge-green">Active</span>';
        borrowList.innerHTML += `
          <div class="item-row">
            <div>
              <div class="item-label">Book #${tx.bookId} → Member #${tx.memberId}</div>
              <div class="item-sub">Due: ${formatTimestamp(tx.dueDate)}</div>
            </div>
            ${overdue}
          </div>`;
      });
    } else {
      borrowList.innerHTML = '<div style="color:var(--text-muted);font-size:13px;padding:8px">No active borrows.</div>';
    }

    // Check API connectivity
    document.getElementById('api-status').innerHTML = '<span class="status-dot"></span> API Connected';
  } catch (err) {
    document.getElementById('api-status').innerHTML = '<span style="color:var(--danger)">⚠ API Offline</span>';
  }
}

// ════════════════════════════════════════════════════════════
//  Books
// ════════════════════════════════════════════════════════════
async function loadBooks(books = null) {
  if (!books) books = await apiFetch('/api/books');
  const el = document.getElementById('books-table-container');
  if (!books || books.length === 0) {
    el.innerHTML = '<p style="color:var(--text-muted);padding:12px">No books found.</p>';
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
            <td><span class="badge badge-gray">${esc(b.category)}</span></td>
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

// ════════════════════════════════════════════════════════════
//  Members
// ════════════════════════════════════════════════════════════
async function loadMembers() {
  const members = await apiFetch('/api/members');
  const el = document.getElementById('members-table-container');
  if (!members || members.length === 0) {
    el.innerHTML = '<p style="color:var(--text-muted);padding:12px">No members found.</p>';
    return;
  }
  el.innerHTML = `
    <div class="table-wrap">
      <table>
        <thead><tr>
          <th>#</th><th>Name</th><th>Email</th><th>Active Borrows</th>
          <th>Fines Owed</th><th>Can Borrow</th><th></th>
        </tr></thead>
        <tbody>${members.map(m => `
          <tr>
            <td style="color:var(--text-muted)">${m.id}</td>
            <td><strong>${esc(m.name)}</strong></td>
            <td style="color:var(--text-muted)">${esc(m.email)}</td>
            <td>${m.activeBorrowCount}</td>
            <td style="color:${m.finesOwed > 0 ? 'var(--danger)' : 'var(--success)'}">₹${m.finesOwed.toFixed(2)}</td>
            <td><span class="badge ${m.canBorrow ? 'badge-green' : 'badge-red'}">${m.canBorrow ? 'Yes' : 'No'}</span></td>
            <td><button class="btn btn-danger" onclick="deleteMember(${m.id})">Delete</button></td>
          </tr>`).join('')}
        </tbody>
      </table>
    </div>`;
}

async function handleAddMember(e) {
  e.preventDefault();
  const data = {
    name:  document.getElementById('member-name').value,
    email: document.getElementById('member-email').value
  };
  const res = await apiFetch('/api/members', { method:'POST', body: JSON.stringify(data) });
  if (res.id) {
    showToast('Member registered!', 'success');
    closeModal('add-member-modal');
    e.target.reset();
    loadMembers();
  } else {
    showToast(res.error || 'Failed to register member', 'error');
  }
}

async function deleteMember(id) {
  if (!confirm('Delete this member?')) return;
  await apiFetch('/api/members/' + id, { method:'DELETE' });
  showToast('Member deleted', 'success');
  loadMembers();
}

// ════════════════════════════════════════════════════════════
//  Transactions
// ════════════════════════════════════════════════════════════
async function loadTransactions() {
  const txs = await apiFetch('/api/transactions');
  const el  = document.getElementById('transactions-table-container');
  if (!txs || txs.length === 0) {
    el.innerHTML = '<p style="color:var(--text-muted);padding:12px">No transactions yet.</p>';
    return;
  }
  el.innerHTML = `
    <div class="table-wrap">
      <table>
        <thead><tr>
          <th>#</th><th>Book ID</th><th>Member ID</th><th>Borrow Date</th>
          <th>Due Date</th><th>Return Date</th><th>Status</th>
          <th>Fine (₹)</th><th>Actions</th>
        </tr></thead>
        <tbody>${txs.map(tx => `
          <tr>
            <td style="color:var(--text-muted)">${tx.id}</td>
            <td>${tx.bookId}</td>
            <td>${tx.memberId}</td>
            <td>${formatTimestamp(tx.borrowDate)}</td>
            <td style="color:${tx.isOverdue ? 'var(--danger)' : 'inherit'}">${formatTimestamp(tx.dueDate)}</td>
            <td>${tx.returnDate && tx.returnDate !== '0' ? formatTimestamp(tx.returnDate) : '—'}</td>
            <td><span class="badge ${statusBadge(tx.status)}">${tx.status}</span></td>
            <td style="color:${tx.currentFine > 0 ? 'var(--danger)' : 'var(--text-muted)'}">
              ${tx.currentFine > 0 ? '₹' + tx.currentFine.toFixed(2) : '—'}
            </td>
            <td>
              ${tx.status === 'ACTIVE' ? `
                <button class="btn btn-sm" onclick="quickReturn(${tx.id})" style="font-size:11px;padding:4px 10px">Return</button>
              ` : ''}
            </td>
          </tr>`).join('')}
        </tbody>
      </table>
    </div>`;
}

async function quickReturn(txId) {
  const res = await apiFetch('/api/return', { method:'POST', body: JSON.stringify({ transactionId: txId }) });
  if (res.success) {
    const fineMsg = res.fine > 0 ? ` Fine charged: ₹${res.fine.toFixed(2)}` : '';
    showToast('Book returned!' + fineMsg, 'success');
    loadTransactions();
    loadDashboard();
  } else {
    showToast(res.error || 'Failed to return', 'error');
  }
}

// ════════════════════════════════════════════════════════════
//  Borrow / Return
// ════════════════════════════════════════════════════════════
async function handleBorrow(e) {
  e.preventDefault();
  const memberId = parseInt(document.getElementById('borrow-member-id').value);
  const bookId   = parseInt(document.getElementById('borrow-book-id').value);
  const res = await apiFetch('/api/borrow', { method:'POST', body: JSON.stringify({ memberId, bookId }) });
  const el  = document.getElementById('borrow-result');
  el.classList.remove('hidden', 'success', 'error');
  if (res.id) {
    el.textContent = `✅ Borrowed! Transaction ID: ${res.id}. Due: ${formatTimestamp(res.dueDate)}`;
    el.classList.add('success');
    showToast('Book borrowed successfully!', 'success');
  } else {
    el.textContent = '❌ ' + (res.error || 'Failed to borrow');
    el.classList.add('error');
  }
}

async function handleReturn(e) {
  e.preventDefault();
  const txId = parseInt(document.getElementById('return-tx-id').value);
  const res  = await apiFetch('/api/return', { method:'POST', body: JSON.stringify({ transactionId: txId }) });
  const el   = document.getElementById('return-result');
  el.classList.remove('hidden', 'success', 'error');
  if (res.success) {
    const fineMsg = res.fine > 0 ? ` Fine charged: ₹${res.fine.toFixed(2)}` : ' No fine.';
    el.textContent = '✅ Book returned!' + fineMsg;
    el.classList.add('success');
    showToast('Book returned!', 'success');
  } else {
    el.textContent = '❌ ' + (res.error || 'Failed');
    el.classList.add('error');
  }
}

// ════════════════════════════════════════════════════════════
//  Global Search
// ════════════════════════════════════════════════════════════
let searchTimer;
async function handleGlobalSearch(q) {
  clearTimeout(searchTimer);
  if (!q.trim()) { loadBooks(); return; }
  searchTimer = setTimeout(async () => {
    const results = await apiFetch('/api/books?q=' + encodeURIComponent(q));
    showPage('books');
    loadBooks(results);
  }, 300);
}

// ════════════════════════════════════════════════════════════
//  Utilities
// ════════════════════════════════════════════════════════════
function esc(s) {
  return String(s || '').replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');
}

function formatTimestamp(ts) {
  if (!ts || ts === '0') return '—';
  const t = parseInt(ts);
  if (isNaN(t) || t === 0) return '—';
  return new Date(t * 1000).toLocaleDateString('en-IN', { day:'2-digit', month:'short', year:'numeric' });
}

function statusBadge(s) {
  switch (s) {
    case 'ACTIVE':   return 'badge-green';
    case 'RETURNED': return 'badge-gray';
    case 'OVERDUE':  return 'badge-red';
    case 'LOST':     return 'badge-yellow';
    default:         return 'badge-gray';
  }
}

// ════════════════════════════════════════════════════════════
//  Init
// ════════════════════════════════════════════════════════════
loadDashboard();
