#!/usr/bin/env bash
set -euo pipefail

script="$(dirname -- "${BASH_SOURCE[0]}")/publish_ci_badge.sh"
sandbox=$(mktemp -d)
trap 'rm -rf -- "$sandbox"' EXIT
remote="$sandbox/badges.git"
ref=refs/heads/ci-badges
git init --quiet --bare "$remote"

publish() {
  bash "$script" "$remote" "$@"
}

assert_badge() {
  local name=$1 label=$2 status=$3 color=$4 run=$5 attempt=$6
  git --git-dir="$remote" show "$ref:$name.json" |
    jq -e --arg label "$label" --arg status "$status" --arg color "$color" \
      --argjson run "$run" --argjson attempt "$attempt" \
      '.schemaVersion == 1 and .label == $label and .message == $status
      and .color == $color and .runNumber == $run and .runAttempt == $attempt' >/dev/null
}

# First publish creates the branch and its first record. A failed check is red.
publish ubuntu success 10 1
assert_badge ubuntu Ubuntu passing brightgreen 10 1
publish docker failure 11 1
assert_badge docker Docker failing red 11 1
assert_badge ubuntu Ubuntu passing brightgreen 10 1

# An older run, a canceled rerun, and a duplicate cannot replace the saved result.
publish docker success 10 4
publish docker cancelled 11 2
publish docker success 11 1
assert_badge docker Docker failing red 11 1
publish docker success 11 2
assert_badge docker Docker passing brightgreen 11 2

# Publishing concurrently must preserve both changes, including on a fresh branch.
remote="$sandbox/concurrent.git"
git init --quiet --bare "$remote"
publish macos success 12 1 &
macos_pid=$!
publish windows failure 12 1 &
windows_pid=$!
wait "$macos_pid"
wait "$windows_pid"
assert_badge macos macOS passing brightgreen 12 1
assert_badge windows Windows failing red 12 1

# A run that finishes late must leave the newer result intact.
publish codeql failure 13 1 &
older_pid=$!
publish codeql success 14 1 &
newer_pid=$!
wait "$older_pid"
wait "$newer_pid"
assert_badge codeql CodeQL passing brightgreen 14 1

echo "CI badge publishing tests passed"
