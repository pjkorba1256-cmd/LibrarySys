// ── API base URL ─────────────────────────────────────────────
// If the C++ server is reachable, use it. Otherwise, use localStorage.
const API = '';

// ── LocalStorage DB (Vercel / offline fallback) ───────────────
const DB = {
  get(key) {
    try { return JSON.parse(localStorage.getItem('libsys_' + key)) || []; }
    catch { return []; }
  },
  set(key, val) { localStorage.setItem('libsys_' + key, JSON.stringify(val)); },
  nextId(key) {
    const items = DB.get(key);
    return items.length ? Math.max(...items.map(i => i.id)) + 1 : 1;
  },
  seed() {
    if (DB.get('books').length === 0) {
      DB.set('books', [
        { id:1, title:'The Great Gatsby', author:'F. Scott Fitzgerald', isbn:'978-0743273565', category:'Fiction', totalCopies:3, availableCopies:2, status:'AVAILABLE' },
        { id:2, title:'To Kill a Mockingbird', author:'Harper Lee', isbn:'978-0061935466', category:'Fiction', totalCopies:2, availableCopies:2, status:'AVAILABLE' },
        { id:3, title:'Clean Code', author:'Robert C. Martin', isbn:'978-0132350884', category:'Technology', totalCopies:4, availableCopies:3, status:'AVAILABLE' },
        { id:4, title:'The Pragmatic Programmer', author:'David Thomas', isbn:'978-0135957059', category:'Technology', totalCopies:2, availableCopies:1, status:'AVAILABLE' },
        { id:5, title:'Atomic Habits', author:'James Clear', isbn:'978-0735211292', category:'Self-Help', totalCopies:5, availableCopies:5, status:'AVAILABLE' }
      ]);
    }
    if (DB.get('members').length === 0) {
      DB.set('members', [
        { id:1, name:'Arjun Sharma', email:'arjun@example.com', activeBorrowCount:1, finesOwed:0, canBorrow:true },
        { id:2, name:'Priya Patel', email:'priya@example.com', activeBorrowCount:0, finesOwed:0, canBorrow:true },
        { id:3, name:'Rohan Mehta', email:'rohan@example.com', activeBorrowCount:0, finesOwed:5.50, canBorrow:false }
      ]);
    }
    if (DB.get('transactions').length === 0) {
      const now = Math.floor(Date.now()/1000);
      DB.set('transactions', [
        { id:1, bookId:1, memberId:1, borrowDate: String(now - 86400*5), dueDate: String(now + 86400*9), returnDate:'0', status:'ACTIVE', currentFine:0, isOverdue:false }
      ]);
    }
  }
};

// ── Mode detection ────────────────────────────────────────────
let USE_LOCAL = false;
async function detectMode() {
  try {
    const r = await fetch('/api/books', { signal: AbortSignal.timeout(2000) });
    if (!r.ok) throw new Error();
    USE_LOCAL = false;
  } catch {
    USE_LOCAL = true;
    DB.seed();
  }
}

// ── Unified fetch (real API or localStorage) ──────────────────
async function apiFetch(path, options = {}) {
  if (!USE_LOCAL) {
    const res = await fetch(API + path, {
      headers: { 'Content-Type': 'application/json' },
      ...options
    });
    return res.json();
  }
  return localHandler(path, options);
}

// ── LocalStorage handler (mimics C++ REST API) ────────────────
function localHandler(path, options = {}) {
  const method = options.method || 'GET';
  const body   = options.body ? JSON.parse(options.body) : null;

  // ── /api/books ───────────────────────────────────────────────
  if (path.startsWith('/api/books')) {
    const books = DB.get('books');
    const q = new URLSearchParams(path.split('?')[1] || '').get('q');
    const idMatch = path.match(/\/api\/books\/(\d+)/);

    if (method === 'GET' && q)
      return books.filter(b => b.title.toLowerCase().includes(q.toLowerCase()) ||
                               b.author.toLowerCase().includes(q.toLowerCase()));
    if (method === 'GET' && idMatch)
      return books.find(b => b.id === +idMatch[1]) || { error:'Not found' };
    if (method === 'GET')  return books;

    if (method === 'POST') {
      const nb = { id:DB.nextId('books'), title:body.title, author:body.author,
                   isbn:body.isbn||'', category:body.category||'General',
                   totalCopies:body.copies||1, availableCopies:body.copies||1, status:'AVAILABLE' };
      DB.set('books', [...books, nb]);
      return nb;
    }
    if (method === 'DELETE' && idMatch) {
      DB.set('books', books.filter(b => b.id !== +idMatch[1]));
      return { success:true };
    }
  }

  // ── /api/members ─────────────────────────────────────────────
  if (path.startsWith('/api/members')) {
    const members = DB.get('members');
    const idMatch = path.match(/\/api\/members\/(\d+)/);

    if (method === 'GET' && idMatch)
      return members.find(m => m.id === +idMatch[1]) || { error:'Not found' };
    if (method === 'GET')  return members;

    if (method === 'POST') {
      if (members.some(m => m.email === body.email))
        return { error:'Email already registered' };
      const nm = { id:DB.nextId('members'), name:body.name, email:body.email,
                   activeBorrowCount:0, finesOwed:0, canBorrow:true };
      DB.set('members', [...members, nm]);
      return nm;
    }
    if (method === 'DELETE' && idMatch) {
      DB.set('members', members.filter(m => m.id !== +idMatch[1]));
      return { success:true };
    }
  }

  // ── /api/transactions ────────────────────────────────────────
  if (path.startsWith('/api/transactions')) {
    const txs = DB.get('transactions');
    const activeOnly = path.includes('active=true');
    const now = Math.floor(Date.now()/1000);
    const list = txs.map(t => ({ ...t, isOverdue: t.status==='ACTIVE' && +t.dueDate < now }));
    return activeOnly ? list.filter(t => t.status==='ACTIVE') : list;
  }

  // ── /api/borrow ──────────────────────────────────────────────
  if (path === '/api/borrow' && method === 'POST') {
    const books   = DB.get('books');
    const members = DB.get('members');
    const txs     = DB.get('transactions');
    const book    = books.find(b => b.id === body.bookId);
    const member  = members.find(m => m.id === body.memberId);
    if (!book)   return { error:'Book not found' };
    if (!member) return { error:'Member not found' };
    if (book.availableCopies < 1) return { error:'No copies available' };
    if (!member.canBorrow) return { error:'Member has outstanding fines' };

    const now = Math.floor(Date.now()/1000);
    const dueDate = now + 86400 * 14; // 14 days
    const tx = { id:DB.nextId('transactions'), bookId:body.bookId, memberId:body.memberId,
                 borrowDate:String(now), dueDate:String(dueDate), returnDate:'0',
                 status:'ACTIVE', currentFine:0, isOverdue:false };
    DB.set('transactions', [...txs, tx]);
    book.availableCopies -= 1;
    book.status = book.availableCopies === 0 ? 'BORROWED' : 'AVAILABLE';
    DB.set('books', books);
    member.activeBorrowCount += 1;
    DB.set('members', members);
    return tx;
  }

  // ── /api/return ──────────────────────────────────────────────
  if (path === '/api/return' && method === 'POST') {
    const txs     = DB.get('transactions');
    const books   = DB.get('books');
    const members = DB.get('members');
    const tx      = txs.find(t => t.id === body.transactionId);
    if (!tx) return { error:'Transaction not found' };
    if (tx.status !== 'ACTIVE') return { error:'Already returned' };

    const now = Math.floor(Date.now()/1000);
    const overdueDays = Math.max(0, Math.floor((now - +tx.dueDate) / 86400));
    const fine = overdueDays * 2; // ₹2 per day
    tx.returnDate = String(now);
    tx.status = 'RETURNED';
    tx.currentFine = fine;
    DB.set('transactions', txs);

    const book = books.find(b => b.id === tx.bookId);
    if (book) { book.availableCopies += 1; book.status = 'AVAILABLE'; DB.set('books', books); }
    const member = members.find(m => m.id === tx.memberId);
    if (member) { member.activeBorrowCount = Math.max(0, member.activeBorrowCount-1);
                  member.finesOwed += fine; member.canBorrow = member.finesOwed === 0;
                  DB.set('members', members); }
    return { success:true, fine };
  }

  return { error:'Unknown route' };
}

// ── Sidebar init ──────────────────────────────────────────────
function initSidebar(activeNav) {
  const sidebar  = document.getElementById('sidebar');
  const overlay  = document.getElementById('sidebar-overlay');
  const hamburger = document.getElementById('hamburger');

  if (activeNav) {
    const link = document.getElementById('nav-' + activeNav);
    if (link) link.classList.add('active');
  }

  const collapsed = localStorage.getItem('sidebar-collapsed') === 'true';
  if (collapsed && window.innerWidth > 768) {
    document.body.classList.add('sidebar-collapsed');
    sidebar.classList.add('collapsed');
  }

  hamburger.addEventListener('click', () => {
    if (window.innerWidth <= 768) {
      sidebar.classList.toggle('mobile-open');
      overlay.classList.toggle('show');
    } else {
      sidebar.classList.toggle('collapsed');
      document.body.classList.toggle('sidebar-collapsed');
      localStorage.setItem('sidebar-collapsed', sidebar.classList.contains('collapsed'));
    }
  });

  overlay.addEventListener('click', () => {
    sidebar.classList.remove('mobile-open');
    overlay.classList.remove('show');
  });

  document.querySelectorAll('.nav-link').forEach(link => {
    link.addEventListener('click', () => {
      if (window.innerWidth <= 768) {
        sidebar.classList.remove('mobile-open');
        overlay.classList.remove('show');
      }
    });
  });

  // Detect mode then update status indicator
  detectMode().then(() => {
    const el = document.getElementById('api-status');
    if (el) {
      el.innerHTML = USE_LOCAL
        ? '<span class="status-dot" style="background:var(--warning);box-shadow:0 0 6px var(--warning)"></span><span class="api-text">Demo Mode</span>'
        : '<span class="status-dot"></span><span class="api-text">API Connected</span>';
    }
  });
}

// ── Toast ─────────────────────────────────────────────────────
function showToast(msg, type = 'info') {
  let t = document.getElementById('toast');
  if (!t) { t = document.createElement('div'); t.id='toast'; t.className='toast hidden'; document.body.appendChild(t); }
  const icon = type==='success'?'✅':type==='error'?'❌':'ℹ️';
  t.innerHTML = `<span>${icon}</span><span>${msg}</span>`;
  t.style.borderColor = type==='success'?'rgba(16,185,129,0.4)':type==='error'?'rgba(239,68,68,0.4)':'#2a2d3e';
  t.classList.remove('hidden');
  clearTimeout(t._timer);
  t._timer = setTimeout(() => t.classList.add('hidden'), 3500);
}

// ── Modal ─────────────────────────────────────────────────────
function openModal(id)  { document.getElementById(id).classList.remove('hidden'); }
function closeModal(id) { document.getElementById(id).classList.add('hidden'); }
document.addEventListener('click', e => {
  if (e.target.classList.contains('modal-overlay')) closeModal(e.target.id);
});

// ── Utilities ─────────────────────────────────────────────────
function esc(s) {
  return String(s||'').replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');
}
function formatDate(ts) {
  if (!ts || ts==='0') return '—';
  const t = parseInt(ts);
  if (isNaN(t) || t===0) return '—';
  return new Date(t*1000).toLocaleDateString('en-IN',{day:'2-digit',month:'short',year:'numeric'});
}
function statusBadge(s) {
  return {ACTIVE:'badge-green',RETURNED:'badge-gray',OVERDUE:'badge-red',LOST:'badge-yellow'}[s]||'badge-gray';
}
