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
      closest: (selector) => (selector === ".sd-tab-content" ? panel : null),
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

for (const outerChecked of [false, true]) {
  test(`hash changes reveal an encoded nested target with the outer tab ${outerChecked ? "active" : "hidden"}`, () => {
    const inputs = {
      inner: { checked: false },
      outer: { checked: outerChecked },
    };
    let scrolled = false;
    let outerClicks = 0;
    const outer = {
      previousElementSibling: {
        htmlFor: "outer",
        click() {
          outerClicks++;
          inputs.outer.checked = !inputs.outer.checked;
        },
      },
      parentElement: { closest: () => null },
    };
    const inner = {
      previousElementSibling: {
        htmlFor: "inner",
        click() {
          inputs.inner.checked = true;
        },
      },
      parentElement: {
        closest: (selector) => (selector === ".sd-tab-content" ? outer : null),
      },
    };
    const target = {
      closest: (selector) => (selector === ".sd-tab-content" ? inner : null),
      scrollIntoView() {
        assert.equal(inputs.inner.checked, true);
        assert.equal(inputs.outer.checked, true);
        scrolled = true;
      },
    };
    const listeners = {};
    runInNewContext(script, {
      window: {
        location: { hash: "#topic%20name" },
        addEventListener: (event, callback) => (listeners[event] = callback),
      },
      document: {
        getElementById: (id) => (id === "topic name" ? target : inputs[id]),
        addEventListener() {},
      },
    });
    listeners.hashchange();
    assert.equal(inputs.inner.checked, true);
    assert.equal(inputs.outer.checked, true);
    assert.equal(outerClicks, outerChecked ? 0 : 1);
    assert.equal(scrolled, true);
  });
}

test("malformed fragments do not look up or scroll a target", () => {
  const listeners = {};
  let lookedUp = false;
  runInNewContext(script, {
    window: {
      location: { hash: "#%E0%A4%A" },
      addEventListener: (event, callback) => (listeners[event] = callback),
    },
    document: {
      getElementById() {
        lookedUp = true;
      },
      addEventListener() {},
    },
  });
  assert.doesNotThrow(() => listeners.hashchange());
  assert.equal(lookedUp, false);
});
