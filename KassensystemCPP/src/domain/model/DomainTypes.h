#pragma once

enum class BalanceType
{
	Earning, Spending, All
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