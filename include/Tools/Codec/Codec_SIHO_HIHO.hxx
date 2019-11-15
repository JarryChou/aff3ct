#include "Tools/Codec/Codec_SIHO_HIHO.hpp"

namespace aff3ct
{
namespace tools
{

template <typename B, typename Q>
Codec_SIHO_HIHO<B,Q>
::Codec_SIHO_HIHO(const int K, const int N_cw, const int N, const int n_frames)
: Codec     <B,Q>(K, N_cw, N, n_frames),
  Codec_SIHO<B,Q>(K, N_cw, N, n_frames),
  Codec_HIHO<B,Q>(K, N_cw, N, n_frames)
{
}

template <typename B, typename Q>
void Codec_SIHO_HIHO<B,Q>
::set_decoder_siho_hiho(std::shared_ptr<module::Decoder_SIHO_HIHO<B,Q>> dec)
{
	this->set_decoder_siho(dec);
	this->set_decoder_hiho(dec);
}

template <typename B, typename Q>
void Codec_SIHO_HIHO<B,Q>
::set_decoder_siho_hiho(module::Decoder_SIHO_HIHO<B,Q>* dec)
{
	this->set_decoder_siho_hiho(std::shared_ptr<module::Decoder_SIHO_HIHO<B,Q>>(dec));
}

}
}
