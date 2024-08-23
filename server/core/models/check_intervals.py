import re
from typing import List, Optional, Union


def check_intervals(
    intervals: List[str],
    min_value: Optional[Union[int, float]] = None,
    max_value: Optional[Union[int, float]] = None,
):
    # Check if min_value <= max_value when both are provided
    if (
        min_value is not None
        and max_value is not None
        and min_value > max_value
    ):
        raise ValueError(
            f"min_value {min_value} is greater than max_value {max_value}"
        )

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

        finite_inf_match = finite_inf_exp.match(interval)
        inf_finite_match = inf_finite_exp.match(interval)
        finite_match = finite_exp.match(interval)

        if finite_inf_match:
            lower = float(finite_inf_match.group(1))
            upper = float("inf")
        elif inf_finite_match:
            lower = float("-inf")
            upper = float(inf_finite_match.group(2))
        elif finite_match:
            lower = float(finite_match.group(2))
            upper = float(finite_match.group(4))
        else:
            raise ValueError(f"Incorrect Interval: {interval}")

        if min_value is not None:
            if lower < min_value:
                raise ValueError(
                    f"Interval {interval} violates min_value {min_value}"
                )

        if max_value is not None:
            if upper > max_value:
                raise ValueError(
                    f"Interval {interval} violates max_value {max_value}"
                )
