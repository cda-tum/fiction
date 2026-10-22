import assert from "node:assert/strict";
import { readFileSync } from "node:fs";
import test from "node:test";
import { runInNewContext } from "node:vm";

const script = readFileSync(
  new URL("../../docs/_static/tabs.js", import.meta.url),
  "utf8",
);

for (const checked of [true, false]) {
  test(`fragment target scrolls when its tab starts ${checked ? "active" : "hidden"}`, () => {
    const input = { checked };
    let scrolled = false;
    const panel = {
      previousElementSibling: {
        htmlFor: "tab",
        click() {
          input.checked = true;
        },
      },
      parentElement: { closest: () => null },
    };
    const target = {
      closest: () => panel,
      scrollIntoView() {
        scrolled = true;
      },
    };
    const listeners = {};
    runInNewContext(script, {
      window: {
        location: { hash: "#target" },
        addEventListener: (event, callback) => (listeners[event] = callback),
      },
      document: {
        getElementById: (id) => (id === "target" ? target : input),
        addEventListener() {},
      },
    });
    listeners.load();
    assert.equal(input.checked, true);
    assert.equal(scrolled, true);
  });
}
