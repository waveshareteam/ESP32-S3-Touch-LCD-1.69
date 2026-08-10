import tempfile
import unittest
from pathlib import Path

from scripts.check_first_party_markdown import PAIRS, check


class MarkdownTests(unittest.TestCase):
    def test_pair_and_link_failures_are_reported(self):
        with tempfile.TemporaryDirectory() as tmp:
            repo = Path(tmp)
            for en, zh in PAIRS:
                for source, other in ((en, zh), (zh, en)):
                    path = repo / source; path.parent.mkdir(parents=True, exist_ok=True)
                    path.write_text(f"# title\n\n[language]({Path(other).name})\n", encoding="utf-8")
            (repo / "README.md").write_text("# title\n\n[language](README_ZH.md)\n🌐 📚 📦 🧩 🔧\n## ✨ A\n", encoding="utf-8")
            (repo / "README_ZH.md").write_text("# title\n\n[language](README.md)\n🌐 📚 📦 🧩 🔧\n## ✨ A\n", encoding="utf-8")
            self.assertEqual(check(repo), [])
            (repo / "docs/ci.md").write_text("# CI\n[bad](missing.md)\n", encoding="utf-8")
            self.assertTrue(any("missing reciprocal" in item or "missing relative" in item for item in check(repo)))
