#include <limits>
#include <sstream>
#include <typeinfo>

#include "Tools/Exception/exception.hpp"
#include "Tools/Math/utils.h"

#include "Decoder_LDPC_bit_flipping_OMWBF.hpp"

using namespace aff3ct;
using namespace aff3ct::module;

template <typename B, typename R>
Decoder_LDPC_bit_flipping_OMWBF<B,R>
::Decoder_LDPC_bit_flipping_OMWBF(const int &K, const int &N, const int& n_ite,
                                const tools::Sparse_matrix &H,
                                const std::vector<unsigned> &info_bits_pos,
//                                const float normalize_factor,
                                const R mwbf_factor,
                                const bool enable_syndrome,
                                const int syndrome_depth,
                                const int n_frames,
                                const std::string name)
: Decoder(K, N, n_frames, 1),
  Decoder_LDPC_bit_flipping<B,R>(K, N, n_ite, H, info_bits_pos,mwbf_factor , enable_syndrome, syndrome_depth, n_frames, name)
{
	if (typeid(R) == typeid(signed char))
	{
		std::stringstream message;
		message << "This decoder does not work in 8-bit fixed-point (try in 16-bit).";
		throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
	}
}

template <typename B, typename R>
Decoder_LDPC_bit_flipping_OMWBF<B,R>
::~Decoder_LDPC_bit_flipping_OMWBF()
{
}

template <typename B, typename R>
bool Decoder_LDPC_bit_flipping_OMWBF<B,R>
::BF_process(const R *Y_N, std::vector<R> &V_to_C, std::vector<R> &C_to_V, const R *Y_min, short *decis)
{
	short synd[this->n_C_nodes];
	bool syndrome = 0;
	R energy[this->n_V_nodes];

	R alpha = 0.4;

	for (auto i = 0; i < this->n_C_nodes; ++i)
	{
		synd[i] = 0;

		for (auto j = 0; j < this->n_variables_per_parity[i]; ++j)
			synd[i] ^= decis[this->H.get_rows_from_col(i).at(j)];

		syndrome |= synd[i];
	}

	for (auto i = 0; i < this->n_V_nodes; ++i)
	{
		energy[i] = 0;
		for (auto j = 0; j < this->n_parities_per_variable[i]; ++j)
		{
			auto m = this->H.get_cols_from_row(i)[j];
			energy[i] += (2 * synd[m] - 1) * Y_min[m];
		}
		energy[i] -= this->mwbf_factor * (R)std::abs(Y_N[i]); 
	}

	auto indMax = 0;
	R valMax = energy[0];

	for (auto i = 1; i < this->n_V_nodes; ++i)
	{
		if (valMax < energy[i])
		{
			valMax = energy[i];
			indMax = i;
		}
	}
	if (syndrome == 1)
		decis[indMax] = (decis[indMax] == 0)?1:0;
	return !syndrome;
}

// ==================================================================================== explicit template instantiation 
#include "Tools/types.h"
#ifdef MULTI_PREC
template class aff3ct::module::Decoder_LDPC_bit_flipping_OMWBF<B_8,Q_8>;
template class aff3ct::module::Decoder_LDPC_bit_flipping_OMWBF<B_16,Q_16>;
template class aff3ct::module::Decoder_LDPC_bit_flipping_OMWBF<B_32,Q_32>;
template class aff3ct::module::Decoder_LDPC_bit_flipping_OMWBF<B_64,Q_64>;
#else
template class aff3ct::module::Decoder_LDPC_bit_flipping_OMWBF<B,Q>;
#endif
// ==================================================================================== explicit template instantiation

