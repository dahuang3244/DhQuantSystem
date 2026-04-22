from __future__ import annotations

import os
import sys
from pathlib import Path

from PySide6.QtGui import QGuiApplication
from PySide6.QtQml import QQmlApplicationEngine

from dhquant.gui.backend import TradingDemoBackend


def main() -> int:
    # Force Basic style to avoid native style customization warnings
    os.environ["QT_QUICK_CONTROLS_STYLE"] = "Basic"

    app = QGuiApplication(sys.argv)
    app.setApplicationName("DhQuant GUI Prototype")

    engine = QQmlApplicationEngine()
    # Add components directory to import path
    qml_dir = Path(__file__).resolve().parent / "qml_modern"
    engine.addImportPath(str(qml_dir))

    backend = TradingDemoBackend()
    engine.rootContext().setContextProperty("backend", backend)

    qml_path = qml_dir / "Main.qml"
    engine.load(str(qml_path))
    if not engine.rootObjects():
        return 1
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
