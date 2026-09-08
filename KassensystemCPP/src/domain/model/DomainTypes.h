#pragma once

enum class BalanceType
{
	Earning, EarningAndSupplement, Spending, EarningAndSpending // supplements: drink sales, rounding error at payForeignShare()
};

enum class OverpaymentDisposition
{
	Credit, Tip
};

enum class FilterType
{
	IncludeFullyPaid, OmitFullyPaid
};

enum class GetEntryException
{
	EntryNotFound, MultipleEntriesFound
};

enum class FinancialShare
{
	All, Foreign, Own
};