The weekly canary in `.github/workflows/mockturtle-canary.yml` builds and tests _fiction_
against the current head of the `mnt` branch of
[`marcelwa/mockturtle`](https://github.com/marcelwa/mockturtle) instead of the commit
`cmake/Dependencies.cmake` pins. That run failed.

- Failing run: {{ RUN_URL }}
- Branch under test: <https://github.com/marcelwa/mockturtle/tree/mnt>

Three things produce this, in falling order of likelihood:

1. The branch moved in a way _fiction_ does not follow.
2. The branch itself is broken.
3. Neither: the run hit an infrastructure flake, such as the apt mirror hang the workflow
   bounds with a timeout. Read the run before acting on this issue.

For the first two, Renovate's next bump of the pin carries the same failure, so fixing it
here is what unblocks that pull request.

Close this issue once the canary is green again. While it stays open, further failures
rewrite this body instead of filing another issue.
