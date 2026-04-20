initSidebar('dashboard');

async function handleSearch(q) {
  if (q.trim()) window.location.href = 'books.html?q=' + encodeURIComponent(q);
}

async function loadDashboard() {
  try {
    const [books, members, txs] = await Promise.all([
      apiFetch('/api/books'),
      apiFetch('/api/members'),
      apiFetch('/api/transactions?active=true')
    ]);

    document.getElementById('stat-books').textContent     = books.length;
    document.getElementById('stat-available').textContent = books.reduce((s,b) => s + (b.availableCopies||0), 0);
    document.getElementById('stat-members').textContent   = members.length;
    document.getElementById('stat-borrows').textContent   = Array.isArray(txs) ? txs.length : 0;
    document.getElementById('api-status').innerHTML = '<span class="status-dot"></span><span class="api-text">API Connected</span>';

    // Recent books
    const rb = document.getElementById('recent-books');
    rb.innerHTML = books.length === 0
      ? '<div class="empty-state"><p>No books added yet.</p></div>'
      : books.slice(0, 5).map(b => `
          <div class="item-row">
            <div>
              <div class="item-label">${esc(b.title)}</div>
              <div class="item-sub">${esc(b.author)} · ${esc(b.category)}</div>
            </div>
            <span class="badge ${b.availableCopies > 0 ? 'badge-green' : 'badge-red'}">
              ${b.availableCopies > 0 ? b.availableCopies + ' avail' : 'Borrowed'}
            </span>
          </div>`).join('');

    // Active borrows
    const ab = document.getElementById('active-borrows');
    if (!Array.isArray(txs) || txs.length === 0) {
      ab.innerHTML = '<div class="empty-state"><p>No active borrows.</p></div>';
    } else {
      ab.innerHTML = txs.slice(0, 5).map(tx => `
        <div class="item-row">
          <div>
            <div class="item-label">Book #${tx.bookId} → Member #${tx.memberId}</div>
            <div class="item-sub">Due: ${formatDate(tx.dueDate)}</div>
          </div>
          <span class="badge ${tx.isOverdue ? 'badge-red' : 'badge-green'}">${tx.isOverdue ? 'Overdue' : 'Active'}</span>
        </div>`).join('');
    }
  } catch {
    document.getElementById('api-status').innerHTML = '<span style="color:var(--danger)">⚠ API Offline</span>';
  }
}

loadDashboard();
