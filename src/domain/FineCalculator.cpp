#include "FineCalculator.h"
#include <algorithm>

double FineCalculator::calculate(int overdueDays) {
    if (overdueDays <= 0) return 0.0;
    double fine = overdueDays * FINE_PER_DAY;
    return std::min(fine, MAX_FINE);
}

int FineCalculator::overdueDays(long long borrowTimestamp, long long currentTimestamp) {
    long long dueTimestamp = borrowTimestamp + (LOAN_PERIOD_DAYS * 86400LL);
    if (currentTimestamp <= dueTimestamp) return 0;
    return static_cast<int>((currentTimestamp - dueTimestamp) / 86400);
}
