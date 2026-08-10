import json
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

    def _write_pairs(self, repo: Path) -> None:
        for en, zh in PAIRS:
            for source, other in ((en, zh), (zh, en)):
                path = repo / source
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text(f"# title\n\n[language]({Path(other).name})\n", encoding="utf-8")

    def _write_homepage_contract(self, repo: Path) -> Path:
        self._write_pairs(repo)
        (repo / "assets").mkdir()
        (repo / "assets/hero.jpg").write_bytes(b"hero")
        (repo / "examples/esp-idf").mkdir(parents=True, exist_ok=True)
        (repo / "examples/arduino").mkdir(parents=True, exist_ok=True)
        (repo / "LICENSE").write_text("license", encoding="utf-8")
        h2 = "\n".join(f"## {icon} Section" for icon in ["✨", "🖥️", "📦"])
        for source, other in (("README.md", "README_ZH.md"), ("README_ZH.md", "README.md")):
            (repo / source).write_text(
                f'''<div align="center">
<h1>Board</h1>
<p><strong>Product subtitle</strong></p>
<p><a href="{other}">language</a>
<a href="https://www.waveshare.com/board.htm">product</a>
<a href="https://docs.waveshare.com/board">docs</a>
<a href="#quick-start">quick start</a>
<a href="https://github.com/org/repo/releases/latest">firmware</a>
<a href="examples/esp-idf/">idf</a>
<a href="examples/arduino/">arduino</a></p>
<p><a href="https://github.com/org/repo/actions/workflows/test.yml"><img src="https://example.test/badge.svg"></a>
<a href="https://github.com/org/repo/releases/latest"><img src="https://img.shields.io/github/v/release/org/repo"></a>
<a href="LICENSE"><img src="https://img.shields.io/github/license/org/repo"></a></p>
<img src="assets/hero.jpg" alt="Board hero">
</div>

---

{h2}
''',
                encoding="utf-8",
            )
        config = {
            "pair_exempt_patterns": [],
            "homepage_pairs": [{
                "english": "README.md", "chinese": "README_ZH.md", "profile": "single-product",
                "required_components": ["centered_header", "html_h1", "subtitle", "badges", "language_switch", "quick_links", "hero_image", "separator", "h2"],
                "required_quick_links": ["product", "documentation", "quick_start", "firmware", "esp_idf", "arduino"],
                "required_badges": ["build", "release", "license"],
                "required_h2_icons": ["✨", "🖥️", "📦"],
                "h3_emoji_allow_patterns": [],
            }],
        }
        path = repo / "audit.json"
        path.write_text(json.dumps(config), encoding="utf-8")
        return path

    def test_complete_homepage_contract_passes(self):
        with tempfile.TemporaryDirectory() as tmp:
            repo = Path(tmp)
            self.assertEqual(check(repo, self._write_homepage_contract(repo)), [])

    def test_missing_hero_or_product_quick_link_fails(self):
        with tempfile.TemporaryDirectory() as tmp:
            repo = Path(tmp)
            config = self._write_homepage_contract(repo)
            (repo / "assets/hero.jpg").unlink()
            self.assertTrue(any("hero_image" in error for error in check(repo, config)))
        with tempfile.TemporaryDirectory() as tmp:
            repo = Path(tmp)
            config = self._write_homepage_contract(repo)
            readme = repo / "README.md"
            readme.write_text(readme.read_text(encoding="utf-8").replace('<a href="https://www.waveshare.com/board.htm">product</a>\n', ""), encoding="utf-8")
            self.assertTrue(any("quick link product" in error for error in check(repo, config)))

    def test_malformed_or_unknown_config_and_missing_local_hero_fail_closed(self):
        with tempfile.TemporaryDirectory() as tmp:
            repo = Path(tmp)
            config = self._write_homepage_contract(repo)
            config.write_text("{", encoding="utf-8")
            self.assertTrue(any("invalid markdown audit config" in error for error in check(repo, config)))
        with tempfile.TemporaryDirectory() as tmp:
            repo = Path(tmp)
            config = self._write_homepage_contract(repo)
            policy = json.loads(config.read_text(encoding="utf-8"))
            policy["homepage_pairs"][0]["unknown"] = True
            config.write_text(json.dumps(policy), encoding="utf-8")
            self.assertTrue(any("invalid markdown audit config" in error for error in check(repo, config)))
        with tempfile.TemporaryDirectory() as tmp:
            repo = Path(tmp)
            config = self._write_homepage_contract(repo)
            readme = repo / "README.md"
            readme.write_text(readme.read_text(encoding="utf-8").replace("assets/hero.jpg", "assets/missing.jpg"), encoding="utf-8")
            self.assertTrue(any("hero_image" in error for error in check(repo, config)))
