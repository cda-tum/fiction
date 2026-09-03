const assert = require("node:assert/strict");
const { readFileSync } = require("node:fs");
const { join } = require("node:path");
const { test } = require("node:test");
const { runInNewContext } = require("node:vm");

const script = readFileSync(join(__dirname, "../_static/tabs.js"), "utf8");

function fixture(hash = "#python-api") {
  const events = {};
  const input = { checked: false };
  const peer = { checked: false };
  const panel = {
    previousElementSibling: {
      htmlFor: "python-tab",
      click() {
        input.checked = true;
        peer.checked = true;
      },
    },
    parentElement: { closest: () => null },
  };
  let scrolls = 0;
  const target = {
    closest: () => panel,
    scrollIntoView() {
      scrolls += 1;
    },
  };
  const location = { hash, href: `https://example.test/api.html${hash}` };
  const listen = (name, callback) => {
    events[name] = callback;
  };
  runInNewContext(script, {
    window: { location, addEventListener: listen },
    document: {
      addEventListener: listen,
      getElementById: (id) =>
        ({ "python-api": target, "python-tab": input })[id],
    },
  });
  return { events, input, peer, location, scrolls: () => scrolls };
}

test("incoming anchors activate the tab and its synchronization handler", () => {
  const page = fixture();
  page.events.load();
  assert.equal(page.input.checked, true);
  assert.equal(page.peer.checked, true);
  assert.equal(page.scrolls(), 1);
});

test("hash changes and repeated links reopen a manually hidden target", () => {
  const page = fixture();
  page.events.hashchange();
  page.input.checked = false;
  page.events.click({
    target: { closest: () => ({ href: page.location.href }) },
  });
  assert.equal(page.input.checked, true);
  assert.equal(page.scrolls(), 2);
});

test("visible targets retain normal browser scrolling", () => {
  const page = fixture();
  page.input.checked = true;
  page.events.load();
  assert.equal(page.scrolls(), 0);
});

test("missing targets and malformed escapes leave the page usable", () => {
  for (const hash of ["", "#unknown", "#%E0%A4%A"]) {
    const page = fixture(hash);
    assert.doesNotThrow(() => page.events.load());
    assert.equal(page.input.checked, false);
  }
});
