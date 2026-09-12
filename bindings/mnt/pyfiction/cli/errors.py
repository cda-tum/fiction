# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The exceptions the shell reports and recovers from; a leaf module every other one may import."""

from __future__ import annotations


class CommandError(Exception):
    """A failure the shell reports and recovers from: a usage error, an empty store, a wrong layout type."""

    stats: dict[str, object] | None = None
    """Available algorithm statistics when a search fails."""


class HelpRequested(SystemExit):
    """Raised by the command parser in place of ``sys.exit`` once ``-h`` has produced the help text.

    ``argparse`` ends a help request with ``sys.exit``. This carries the text to the session instead,
    which prints it and treats the command as done.
    """

    def __init__(self, text: str) -> None:
        """Store the help text.

        Args:
            text: The formatted help text.
        """
        super().__init__(0)
        self.text = text
