import unittest
import json
import subprocess
import sys

from scripts.ci_routing import classify, norm, parse_status_lines


class RoutingTests(unittest.TestCase):
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

    def test_multiple_selector_shell_equivalent(self):
        selectors = '["01_ESP_IDF_ST7789","02_ESP_IDF_ST7789_LVGL"]'
        emitted = subprocess.run([sys.executable, "-c", "import json,sys; print(*json.loads(sys.argv[1]), sep='\\n')", selectors], text=True, capture_output=True, check=True).stdout.splitlines()
        completed = subprocess.run([sys.executable, "scripts/discover_examples.py", "--surface", "esp-idf", *sum((["--selector", item] for item in emitted), [])], text=True, capture_output=True, check=True)
        self.assertEqual(len(json.loads(completed.stdout)["include"]), 4)
