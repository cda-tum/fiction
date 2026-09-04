/** Reveal fragment targets inside synchronized language tabs. */
function revealTabTarget() {
  let id;
  try {
    id = decodeURIComponent(window.location.hash.slice(1));
  } catch {
    return;
  }
  const target = document.getElementById(id);
  if (!target) return;

  let panel = target.closest(".sd-tab-content");
  let changed = false;
  while (panel) {
    const label = panel.previousElementSibling;
    const input = document.getElementById(label.htmlFor);
    if (!input.checked) {
      label.click();
      changed = true;
    }
    panel = panel.parentElement.closest(".sd-tab-content");
  }
  if (changed) target.scrollIntoView();
}

window.addEventListener("load", revealTabTarget);
window.addEventListener("hashchange", revealTabTarget);
document.addEventListener("click", (event) => {
  const link = event.target.closest("a[href]");
  if (link && link.href === window.location.href) revealTabTarget();
});
