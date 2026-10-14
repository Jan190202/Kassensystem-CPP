#pragma once
#include "domain/repointerface/FinancialReviewRepository.h"

class SqliteFinancialReviewRepository : public FinancialReviewRepository
{
public:
	SqliteFinancialReviewRepository() = default;

	int64_t addFinancialReviewEntry(const entry::FinancialReview& entry) override;
	std::vector<entry::FinancialReview> getAllFinancialReviewEntries() const override;
};