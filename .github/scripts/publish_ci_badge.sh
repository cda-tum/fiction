#!/usr/bin/env bash
set -euo pipefail

if [[ $# != 5 || ! ${4} =~ ^[1-9][0-9]*$ || ! ${5} =~ ^[1-9][0-9]*$ ]]; then
  echo "Usage: $0 REMOTE BADGE RESULT RUN_NUMBER RUN_ATTEMPT" >&2
  exit 2
fi

remote=$1 badge=$2 result=$3 run_number=$4 run_attempt=$5
case "$badge" in
  ubuntu) label=Ubuntu ;;
  macos) label=macOS ;;
  windows) label=Windows ;;
  docker) label=Docker ;;
  codeql) label=CodeQL ;;
  *) echo "Unknown CI badge: $badge" >&2; exit 2 ;;
esac
case "$result" in
  success) message=passing color=brightgreen ;;
  failure) message=failing color=red ;;
  cancelled | skipped) exit 0 ;;
  *) echo "Unknown CI summary result: $result" >&2; exit 2 ;;
esac

ref=refs/heads/ci-badges
file="$badge.json"
scratch=$(mktemp -d)
trap 'rm -rf -- "$scratch"' EXIT
cd "$scratch"
git init --quiet

# Only the index is needed. Never check out files from the publication branch.
for ((retry = 0; retry < 8; retry++)); do
  published_ref=$(git ls-remote --refs -- "$remote" "$ref")
  parent=()
  if [[ -n "$published_ref" ]]; then
    git fetch --quiet --depth=1 -- "$remote" "$ref"
    base=$(git rev-parse FETCH_HEAD)
    parent=(-p "$base")
    git read-tree "$base"
    if [[ -n $(git ls-tree --name-only "$base" -- "$file") ]]; then
      already_published=$(
        git show "$base:$file" |
          jq -r --argjson run "$run_number" --argjson attempt "$run_attempt" \
            '[.runNumber, .runAttempt] >= [$run, $attempt]'
      )
      [[ "$already_published" != true ]] || exit 0
    fi
  else
    git read-tree --empty
  fi

  blob=$(
    jq -n --arg label "$label" --arg message "$message" --arg color "$color" \
      --argjson run "$run_number" --argjson attempt "$run_attempt" \
      '{schemaVersion: 1, $label, $message, $color, runNumber: $run, runAttempt: $attempt}' |
      git hash-object -w --stdin
  )
  git update-index --add --cacheinfo "100644,$blob,$file"
  tree=$(git write-tree)
  commit=$(
    git -c user.name='github-actions[bot]' -c user.email='41898282+github-actions[bot]@users.noreply.github.com' \
      commit-tree "$tree" "${parent[@]}" -m "👷 Publish $label CI result ($run_number.$run_attempt)"
  )

  # A competing publisher can move the branch between our fetch and push.
  # A normal push rejects that update, so retry from the new branch tip.
  if git push --quiet -- "$remote" "$commit:$ref"; then
    exit 0
  fi
  sleep 1
done

echo "Could not publish $label CI result to $ref" >&2
exit 1
