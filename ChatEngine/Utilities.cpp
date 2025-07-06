#include "pch.hpp"
#include "Utilities.hpp"

const std::wstring alphabet = L"abcdefghigklmnopqrstuvwxyz";

boost::random::mt19937 rng;
boost::random::uniform_real_distribution<double> gen(0.0, 1.0);

std::wstring utility::GenerateNickname()
{
	std::wstring buffer;
	buffer.reserve(6);
	std::wostringstream woss(buffer);

	woss << alphabet[ (size_t)(gen(rng) * (alphabet.size() - 1)) ];
	woss << alphabet[ (size_t)(gen(rng) * (alphabet.size() - 1)) ];
	woss << (int)(gen(rng) * 9);
	woss << (int)(gen(rng) * 9);
	woss << (int)(gen(rng) * 9);
	woss << (int)(gen(rng) * 9);

	return buffer;
}
