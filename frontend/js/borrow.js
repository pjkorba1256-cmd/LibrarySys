initSidebar('borrow');

async function handleBorrow(e) {
  e.preventDefault();
  const memberId = parseInt(document.getElementById('borrow-member-id').value);
  const bookId   = parseInt(document.getElementById('borrow-book-id').value);
  const res = await apiFetch('/api/borrow', { method:'POST', body: JSON.stringify({ memberId, bookId }) });
  const el = document.getElementById('borrow-result');
  el.classList.remove('hidden', 'success', 'error');
  if (res.id) {
    el.textContent = `✅ Borrowed! Transaction ID: ${res.id}. Due: ${formatDate(res.dueDate)}`;
    el.classList.add('success');
    showToast('Book borrowed successfully!', 'success');
    loadActiveBorrows();
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
    const fine = res.fine > 0 ? ` Fine charged: ₹${res.fine.toFixed(2)}` : ' No fine.';
    el.textContent = '✅ Book returned!' + fine;
    el.classList.add('success');
    showToast('Book returned!', 'success');
    loadActiveBorrows();
  } else {
    el.textContent = '❌ ' + (res.error || 'Failed to return');
    el.classList.add('error');
  }
}

async function loadActiveBorrows() {
  await detectMode();
  const txs = await apiFetch('/api/transactions?active=true');
  const el  = document.getElementById('active-borrows-list');
  if (!Array.isArray(txs) || txs.length === 0) {
    el.innerHTML = '<div class="empty-state"><p>No active borrows right now.</p></div>';
    return;
  }
  el.innerHTML = `
    <div class="table-wrap">
      <table>
        <thead><tr><th>Tx #</th><th>Book ID</th><th>Member ID</th><th>Due Date</th><th>Status</th><th></th></tr></thead>
        <tbody>${txs.map(tx => `
          <tr>
            <td style="color:var(--text-muted)">${tx.id}</td>
            <td>${tx.bookId}</td>
            <td>${tx.memberId}</td>
            <td style="color:${tx.isOverdue ? 'var(--danger)' : 'inherit'}">${formatDate(tx.dueDate)}</td>
            <td><span class="badge ${tx.isOverdue ? 'badge-red' : 'badge-green'}">${tx.isOverdue ? 'Overdue' : 'Active'}</span></td>
            <td>
              <button class="btn btn-sm" onclick="prefillReturn(${tx.id})">Return</button>
            </td>
          </tr>`).join('')}
        </tbody>
      </table>
    </div>`;
}

function prefillReturn(txId) {
  document.getElementById('return-tx-id').value = txId;
  document.getElementById('return-tx-id').scrollIntoView({ behavior: 'smooth' });
}

loadActiveBorrows();
