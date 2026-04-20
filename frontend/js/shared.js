// ── API base URL ─────────────────────────────────────────────
// When hosted on Vercel (frontend-only), point API calls to the
// Render-deployed C++ backend. Locally, use the same origin.
const RENDER_BACKEND = 'https://librarysys.onrender.com'; // ← update after deploying to Render
const API = (window.location.hostname.includes('vercel.app') || window.location.hostname.includes('vercel.com'))
  ? RENDER_BACKEND
  : '';


async function apiFetch(path, options = {}) {
  const res = await fetch(API + path, {
    headers: { 'Content-Type': 'application/json' },
    ...options
  });
  return res.json();
}

// ── Toast ─────────────────────────────────────────────────────
function showToast(msg, type = 'info') {
  let t = document.getElementById('toast');
  if (!t) { t = document.createElement('div'); t.id = 'toast'; t.className = 'toast hidden'; document.body.appendChild(t); }
  const icon = type === 'success' ? '✅' : type === 'error' ? '❌' : 'ℹ️';
  t.innerHTML = `<span>${icon}</span><span>${msg}</span>`;
  t.style.borderColor = type === 'success' ? 'rgba(16,185,129,0.4)' : type === 'error' ? 'rgba(239,68,68,0.4)' : '#2a2d3e';
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

// ── Sidebar toggle (desktop collapse / mobile drawer) ────────
function initSidebar(activeNav) {
  const sidebar  = document.getElementById('sidebar');
  const topbar   = document.getElementById('topbar');
  const overlay  = document.getElementById('sidebar-overlay');
  const hamburger = document.getElementById('hamburger');

  // Mark active nav link
  if (activeNav) {
    const link = document.getElementById('nav-' + activeNav);
    if (link) link.classList.add('active');
  }

  // Restore desktop collapse state
  const collapsed = localStorage.getItem('sidebar-collapsed') === 'true';
  if (collapsed && window.innerWidth > 768) {
    document.body.classList.add('sidebar-collapsed');
    sidebar.classList.add('collapsed');
  }

  hamburger.addEventListener('click', () => {
    if (window.innerWidth <= 768) {
      // Mobile: drawer
      sidebar.classList.toggle('mobile-open');
      overlay.classList.toggle('show');
    } else {
      // Desktop: collapse/expand
      sidebar.classList.toggle('collapsed');
      document.body.classList.toggle('sidebar-collapsed');
      localStorage.setItem('sidebar-collapsed', sidebar.classList.contains('collapsed'));
    }
  });

  overlay.addEventListener('click', () => {
    sidebar.classList.remove('mobile-open');
    overlay.classList.remove('show');
  });

  // Close mobile drawer on nav click
  document.querySelectorAll('.nav-link').forEach(link => {
    link.addEventListener('click', () => {
      if (window.innerWidth <= 768) {
        sidebar.classList.remove('mobile-open');
        overlay.classList.remove('show');
      }
    });
  });
}

// ── Utilities ─────────────────────────────────────────────────
function esc(s) {
  return String(s || '').replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');
}

function formatDate(ts) {
  if (!ts || ts === '0') return '—';
  const t = parseInt(ts);
  if (isNaN(t) || t === 0) return '—';
  return new Date(t * 1000).toLocaleDateString('en-IN', { day:'2-digit', month:'short', year:'numeric' });
}

function statusBadge(s) {
  const map = { ACTIVE:'badge-green', RETURNED:'badge-gray', OVERDUE:'badge-red', LOST:'badge-yellow' };
  return map[s] || 'badge-gray';
}
