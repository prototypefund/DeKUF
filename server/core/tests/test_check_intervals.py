from core.models.check_intervals import check_intervals
from django.test import TestCase


class IntervalTestCase(TestCase):
    def test_intervals_work_when_correct(self):
        test_intervals = [
            "(-inf, 0)",
            "[0, 5)",
            "[5, 10)",
            "[10, 10.5)",
            "[11, inf)",
            "[10000, 500000)",
            "[0, 0.1)",
        ]
        check_intervals(test_intervals)

    def test_intervals_fail_when_incorrect(self):
        test_intervals = [
            "[-inf, 0)",
            "[0as, 5)",
            "(inf, 10)",
            "[10, inf]",
            "[10, -inf)",
            "[11- 13)",
            "[10000, 500a000)",
            "[0, 0,1)",
        ]
        for value in test_intervals:
            array_value = [value]
            with self.assertRaises(ValueError):
                check_intervals(array_value)
