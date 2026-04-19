from __future__ import annotations

import csv
import tempfile
from abc import ABC, abstractmethod
from collections.abc import Callable, Mapping
from dataclasses import dataclass
from pathlib import Path

CANONICAL_BAR_COLUMNS = (
    "ts_event",
    "instrument_id",
    "open",
    "high",
    "low",
    "close",
    "volume",
    "turnover",
)


@dataclass
class PreparedReplayData:
    csv_path: Path
    temporary: bool = False

    def cleanup(self) -> None:
        if self.temporary and self.csv_path.exists():
            self.csv_path.unlink()


class ReplayPreprocessor(ABC):
    @abstractmethod
    def prepare(self, source_path: str | Path) -> PreparedReplayData:
        """Convert arbitrary source data into the canonical replay CSV."""


class IdentityReplayPreprocessor(ReplayPreprocessor):
    def prepare(self, source_path: str | Path) -> PreparedReplayData:
        path = Path(source_path)
        if not path.exists():
            raise FileNotFoundError(f"Replay source does not exist: {path}")

        with path.open("r", encoding="utf-8", newline="") as handle:
            reader = csv.reader(handle)
            header = next(reader, None)

        if header != list(CANONICAL_BAR_COLUMNS):
            raise ValueError(
                "Replay CSV header does not match canonical format: "
                f"expected {list(CANONICAL_BAR_COLUMNS)}, got {header}"
            )

        return PreparedReplayData(csv_path=path, temporary=False)


class BaseCsvReplayPreprocessor(ReplayPreprocessor, ABC):
    def __init__(self, *, encoding: str = "utf-8", delimiter: str = ",") -> None:
        self._encoding = encoding
        self._delimiter = delimiter

    def prepare(self, source_path: str | Path) -> PreparedReplayData:
        source = Path(source_path)
        if not source.exists():
            raise FileNotFoundError(f"Replay source does not exist: {source}")

        fd, temp_path = tempfile.mkstemp(prefix="dhquant_replay_", suffix=".csv")
        Path(temp_path).unlink(missing_ok=True)
        prepared = PreparedReplayData(csv_path=Path(temp_path), temporary=True)

        row_count = 0
        with source.open("r", encoding=self._encoding, newline="") as src:
            reader = csv.DictReader(src, delimiter=self._delimiter)
            if reader.fieldnames is None:
                prepared.cleanup()
                raise ValueError(f"Replay source has no header: {source}")

            with prepared.csv_path.open("w", encoding="utf-8", newline="") as dst:
                writer = csv.DictWriter(dst, fieldnames=list(CANONICAL_BAR_COLUMNS))
                writer.writeheader()

                for raw_row in reader:
                    canonical_row = self.transform_row(raw_row)
                    if canonical_row is None:
                        continue
                    normalized_row = self._normalize_row(canonical_row)
                    writer.writerow(normalized_row)
                    row_count += 1

        if row_count == 0:
            prepared.cleanup()
            raise ValueError(f"Replay source produced no valid rows after preprocessing: {source}")

        return prepared

    @abstractmethod
    def transform_row(self, raw_row: Mapping[str, str]) -> Mapping[str, object] | None:
        """Return a canonical replay row or None to skip the row."""

    def _normalize_row(self, row: Mapping[str, object]) -> dict[str, object]:
        normalized: dict[str, object] = {}
        for column in CANONICAL_BAR_COLUMNS:
            if column not in row:
                raise ValueError(f"Missing canonical replay field: {column}")
            normalized[column] = row[column]
        return normalized


class MappedBarReplayPreprocessor(BaseCsvReplayPreprocessor):
    def __init__(
        self,
        field_mapping: Mapping[str, str],
        *,
        timestamp_unit: str = "ns",
        instrument_transform: Callable[[str], str] | None = None,
        row_filter: Callable[[Mapping[str, str]], bool] | None = None,
        defaults: Mapping[str, object] | None = None,
        encoding: str = "utf-8",
        delimiter: str = ",",
    ) -> None:
        super().__init__(encoding=encoding, delimiter=delimiter)
        self._field_mapping = dict(field_mapping)
        self._timestamp_unit = timestamp_unit
        self._instrument_transform = instrument_transform
        self._row_filter = row_filter
        self._defaults = dict(defaults or {})

    def transform_row(self, raw_row: Mapping[str, str]) -> Mapping[str, object] | None:
        if self._row_filter is not None and not self._row_filter(raw_row):
            return None

        canonical: dict[str, object] = {}
        for canonical_field in CANONICAL_BAR_COLUMNS:
            source_field = self._field_mapping.get(canonical_field, canonical_field)
            raw_value = raw_row.get(source_field)
            if (raw_value is None or raw_value == "") and canonical_field in self._defaults:
                canonical[canonical_field] = self._defaults[canonical_field]
                continue
            if raw_value is None or raw_value == "":
                raise ValueError(
                    f"Missing source field '{source_field}' for canonical field '{canonical_field}'"
                )
            canonical[canonical_field] = raw_value

        canonical["ts_event"] = self._normalize_timestamp(canonical["ts_event"])
        instrument_id = str(canonical["instrument_id"])
        if self._instrument_transform is not None:
            instrument_id = self._instrument_transform(instrument_id)
        canonical["instrument_id"] = instrument_id
        canonical["open"] = float(canonical["open"])
        canonical["high"] = float(canonical["high"])
        canonical["low"] = float(canonical["low"])
        canonical["close"] = float(canonical["close"])
        canonical["volume"] = int(float(canonical["volume"]))
        canonical["turnover"] = float(canonical["turnover"])
        return canonical

    def _normalize_timestamp(self, raw_value: object) -> int:
        value = int(float(str(raw_value)))
        if self._timestamp_unit == "s":
            return value * 1_000_000_000
        if self._timestamp_unit == "ms":
            return value * 1_000_000
        if self._timestamp_unit == "us":
            return value * 1_000
        if self._timestamp_unit == "ns":
            return value
        raise ValueError(f"Unsupported timestamp unit: {self._timestamp_unit}")


class ReplayDataLoader:
    def __init__(self, engine: object) -> None:
        self._engine = engine

    def load(
        self,
        source_path: str | Path,
        *,
        preprocessor: ReplayPreprocessor | None = None,
        keep_prepared_file: bool = False,
    ) -> Path:
        processor = preprocessor or IdentityReplayPreprocessor()
        prepared = processor.prepare(source_path)
        try:
            self._engine.load_replay(str(prepared.csv_path))
            return prepared.csv_path
        finally:
            if prepared.temporary and not keep_prepared_file:
                prepared.cleanup()
