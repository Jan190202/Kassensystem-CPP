#pragma once
#include <cstdint>

enum class BalanceType : uint8_t
{
	none = 0,			// 0000.0000
	earning = 1 << 0,	// 0000.0001
	spending = 1 << 1,	// 0000.0010
	supplement = 1 << 2 // 0000.0100
	// supplements: drink sales, rounding error at payForeignShare()
};

enum class OverpaymentDisposition
{
	credit, tip
};

enum class FilterType
{
	includeFullyPaid, omitFullyPaid
};

enum class GetEntryException
{
	entryNotFound, multipleEntriesFound
};

enum class FinancialShare
{
	all, foreign, own
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