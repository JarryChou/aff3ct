#ifndef FACTORY_DECODER_NO_HPP
#define FACTORY_DECODER_NO_HPP

#include <string>

#include "Module/Decoder/Decoder_SISO_SIHO.hpp"

#include "../Decoder.hpp"

namespace aff3ct
{
namespace factory
{
struct Decoder_NO : public Decoder
{
	static const std::string name;
	static const std::string prefix;

	class parameters : public Decoder::parameters
	{
	public:
		// ------------------------------------------------------------------------------------------------- PARAMETERS
		// empty

		// ---------------------------------------------------------------------------------------------------- METHODS
		parameters(const std::string p = Decoder_NO::prefix);
		virtual ~parameters();
		Decoder_NO::parameters* clone() const;

		// parameters construction
		void get_description(tools::Argument_map_info &req_args, tools::Argument_map_info &opt_args) const;
		void store          (const tools::Argument_map_value &vals                                 );
		void get_headers    (std::map<std::string,header_list>& headers, const bool full = true) const;

		// builder
		template <typename B = int, typename Q = float>
		module::Decoder_SISO_SIHO<B,Q>* build() const;
	};

	template <typename B = int, typename Q = float>
	static module::Decoder_SISO_SIHO<B,Q>* build(const parameters &params);
};
}
}

#endif /* FACTORY_DECODER_NO_HPP */
