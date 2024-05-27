import re
from typing import List


def check_intervals(intervals: List[str]):
    finite_inf_exp = re.compile(r"^\[(-?\d+(\.\d+)?),\s*inf\)$")
    inf_finite_exp = re.compile(r"^\((-inf),\s*(-?\d+(\.\d+)?|inf)(\]|\))$")
    finite_exp = re.compile(
        r"^(\[|\()(-?\d+(\.\d+)?),\s*(-?\d+(\.\d+)?)(\]|\))$"
    )

    for interval in intervals:
        if not (
            finite_inf_exp.match(interval)
            or inf_finite_exp.match(interval)
            or finite_exp.match(interval)
        ):
            raise ValueError(f"Incorrect Interval: {interval}")
