# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of the ``fiction`` entry point: one-shot commands, scripts, exit codes, and the console script."""

from __future__ import annotations

import json
import subprocess  # ruff: ignore[suspicious-subprocess-import] -- the console script is exercised as a process on purpose
import sys
from pathlib import Path
from unittest.mock import Mock

import pytest
from prompt_toolkit import PromptSession
from prompt_toolkit.application import create_app_session
from prompt_toolkit.completion import CompleteEvent
from prompt_toolkit.document import Document
from prompt_toolkit.history import FileHistory
from prompt_toolkit.input import DummyInput
from prompt_toolkit.output import DummyOutput

from mnt.pyfiction import __version__
from mnt.pyfiction.cli import Session, main
from mnt.pyfiction.cli import app as cli_app


def test_commands_succeed() -> None:
    assert main(["-c", "version; version"]) == 0


def test_failing_command_returns_one() -> None:
    assert main(["-c", "version; frobnicate"]) == 1


def test_script_file(tmp_path: Path) -> None:
    script = tmp_path / "run.fs"
    script.write_text("version\n# a comment\n\nversion\n", encoding="utf-8")
    assert main(["-f", str(script)]) == 0


def test_missing_script_returns_two(tmp_path: Path) -> None:
    assert main(["-f", str(tmp_path / "missing.fs")]) == 2


def test_unreadable_script_returns_two(tmp_path: Path, capsys: pytest.CaptureFixture[str]) -> None:
    """A path that exists but cannot be read as a file reports the reason and exits with 2."""
    assert main(["-f", str(tmp_path)]) == 2
    assert "cannot read script" in capsys.readouterr().err


def test_log_is_written_on_failure(tmp_path: Path) -> None:
    log = tmp_path / "log.json"
    assert main(["-l", str(log), "-c", "version; ortho"]) == 1
    entries = json.loads(log.read_text(encoding="utf-8"))
    assert [entry["status"] for entry in entries] == ["ok", "error"]


def test_version_flag(capsys: pytest.CaptureFixture[str]) -> None:
    with pytest.raises(SystemExit) as exit_info:
        main(["--version"])
    assert exit_info.value.code == 0
    assert __version__ in capsys.readouterr().out


def test_module_runs_as_a_process() -> None:
    result = subprocess.run(
        [sys.executable, "-m", "mnt.pyfiction.cli", "-c", "version"], check=False, capture_output=True, text=True
    )
    assert result.returncode == 0, result.stderr
    assert __version__ in result.stdout


def test_console_script_is_installed() -> None:
    scripts = Path(sys.executable).parent
    script = scripts / ("fiction.exe" if sys.platform == "win32" else "fiction")
    if not script.exists():
        pytest.skip("the environment has no console script; the package is not installed")
    result = subprocess.run([str(script), "-c", "version"], check=False, capture_output=True, text=True)  # ruff: ignore[subprocess-without-shell-equals-true]
    assert result.returncode == 0, result.stderr
    assert __version__ in result.stdout


def test_interactive_interrupt_continues_and_eof_closes_log(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    monkeypatch.setattr(cli_app, "HISTORY_FILE", tmp_path / "history")
    monkeypatch.setattr(PromptSession, "prompt", Mock(side_effect=[KeyboardInterrupt, "version", EOFError]))
    log = tmp_path / "interactive.json"
    with create_app_session(input=DummyInput(), output=DummyOutput()):
        assert main(["--log", str(log)]) == 0
    entries = json.loads(log.read_text(encoding="utf-8"))
    assert len(entries) == 1
    assert entries[0]["status"] == "ok"
    assert entries[0]["result"]["version"] == __version__


@pytest.mark.parametrize(("text", "expected"), [("", "read"), ("version; rea", "read"), ("read --ty", "--type")])
def test_command_and_option_completion(text: str, expected: str) -> None:
    completions = cli_app.CommandCompleter().get_completions(Document(text), CompleteEvent(completion_requested=True))
    assert expected in {completion.text for completion in completions}


def test_file_completion(tmp_path: Path) -> None:
    (tmp_path / "circuit.v").write_text("", encoding="utf-8")
    text = f"read {tmp_path.as_posix()}/circ"
    completions = cli_app.CommandCompleter().get_completions(Document(text), CompleteEvent(completion_requested=True))
    assert "uit.v" in {completion.text for completion in completions}


@pytest.mark.parametrize(
    ("text", "expected"),
    [("read --type ", "tec"), ("read --topology hex", "hexagonal"), ("gold -e ", "high_effort")],
)
def test_choice_completion(text: str, expected: str) -> None:
    """An option that only accepts certain values offers them instead of falling through to paths."""
    completions = cli_app.CommandCompleter().get_completions(Document(text), CompleteEvent(completion_requested=True))
    assert expected in {completion.text for completion in completions}


def test_quit_stops_the_rest_of_a_command_string() -> None:
    """`quit` ends the run without failing, and the commands after it are not attempted."""
    assert main(["-c", "version; quit; frobnicate"]) == 0


def test_quit_stops_the_rest_of_a_script(tmp_path: Path) -> None:
    """`quit` in a script ends it, and the lines below it are not attempted."""
    script = tmp_path / "script.fs"
    script.write_text("version\nquit\nfrobnicate\n", encoding="utf-8")
    assert main(["-f", str(script)]) == 0


def test_interactive_continues_after_commands(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    """-i runs the shell once -c is done, as the C++ shell did."""
    monkeypatch.setattr(cli_app, "HISTORY_FILE", tmp_path / "history")
    monkeypatch.setattr(PromptSession, "prompt", Mock(side_effect=["version", EOFError]))
    log = tmp_path / "interactive.json"
    with create_app_session(input=DummyInput(), output=DummyOutput()):
        assert main(["-i", "--log", str(log), "-c", "version"]) == 0
    entries = json.loads(log.read_text(encoding="utf-8"))
    assert [entry["command"] for entry in entries] == ["version", "version"]


def test_interactive_is_not_entered_after_quit(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    """`quit` behind -c means it, even with -i."""
    monkeypatch.setattr(cli_app, "HISTORY_FILE", tmp_path / "history")
    prompt = Mock(side_effect=[EOFError])
    monkeypatch.setattr(PromptSession, "prompt", prompt)
    with create_app_session(input=DummyInput(), output=DummyOutput()):
        assert main(["-i", "-c", "version; quit"]) == 0
    assert prompt.call_count == 0


def test_quiet_keeps_errors_but_drops_informational_output(capsys: pytest.CaptureFixture[str]) -> None:
    """--quiet is for scripted runs: the result lines go, the errors stay."""
    assert main(["-q", "-c", "version"]) == 0
    assert not capsys.readouterr().out
    assert main(["-q", "-c", "frobnicate"]) == 1
    assert "unknown command" in capsys.readouterr().err


def test_history_write_failure_is_reported_once_and_swallowed(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    """`prompt_toolkit` appends to the history file inside `prompt`, so a failing append would end
    the shell on the first command a user types. It costs the history alone instead."""

    def refuse(_self: object, _string: str) -> None:
        msg = "no space left on device"
        raise OSError(28, msg)

    monkeypatch.setattr(FileHistory, "store_string", refuse)
    reported: list[str] = []
    history = cli_app.ForgivingFileHistory(str(tmp_path / "history"), reported.append)
    history.store_string("version")
    history.store_string("ortho")
    assert len(reported) == 1
    assert "cannot write the command history" in reported[0]


def test_an_unusable_history_file_still_starts_the_shell(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    """A home the history cannot even be opened in leaves the shell without one, not without a prompt."""
    monkeypatch.setattr(cli_app, "HISTORY_FILE", tmp_path / "history")
    monkeypatch.setattr(cli_app, "ForgivingFileHistory", Mock(side_effect=OSError(13, "permission denied")))
    monkeypatch.setattr(PromptSession, "prompt", Mock(side_effect=["version", EOFError]))
    session = Session()
    with create_app_session(input=DummyInput(), output=DummyOutput()):
        cli_app.repl(session)
    session.close()
    assert [entry["command"] for entry in session.log] == ["version"]
