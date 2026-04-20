#pragma once

// ============================================================
//  FineCalculator — stateless utility for fine policies.
//  Centralizes fine-related constants so they can be changed
//  in one place. Transaction delegates its math here.
// ============================================================
class FineCalculator {
public:
    static constexpr int    LOAN_PERIOD_DAYS = 14;
    static constexpr double FINE_PER_DAY     = 5.0;
    static constexpr double MAX_FINE         = 500.0;

    // Returns fine in ₹ given overdue days
    static double calculate(int overdueDays);

    // Returns overdue days from timestamps (0 if not overdue)
    static int overdueDays(long long borrowTimestamp, long long currentTimestamp);
};
