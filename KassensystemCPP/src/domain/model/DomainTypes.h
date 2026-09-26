#pragma once
#include <cstdint>

// TBD: to lower of all enum fields

enum class BalanceType : uint8_t
{
	None = 0,			// 0000.0000
	Earning = 1 << 0,	// 0000.0001
	Spending = 1 << 1,	// 0000.0010
	Supplement = 1 << 2 // 0000.0100
	// supplements: drink sales, rounding error at payForeignShare()
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

constexpr BalanceType operator|(BalanceType a, BalanceType b)
{
	return static_cast<BalanceType>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

constexpr BalanceType operator&(BalanceType a, BalanceType b)
{
	return static_cast<BalanceType>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

constexpr BalanceType operator^(BalanceType a, BalanceType b)
{
	return static_cast<BalanceType>(static_cast<uint8_t>(a) ^ static_cast<uint8_t>(b));
}

constexpr bool hasFlag(BalanceType value, BalanceType flag)
{
	return static_cast<uint8_t>(value) & static_cast<uint8_t>(flag);
}