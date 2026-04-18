from __future__ import annotations

import os
from pathlib import Path
from typing import Any

import yaml

PROJECT_ROOT = Path(__file__).resolve().parents[2]
CONFIG_DIR = PROJECT_ROOT / "config"


def resolve_config_path(env: str | None = None) -> Path:
    selected_env = env or os.getenv("DH_ENV", "dev")
    env_path = CONFIG_DIR / f"{selected_env}.yaml"
    if env_path.exists():
        return env_path
    return CONFIG_DIR / "base.yaml"


def _deep_merge(base: dict[str, Any], override: dict[str, Any]) -> dict[str, Any]:
    merged = dict(base)
    for key, value in override.items():
        if isinstance(value, dict) and isinstance(merged.get(key), dict):
            merged[key] = _deep_merge(merged[key], value)
        else:
            merged[key] = value
    return merged


def load_config(env: str | None = None) -> dict[str, Any]:
    base_path = CONFIG_DIR / "base.yaml"
    base_config = (
        yaml.safe_load(base_path.read_text(encoding="utf-8")) if base_path.exists() else {}
    )

    env_path = resolve_config_path(env)
    if env_path == base_path or not env_path.exists():
        return base_config

    env_config = yaml.safe_load(env_path.read_text(encoding="utf-8")) or {}
    return _deep_merge(base_config, env_config)
