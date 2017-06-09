#include <typeinfo>
#include <stdexcept>

#include "Modem_BPSK_fast.hpp"

using namespace aff3ct::module;

template <typename B, typename R, typename Q>
Modem_BPSK_fast<B,R,Q>
::Modem_BPSK_fast(const int N, const R sigma, const bool disable_sig2, const int n_frames, const std::string name)
: Modem<B,R,Q>(N, sigma, n_frames, name),
  disable_sig2(disable_sig2), two_on_square_sigma((R)2.0 / (sigma * sigma))
{
}

template <typename B, typename R, typename Q>
Modem_BPSK_fast<B,R,Q>
::~Modem_BPSK_fast()
{
}

template <typename B, typename R, typename Q>
void Modem_BPSK_fast<B,R,Q>
::set_sigma(const R sigma)
{
	Modem<B,R,Q>::set_sigma(sigma);
	two_on_square_sigma = (R)2.0 / (sigma * sigma);
}


template <typename B, typename R, typename Q>
void Modem_BPSK_fast<B,R,Q>
::modulate(const B *X_N1, R *X_N2)
{
	throw std::runtime_error("aff3ct::module::Modem_BPSK_fast: this type of data is not supported in the "
	                         "\"_modulate\" method.");
}

namespace aff3ct
{
namespace module
{
template <>
void Modem_BPSK_fast<int, float, float>
::modulate(const int *X_N1, float *X_N2)
{
	auto size = (unsigned int)(this->N * this->n_frames);

	const auto vec_loop_size = (size / mipp::nElReg<int>()) * mipp::nElReg<int>();
	const mipp::Reg<float> one = 1.f;
	for (unsigned i = 0; i < vec_loop_size; i += mipp::nElReg<int>())
	{
		auto x1b = mipp::Reg<int>(&X_N1[i]);
		auto x1r = x1b.cvt<float>();
		auto x2r = one - (x1r + x1r);
		x2r.store(&X_N2[i]);
	}

	for (unsigned i = vec_loop_size; i < size; i++)
		X_N2[i] = (float)((int)1 - (X_N1[i] + X_N1[i])); // (X_N[i] == 1) ? -1 : +1
}
}
}

namespace aff3ct
{
namespace module
{
template <>
void Modem_BPSK_fast<short, float, float>
::modulate(const short *X_N1, float *X_N2)
{
	auto size = (unsigned int)(this->N * this->n_frames);

	const auto vec_loop_size = size / mipp::nElReg<short>();
	const mipp::Reg<float> one = 1.f;
	for (unsigned i = 0; i < vec_loop_size; i++)
	{
		auto x1b = mipp::Reg<short>(&X_N1[i * mipp::nElReg<short>()]);
		auto x1b_low  = x1b.low ().cvt<int>();
		auto x1b_high = x1b.high().cvt<int>();

		auto x1r_low  = x1b_low .cvt<float>();
		auto x1r_high = x1b_high.cvt<float>();

		auto x2r_low  = one - (x1r_low  + x1r_low);
		auto x2r_high = one - (x1r_high + x1r_high);

		x2r_low .store(&X_N2[i * 2 * mipp::nElReg<float>() + 0 * mipp::nElReg<float>()]);
		x2r_high.store(&X_N2[i * 2 * mipp::nElReg<float>() + 1 * mipp::nElReg<float>()]);
	}

	for (unsigned i = vec_loop_size * mipp::nElReg<short>(); i < size; i++)
		X_N2[i] = (float)((short)1 - (X_N1[i] + X_N1[i])); // (X_N[i] == 1) ? -1 : +1
}
}
}

namespace aff3ct
{
namespace module
{
template <>
void Modem_BPSK_fast<signed char, float, float>
::modulate(const signed char *X_N1, float *X_N2)
{
	auto size = (unsigned int)(this->N * this->n_frames);

	const auto vec_loop_size = size / mipp::nElReg<signed char>();
	const mipp::Reg<float> one = 1.f;
	for (unsigned i = 0; i < vec_loop_size; i++)
	{
		auto x1b = mipp::Reg<signed char>(&X_N1[i * mipp::nElReg<signed char>()]);
		auto x1b_low  = x1b.low ().cvt<short>();
		auto x1b_high = x1b.high().cvt<short>();

		auto x1b_low_low   = x1b_low .low ().cvt<int>();
		auto x1b_low_high  = x1b_low .high().cvt<int>();
		auto x1b_high_low  = x1b_high.low ().cvt<int>();
		auto x1b_high_high = x1b_high.high().cvt<int>();

		auto x1r_low_low   = x1b_low_low  .cvt<float>();
		auto x1r_low_high  = x1b_low_high .cvt<float>();
		auto x1r_high_low  = x1b_high_low .cvt<float>();
		auto x1r_high_high = x1b_high_high.cvt<float>();

		auto x2r_low_low   = one - (x1r_low_low   + x1r_low_low  );
		auto x2r_low_high  = one - (x1r_low_high  + x1r_low_high );
		auto x2r_high_low  = one - (x1r_high_low  + x1r_high_low );
		auto x2r_high_high = one - (x1r_high_high + x1r_high_high);

		x2r_low_low  .store(&X_N2[i * 4 * mipp::nElReg<float>() + 0 * mipp::nElReg<float>()]);
		x2r_low_high .store(&X_N2[i * 4 * mipp::nElReg<float>() + 1 * mipp::nElReg<float>()]);
		x2r_high_low .store(&X_N2[i * 4 * mipp::nElReg<float>() + 2 * mipp::nElReg<float>()]);
		x2r_high_high.store(&X_N2[i * 4 * mipp::nElReg<float>() + 3 * mipp::nElReg<float>()]);
	}

	for (unsigned i = vec_loop_size * mipp::nElReg<signed char>(); i < size; i++)
		X_N2[i] = (float)((signed char)1 - (X_N1[i] + X_N1[i])); // (X_N[i] == 1) ? -1 : +1
}
}
}

template <typename B,typename R, typename Q>
void Modem_BPSK_fast<B,R,Q>
::filter(const R *Y_N1, R *Y_N2)
{
	std::copy(Y_N1, Y_N1 + this->N_fil * this->n_frames, Y_N2);
}

template <typename B, typename R, typename Q>
void Modem_BPSK_fast<B,R,Q>
::demodulate(const Q *Y_N1, Q *Y_N2)
{
	if (typeid(R) != typeid(Q))
		throw std::invalid_argument("aff3ct::module::Modem_BPSK_fast: type \"R\" and \"Q\" have to be the same.");

	if (typeid(Q) != typeid(float) && typeid(Q) != typeid(double))
		throw std::invalid_argument("aff3ct::module::Modem_BPSK_fast: type \"Q\" has to be float or double.");

	if (disable_sig2)
		std::copy(Y_N1, Y_N1 + this->N * this->n_frames, Y_N2);
	else
	{
		auto size = (unsigned int)(this->N * this->n_frames);
		auto vec_loop_size = (size / mipp::nElReg<Q>()) * mipp::nElReg<Q>();
		for (unsigned i = 0; i < vec_loop_size; i += mipp::nElReg<Q>())
		{
			auto y = mipp::Reg<Q>(&Y_N1[i]) * (Q)two_on_square_sigma;
			y.store(&Y_N2[i]);
		}
		for (unsigned i = vec_loop_size; i < size; i++)
			Y_N2[i] = Y_N1[i] * (Q)two_on_square_sigma;
	}
}

template <typename B, typename R, typename Q>
void Modem_BPSK_fast<B,R,Q>
::demodulate(const Q *Y_N1, const Q *Y_N2, Q *Y_N3)
{
	if (typeid(R) != typeid(Q))
		throw std::invalid_argument("aff3ct::module::Modem_BPSK_fast: type \"R\" and \"Q\" have to be the same.");

	if (typeid(Q) != typeid(float) && typeid(Q) != typeid(double))
		throw std::invalid_argument("aff3ct::module::Modem_BPSK_fast: type \"Q\" has to be float or double.");

	auto size = (unsigned int)(this->N * this->n_frames);
	if (disable_sig2)
	{
		auto vec_loop_size = (size / mipp::nElReg<Q>()) * mipp::nElReg<Q>();
		for (unsigned i = 0; i < vec_loop_size; i += mipp::nElReg<Q>())
		{
			auto y = mipp::add(mipp::Reg<Q>(&Y_N1[i]), mipp::Reg<Q>(&Y_N2[i]));
			y.store(&Y_N3[i]);
		}
		for (unsigned i = vec_loop_size; i < size; i++)
			Y_N3[i] = Y_N1[i] + Y_N2[i];
	}
	else
	{
		auto vec_loop_size = (size / mipp::nElReg<Q>()) * mipp::nElReg<Q>();
		for (unsigned i = 0; i < vec_loop_size; i += mipp::nElReg<Q>())
		{
			// mipp::fmadd(a, b, c) = a * b + c
			auto y = mipp::fmadd(mipp::Reg<Q>(&Y_N1[i]), mipp::Reg<Q>((Q)two_on_square_sigma), mipp::Reg<Q>(&Y_N2[i]));
			y.store(&Y_N3[i]);
		}
		for (unsigned i = vec_loop_size; i < size; i++)
			Y_N3[i] = (Y_N1[i] * (Q)two_on_square_sigma) + Y_N2[i];
	}
}

// ==================================================================================== explicit template instantiation 
#include "Tools/types.h"
#ifdef MULTI_PREC
template class aff3ct::module::Modem_BPSK_fast<B_8,R_8,R_8>;
template class aff3ct::module::Modem_BPSK_fast<B_8,R_8,Q_8>;
template class aff3ct::module::Modem_BPSK_fast<B_16,R_16,R_16>;
template class aff3ct::module::Modem_BPSK_fast<B_16,R_16,Q_16>;
template class aff3ct::module::Modem_BPSK_fast<B_32,R_32,R_32>;
template class aff3ct::module::Modem_BPSK_fast<B_64,R_64,R_64>;
#else
template class aff3ct::module::Modem_BPSK_fast<B,R,Q>;
#if !defined(PREC_32_BIT) && !defined(PREC_64_BIT)
template class aff3ct::module::Modem_BPSK_fast<B,R,R>;
#endif
#endif
// ==================================================================================== explicit template instantiation