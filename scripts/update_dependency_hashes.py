#!/usr/bin/env python3
"""Recompute the ``URL_HASH`` values in ``cmake/Dependencies.cmake``.

Renovate bumps the ``*_VERSION`` and ``ALICE_REV`` variables in that file but
cannot compute an archive hash, so a dependency bump arrives with a stale
``*_SHA256`` and fails the CMake configure step with ``Hash mismatch``. This
script downloads each declared archive and writes the hash that matches it.

Run it with no arguments to update the file in place, or with ``--check`` to
verify the hashes and exit non-zero when any of them is stale.
"""

from __future__ import annotations

import argparse
import hashlib
import re
import sys
import urllib.error
import urllib.request
from pathlib import Path
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from http.client import HTTPMessage
    from typing import IO

REPOSITORY_ROOT = Path(__file__).resolve().parent.parent
DEPENDENCIES_CMAKE = REPOSITORY_ROOT / "cmake" / "Dependencies.cmake"

# Every dependency fetched by URL, as (CMake variable prefix, display name).
DEPENDENCIES = [
    ("JSON", "nlohmann_json"),
    ("CATCH2", "Catch2"),
    ("PARALLEL_HASHMAP", "parallel-hashmap"),
    ("TINYXML2", "tinyxml2"),
    ("ALICE", "alice"),
]

DOWNLOAD_CHUNK_SIZE = 1 << 16


def read_variable(text: str, name: str) -> str:
    """Read the first value of a ``set()`` call.

    Handles the plain form and the ``CACHE STRING "..."`` form, and tolerates
    the line breaks that ``cmake-format`` introduces.

    Args:
        text: Contents of ``cmake/Dependencies.cmake``.
        name: The CMake variable to look up.

    Returns:
        The variable's value, with ``${...}`` references left unexpanded.

    Raises:
        SystemExit: When the variable is absent, which means this script and
            ``cmake/Dependencies.cmake`` have drifted apart.
    """
    match = re.search(rf"set\({name}\s+([^\s)]+)", text)
    if match is None:
        msg = f"{name} not found in {DEPENDENCIES_CMAKE.name}"
        raise SystemExit(msg)
    return match.group(1)


def expand(value: str, text: str) -> str:
    """Substitute ``${VAR}`` references using the variables in the file.

    Args:
        value: A value that may contain ``${VAR}`` references.
        text: Contents of ``cmake/Dependencies.cmake``.

    Returns:
        The value with every reference replaced.
    """
    while (match := re.search(r"\$\{(\w+)\}", value)) is not None:
        value = value.replace(match.group(0), read_variable(text, match.group(1)))
    return value


class HTTPSOnlyRedirectHandler(urllib.request.HTTPRedirectHandler):
    """Redirect handler that refuses to leave ``https``.

    ``urllib`` follows a redirect from ``https`` to ``http`` without complaint. The hash this
    script writes into ``Dependencies.cmake`` is what later builds verify their download against,
    so letting an unauthenticated response decide it would defeat the point of recording it.
    """

    def redirect_request(
        self,
        req: urllib.request.Request,
        fp: IO[bytes],
        code: int,
        msg: str,
        headers: HTTPMessage,
        newurl: str,
    ) -> urllib.request.Request | None:
        """Reject a redirect that leaves ``https``, and otherwise defer to the default.

        Args:
            req: The request that was redirected.
            fp: The response body of the redirect.
            code: The HTTP status code of the redirect.
            msg: The HTTP status message of the redirect.
            headers: The headers of the redirect response.
            newurl: The URL the redirect points at.

        Returns:
            The request to follow, or ``None`` where the default declines to redirect.

        Raises:
            urllib.error.HTTPError: If ``newurl`` is not ``https``.
        """
        if not newurl.startswith("https://"):
            reason = f"refusing to follow a redirect to a non-https URL: {newurl}"
            raise urllib.error.HTTPError(newurl, code, reason, headers, fp)
        return super().redirect_request(req, fp, code, msg, headers, newurl)


HTTPS_ONLY_OPENER = urllib.request.build_opener(HTTPSOnlyRedirectHandler)


def sha256_of(url: str) -> str:
    """Stream an archive and digest it without holding it in memory.

    Args:
        url: The archive to download.

    Returns:
        The archive's SHA-256 digest as a hexadecimal string.

    Raises:
        ValueError: If the URL is not ``https``.
    """
    if not url.startswith("https://"):
        msg = f"refusing to download over a non-https URL: {url}"
        raise ValueError(msg)

    digest = hashlib.sha256()
    # the opener rejects a redirect that would leave https, which `urlopen` would follow
    with HTTPS_ONLY_OPENER.open(url) as response:
        while chunk := response.read(DOWNLOAD_CHUNK_SIZE):
            digest.update(chunk)
    return digest.hexdigest()


def main() -> int:
    """Compare every recorded hash against its archive.

    Returns:
        ``0`` when the hashes match or were updated, ``1`` otherwise.
    """
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--check",
        action="store_true",
        help="verify the hashes without writing; exit non-zero when one is stale",
    )
    arguments = parser.parse_args()

    text = DEPENDENCIES_CMAKE.read_text()
    updated = text
    stale: list[str] = []

    for prefix, name in DEPENDENCIES:
        url = expand(read_variable(text, f"{prefix}_URL"), text)
        if not url.startswith("https://"):
            print(f"{name}: refusing to fetch over a non-HTTPS URL: {url}", file=sys.stderr)
            return 1

        recorded = read_variable(text, f"{prefix}_SHA256")
        actual = sha256_of(url)

        if recorded == actual:
            print(f"{name}: up to date")
            continue

        stale.append(name)
        print(f"{name}: recorded {recorded}")
        print(f"{' ' * len(name)}  actual   {actual}")
        updated = re.sub(
            rf"(set\({prefix}_SHA256\s+){re.escape(recorded)}",
            rf"\g<1>{actual}",
            updated,
        )

    if not stale:
        print("\nAll hashes match.")
        return 0

    if arguments.check:
        print(f"\nStale hashes: {', '.join(stale)}", file=sys.stderr)
        print("Run scripts/update_dependency_hashes.py to fix them.", file=sys.stderr)
        return 1

    DEPENDENCIES_CMAKE.write_text(updated)
    print(f"\nUpdated {len(stale)} hash(es) in {DEPENDENCIES_CMAKE.relative_to(REPOSITORY_ROOT)}.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
