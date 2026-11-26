"""Build the single-file judge submission from maintained sources."""
from pathlib import Path
root = Path(__file__).resolve().parents[1]
header = (root / "src/include/int2048.h").read_text()
source = (root / "src/int2048.cpp").read_text()
(root / "code.cpp").write_text(header + "\n" + source.replace('#include "include/int2048.h"\n', "", 1))
