#pragma once
#include "domain/model/Entities.h"
#include <vector>

class FinancialReviewRepository
{
public:
	FinancialReviewRepository() = default;
	virtual ~FinancialReviewRepository() = default;

	virtual int64_t addFinancialReviewEntry(const entry::FinancialReview& entry) = 0; // add review entry to database, return associated ID
	virtual std::vector<entry::FinancialReview> getAllFinancialReviewEntries() const = 0; // all review entries in any order (the service sorts by dateTime)
};