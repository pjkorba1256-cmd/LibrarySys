initSidebar('members');

let allMembers = [];

async function handleSearch(q) {
  const filtered = allMembers.filter(m =>
    m.name.toLowerCase().includes(q.toLowerCase()) ||
    m.email.toLowerCase().includes(q.toLowerCase())
  );
  renderMembers(filtered);
}

function renderMembers(members) {
  const el = document.getElementById('members-container');
  if (!members || members.length === 0) {
    el.innerHTML = '<div class="empty-state"><div class="empty-icon">👤</div><p>No members found.</p></div>';
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

async function loadMembers() {
  await detectMode();
  allMembers = await apiFetch('/api/members');
  renderMembers(allMembers);
}

loadMembers();
