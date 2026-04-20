initSidebar('transactions');

let allTxs = [];
let currentFilter = 'all';

function setFilter(f) {
  currentFilter = f;
  ['all','active','overdue'].forEach(x => {
    document.getElementById('filter-' + x).classList.toggle('btn-primary', x === f);
  });
  applyFilter();
}

function handleFilter(q) {
  applyFilter(q);
}

function applyFilter(q = '') {
  let txs = allTxs;
  if (currentFilter === 'active')  txs = txs.filter(t => t.status === 'ACTIVE');
  if (currentFilter === 'overdue') txs = txs.filter(t => t.isOverdue);
  if (q) txs = txs.filter(t =>
    String(t.id).includes(q) ||
    String(t.bookId).includes(q) ||
    String(t.memberId).includes(q) ||
    (t.status || '').toLowerCase().includes(q.toLowerCase())
  );
  renderTxs(txs);
}

function renderTxs(txs) {
  const el = document.getElementById('transactions-container');
  if (!txs || txs.length === 0) {
    el.innerHTML = '<div class="empty-state"><div class="empty-icon">📋</div><p>No transactions found.</p></div>';
    return;
  }
  el.innerHTML = `
    <div class="table-wrap">
      <table>
        <thead><tr>
          <th>#</th><th>Book ID</th><th>Member ID</th><th>Borrowed</th>
          <th>Due</th><th>Returned</th><th>Status</th><th>Fine (₹)</th><th></th>
        </tr></thead>
        <tbody>${txs.map(tx => `
          <tr>
            <td style="color:var(--text-muted)">${tx.id}</td>
            <td>${tx.bookId}</td>
            <td>${tx.memberId}</td>
            <td>${formatDate(tx.borrowDate)}</td>
            <td style="color:${tx.isOverdue ? 'var(--danger)' : 'inherit'}">${formatDate(tx.dueDate)}</td>
            <td>${tx.returnDate && tx.returnDate !== '0' ? formatDate(tx.returnDate) : '—'}</td>
            <td><span class="badge ${statusBadge(tx.status)}">${tx.status}</span></td>
            <td style="color:${tx.currentFine > 0 ? 'var(--danger)' : 'var(--text-muted)'}">
              ${tx.currentFine > 0 ? '₹' + tx.currentFine.toFixed(2) : '—'}
            </td>
            <td>
              ${tx.status === 'ACTIVE' ? `
                <button class="btn btn-sm" onclick="quickReturn(${tx.id})" style="font-size:11px">Return</button>
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
    const fine = res.fine > 0 ? ` Fine: ₹${res.fine.toFixed(2)}` : '';
    showToast('Book returned!' + fine, 'success');
    loadTransactions();
  } else {
    showToast(res.error || 'Failed to return', 'error');
  }
}

async function loadTransactions() {
  try {
    allTxs = await apiFetch('/api/transactions');
    applyFilter();
    document.getElementById('api-status').innerHTML = '<span class="status-dot"></span><span class="api-text">API Connected</span>';
  } catch {
    document.getElementById('api-status').innerHTML = '<span style="color:var(--danger)">⚠ API Offline</span>';
    document.getElementById('transactions-container').innerHTML = '<div class="empty-state"><div class="empty-icon">⚠️</div><p>Could not load transactions. Is the C++ server running?</p></div>';
  }
}

setFilter('all');
loadTransactions();
