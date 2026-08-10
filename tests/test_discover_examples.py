import tempfile
import unittest
from pathlib import Path

from scripts.discover_examples import build_matrix


class DiscoverTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        root = Path(self.temp.name)
        for rel in ("examples/esp-idf/one/CMakeLists.txt", "examples/esp-idf/two/CMakeLists.txt", "examples/arduino/one/one.ino", "examples/arduino/two/two.ino", "examples/arduino/libraries/upstream/demo/demo.ino"):
            path = root / rel; path.parent.mkdir(parents=True, exist_ok=True); path.write_text("")
        self.root = root

    def tearDown(self): self.temp.cleanup()

    def args(self, surface, selectors):
        return type("Args", (), {"repo": str(self.root), "surface": surface, "selector": selectors, "idf_versions": "v5.5.5,v6.0.2", "arduino_core": "3.3.11", "fqbn": "fqbn"})()

    def test_all_name_path_multiple_and_bundled_exclusion(self):
        self.assertEqual(len(build_matrix(self.args("esp-idf", ["all"]))["include"]), 4)
        self.assertEqual(len(build_matrix(self.args("esp-idf", ["one"]))["include"]), 2)
        self.assertEqual(len(build_matrix(self.args("arduino", ["examples/arduino/one", "two"]))["include"]), 2)
