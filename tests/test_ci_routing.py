import unittest
import json
import subprocess
import sys
import tempfile
from pathlib import Path

from scripts.ci_routing import classify, norm, parse_status_lines


class RoutingTests(unittest.TestCase):
    SCRIPT = Path(__file__).resolve().parents[1] / "scripts/ci_routing.py"

    def _git(self, repo: Path, *args: str) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            ["git", *args], cwd=repo, text=True, capture_output=True, check=True
        )

    def test_documentation_has_zero_builds(self):
        for path in ("README.md", ".github/ISSUE_TEMPLATE/bug_report.md", "config/README.md", "releases/README.md", "examples/esp-idf/01_ESP_IDF_ST7789/README.md", "examples/arduino/01_HelloWorld/README.md", "examples/arduino/libraries/X/README.md"):
            result = classify([path])
            self.assertEqual((result["idf_route"], result["arduino_route"]), ("none", "none"))
            self.assertTrue(result["docs_only"])

    def test_multiple_direct_projects_are_selected(self):
        result = classify(["examples/esp-idf/01_ESP_IDF_ST7789/main/main.c", "examples/esp-idf/02_ESP_IDF_ST7789_LVGL/main/main.c", "examples/arduino/01_HelloWorld/01_HelloWorld.ino", "examples/arduino/02_Drawing_board/02_Drawing_board.ino"])
        self.assertEqual(result["idf_selectors"], ["examples/esp-idf/01_ESP_IDF_ST7789", "examples/esp-idf/02_ESP_IDF_ST7789_LVGL"])
        self.assertEqual(result["arduino_selectors"], ["examples/arduino/01_HelloWorld", "examples/arduino/02_Drawing_board"])

    def test_cmake_lists_is_a_build_input_not_documentation(self):
        result = classify(["examples/esp-idf/01_ESP_IDF_ST7789/CMakeLists.txt"])
        self.assertEqual(result["idf_route"], "selected")
        self.assertEqual(result["idf_selectors"], ["examples/esp-idf/01_ESP_IDF_ST7789"])
        self.assertFalse(result["docs_only"])

    def test_shared_and_dotfile_global_inputs_select_all(self):
        self.assertEqual(classify(["examples/arduino/libraries/X/src/x.cpp"])["arduino_route"], "all")
        self.assertEqual(classify(["examples/esp-idf/components/x.c"])["idf_route"], "all")
        self.assertEqual(classify(["scripts/discover_examples.py"])["idf_route"], "all")
        self.assertEqual(norm("./.github/workflows/examples.yml"), ".github/workflows/examples.yml")
        self.assertEqual(classify([".github/workflows/examples.yml"])["arduino_route"], "all")

    def test_firmware_file_kinds_never_select_examples(self):
        for path, docs_only, release in (("firmware/README.md", True, False), ("firmware/main.c", False, False), ("firmware/sdkconfig.defaults", False, False), ("firmware/image.bin", False, True), ("firmware/package.zip", False, True)):
            result = classify([path])
            self.assertEqual((result["idf_route"], result["arduino_route"]), ("none", "none"))
            self.assertEqual(result["docs_only"], docs_only)
            self.assertEqual(bool(result["release_paths"]), release)

    def test_rename_and_deleted_direct_source(self):
        result = classify(parse_status_lines(["R100\texamples/esp-idf/01_ESP_IDF_ST7789/main/old.c\texamples/esp-idf/02_ESP_IDF_ST7789_LVGL/main/new.c"]))
        self.assertEqual(result["idf_selectors"], ["examples/esp-idf/01_ESP_IDF_ST7789", "examples/esp-idf/02_ESP_IDF_ST7789_LVGL"])
        result = classify(parse_status_lines(["D\texamples/arduino/01_HelloWorld/01_HelloWorld.ino"]))
        self.assertEqual(result["arduino_selectors"], ["examples/arduino/01_HelloWorld"])

    def test_unknown_path_selects_all(self):
        result = classify(["odd.dat"])
        self.assertEqual((result["idf_route"], result["arduino_route"]), ("all", "all"))
        self.assertEqual(result["unknown_paths"], ["odd.dat"])

    def test_empty_is_rejected(self):
        with self.assertRaises(ValueError):
            classify([])

    def test_cli_workflow_style_invocation(self):
        completed = subprocess.run([sys.executable, "scripts/ci_routing.py", "--changed-file", "examples/arduino/01_HelloWorld/01_HelloWorld.ino"], text=True, capture_output=True, check=True)
        self.assertEqual(json.loads(completed.stdout)["arduino_route"], "selected")

    def test_cli_base_and_github_output_match_workflow_contract(self):
        with tempfile.TemporaryDirectory() as tmp:
            repo = Path(tmp)
            self._git(repo, "init", "-q")
            self._git(repo, "config", "user.name", "CI Routing Test")
            self._git(repo, "config", "user.email", "ci-routing@example.invalid")
            (repo / "README.md").write_text("baseline\n", encoding="utf-8")
            self._git(repo, "add", "README.md")
            self._git(repo, "commit", "-q", "-m", "baseline")
            base = self._git(repo, "rev-parse", "HEAD").stdout.strip()

            source = repo / "examples/arduino/one/one.ino"
            source.parent.mkdir(parents=True)
            source.write_text("void setup() {}\nvoid loop() {}\n", encoding="utf-8")
            self._git(repo, "add", source.relative_to(repo).as_posix())
            self._git(repo, "commit", "-q", "-m", "add sketch")

            github_output = repo / "github-output.txt"
            completed = subprocess.run(
                [
                    sys.executable,
                    str(self.SCRIPT),
                    "--base",
                    base,
                    "--github-output",
                    str(github_output),
                ],
                cwd=repo,
                text=True,
                capture_output=True,
                check=True,
            )
            result = json.loads(completed.stdout)
            self.assertEqual(result["arduino_route"], "selected")
            self.assertEqual(result["arduino_selectors"], ["examples/arduino/one"])
            outputs = dict(
                line.split("=", 1)
                for line in github_output.read_text(encoding="utf-8").splitlines()
            )
            self.assertEqual(outputs["arduino_route"], "selected")
            self.assertEqual(json.loads(outputs["arduino_selectors"]), ["examples/arduino/one"])

            current = self._git(repo, "rev-parse", "HEAD").stdout.strip()
            empty = subprocess.run(
                [sys.executable, str(self.SCRIPT), "--base", current],
                cwd=repo,
                text=True,
                capture_output=True,
                check=False,
            )
            self.assertEqual(empty.returncode, 2)
            self.assertIn("changed-file input is empty or unavailable", empty.stderr)

    def test_cli_manual_selector_writes_github_output(self):
        with tempfile.TemporaryDirectory() as tmp:
            output = Path(tmp) / "github-output.txt"
            completed = subprocess.run(
                [
                    sys.executable,
                    str(self.SCRIPT),
                    "--manual-selector",
                    "all",
                    "--github-output",
                    str(output),
                ],
                text=True,
                capture_output=True,
                check=True,
            )
            self.assertEqual(json.loads(completed.stdout)["idf_route"], "all")
            values = dict(
                line.split("=", 1)
                for line in output.read_text(encoding="utf-8").splitlines()
            )
            self.assertEqual(values["idf_route"], "all")
            self.assertEqual(values["arduino_route"], "all")

    def test_multiple_selector_shell_equivalent(self):
        selectors = '["01_ESP_IDF_ST7789","02_ESP_IDF_ST7789_LVGL"]'
        emitted = subprocess.run([sys.executable, "-c", "import json,sys; print(*json.loads(sys.argv[1]), sep='\\n')", selectors], text=True, capture_output=True, check=True).stdout.splitlines()
        completed = subprocess.run([sys.executable, "scripts/discover_examples.py", "--surface", "esp-idf", *sum((["--selector", item] for item in emitted), [])], text=True, capture_output=True, check=True)
        self.assertEqual(len(json.loads(completed.stdout)["include"]), 4)
