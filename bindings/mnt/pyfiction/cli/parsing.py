# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Argument parsing and quote-aware shell tokenization."""

from __future__ import annotations

import argparse
import math
import textwrap
from typing import TYPE_CHECKING, NoReturn

from .errors import CommandError, HelpRequested

if TYPE_CHECKING:
    from _typeshed import SupportsWrite


class DefaultsFormatter(argparse.RawDescriptionHelpFormatter):
    """A help formatter that states each option's default next to the option itself.

    ``argparse.ArgumentDefaultsHelpFormatter`` would also append ``(default: False)`` to every flag,
    which says nothing; only options that take a value and carry a real default get the suffix here.
    """

    # an override of argparse's method, which does not need the formatter either
    def _get_help_string(self, action: argparse.Action) -> str:  # ruff: ignore[no-self-use]
        """Return the option's help, with its default appended when it has a meaningful one.

        Args:
            action: The action being rendered.

        Returns:
            The help string shown for the option.
        """
        text = action.help or ""
        if (
            action.default is None
            or action.default is argparse.SUPPRESS
            or isinstance(action.default, bool)
            or "%(default)" in text
            or "(default:" in text
        ):
            return text
        return f"{text} (default: %(default)s)"


class Parser(argparse.ArgumentParser):
    """An argument parser that reports through exceptions instead of exiting the process.

    Attributes:
        actions: Arguments in declaration order, for help and completion.
    """

    def __init__(self, name: str, description: str, *, inputs: str, example: str, unavailable: str | None) -> None:
        """Create the parser of one command.

        Args:
            name: The command name, shown as the program name in usage lines.
            description: The command's docstring, shown by ``-h``.
            inputs: The command's input requirements.
            example: A shell invocation of the command.
            unavailable: The missing capability, or None when available.
        """
        summary, _, restrictions = description.partition("\n\n")
        self.restrictions = unavailable or restrictions or "No additional restrictions."
        self.example = example
        super().__init__(
            prog=name,
            description=f"{summary}\n\nInputs:\n  {inputs}",
            allow_abbrev=False,
            formatter_class=DefaultsFormatter,
        )

    @property
    def actions(self) -> list[argparse.Action]:
        """Argparse's actions, including grouped arguments, for completion.

        Returns:
            Arguments in declaration order.
        """
        return self._actions

    def validate(self, parsed: argparse.Namespace) -> None:
        """Validate numeric arguments before entering native code.

        Args:
            parsed: The parsed command arguments.
        """
        positive = {
            "inputs",
            "bitwidth",
            "threads",
            "upper_x",
            "upper_y",
            "upper_area",
            "fixed_size",
            "iterations",
            "expansions",
            "epsilon_r",
            "lambda_tf",
            "timeout",
            "max_temperature",
            "random_sampling",
            "flood_fill",
            "contour_tracing",
        }
        for name, value in vars(parsed).items():
            if isinstance(value, bool) or value is None:
                continue
            maximum = 2**64 - 1 if name == "seed" else 2**32 - 1
            if isinstance(value, int) and not 0 <= value <= maximum:
                self.error(f"{name.replace('_', '-')}: expected an integer from 0 to {maximum}")
            if isinstance(value, float) and not math.isfinite(value):
                self.error(f"{name.replace('_', '-')}: expected a finite number")
            if name in positive and value <= 0:
                self.error(
                    f"{name.replace('_', '-')}: must be at least 1"
                    if isinstance(value, int)
                    else f"{name.replace('_', '-')}: must be positive"
                )
            if name in {"width", "height", "hspace", "vspace", "alpha"} and value < 0:
                self.error(f"{name.replace('_', '-')}: cannot be negative")
            if name == "confidence" and not 0 < value <= 1:
                self.error("confidence must be in (0, 1]")

    def error(self, message: str) -> NoReturn:
        """Turn a usage error into a :class:`CommandError` that carries the usage line.

        Args:
            message: The error message ``argparse`` produced.

        Raises:
            CommandError: Always.
        """
        msg = f"{message}\n{self.format_usage().rstrip()}"
        raise CommandError(msg)

    # an override of argparse's method, which does not need the parser either
    def exit(self, status: int = 0, message: str | None = None) -> NoReturn:  # ruff: ignore[no-self-use]
        """Never exit the process.

        Args:
            status: The exit status ``argparse`` intended.
            message: The message ``argparse`` intended to print.

        Raises:
            CommandError: When the status signals an error.
            HelpRequested: Otherwise.
        """
        if status:
            raise CommandError(message or "invalid arguments")
        raise HelpRequested(message or "")

    def format_help(self) -> str:
        """Render options with their defaults, the restrictions, and a shell example.

        Returns:
            Detailed plain-text command help.
        """
        return (
            super().format_help()
            + "\nRestrictions:\n"
            + textwrap.indent(self.restrictions, "  ")
            + f"\n\nExample:\n  {self.example}\n"
        )

    def print_help(self, file: SupportsWrite[str] | None = None) -> None:
        """Hand the help text to the session instead of writing it to a stream.

        Args:
            file: Ignored; kept for the ``argparse`` signature.

        Raises:
            HelpRequested: Always, carrying the formatted help.
        """
        del file
        raise HelpRequested(self.format_help())


def tokenize(line: str, *, incomplete: bool = False) -> list[list[str]]:  # ruff: ignore[complex-structure]
    """Split commands, preserving quoted separators and literal Windows backslashes.

    Args:
        line: Commands separated by unquoted semicolons; unquoted # starts a comment.
        incomplete: Permit an open quote and retain the last empty word for completion.

    Returns:
        Commands containing decoded words.

    Raises:
        ValueError: An execution line contains an unclosed quote.
    """
    commands: list[list[str]] = []
    words: list[str] = []
    word = ""
    quote = ""
    started = False
    for char in line:
        if quote:
            if char == quote:
                quote = ""
            else:
                word += char
        elif char in "\"'":
            quote = char
            started = True
        elif char == "#":
            break
        elif char == ";" or char.isspace():
            if started:
                words.append(word)
                word, started = "", False
            if char == ";":
                if words:
                    commands.append(words)
                words = []
        else:
            word += char
            started = True
    if quote and not incomplete:
        msg = "unclosed quote"
        raise ValueError(msg)
    if started or incomplete:
        words.append(word)
    if words:
        commands.append(words)
    return commands
